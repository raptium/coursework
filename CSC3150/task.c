/*
 * CSC3150 Operating System
 * Assignment 1
 * Simple Unix Shell
 *
 *
 * task.c
 * prototype
 *
 *
 */
#include "task.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct taskCDT {
    int argc;
    pid_t pid;
    char **argv;
    char *pwd;
    int input, output;
};

struct tasksCDT {
    char *head, *tail;
    int append;
    int total;
    taskADT *list;
};


taskADT taskNew(const char *cmd) {
    taskADT task;
    char *cmd_cpy;
    char *token;

    task = malloc(sizeof(struct taskCDT));
    task->argc = 0;
    task->pid = -1;
    task->argv = malloc(sizeof(char *) * 32);
    task->pwd = malloc(sizeof(char) * 1024);
    task->input = 0;  //file descriptor for stdin
    task->output = 1; //file descriptor for stdout

    cmd_cpy = malloc(sizeof(char) * (strlen(cmd) + 1));
    strcpy(cmd_cpy, cmd);
    token = strtok(cmd_cpy, " ");
    while (token != NULL) {
        task->argv[task->argc] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(task->argv[task->argc], token);
        task->argc++;
        token = strtok(NULL, " ");
    }

    return task;
}

pid_t taskExec(taskADT task) {
    pid_t pid;
    pid = fork();
    if (pid)
        task->pid = pid;
    else {
        if (task->input != 0) {
            dup2(task->input, 0);
            close(task->input + 1);
        }
        if (task->output != 1) {
            dup2(task->output, 1);
            close(task->output - 1);
        }
        pid = execvp(task->argv[0], task->argv);
    }
    return pid;
}

/*
   char *update_cwd(char *cwd){
   int i=0,n;
   char *nwd;
   char **dir;
   char *token;
   nwd=cwd;
   dir=malloc(sizeof(char *)*128);
   token=strtok(nwd,"/");
   while(token!=NULL){
   if(strcmp(token,".")&&strcmp(token,"")){
   if(!strcmp(token,"..")){
   i-=2;
   if(i<0)
   i=-1;
   free(dir[i+1]);
   }else{
   dir[i]=malloc(sizeof(char)*(strlen(token)+1));
   strcpy(dir[i],token);
   }
   i++;
   }
   token=strtok(NULL,"/");
   }
   n=i;
   strcpy(nwd,"/");
   for(i=0;i<n;i++){
   strcat(nwd,dir[i]);
   strcat(nwd,"/");
   }
   if(nwd[1]!='\0')
   nwd[strlen(nwd)-1]='\0';
   return nwd;
   }


*/

char *cmd_cd(const char *path) {
    char *cwd, *nwd;
    if (path[0] != '/') {
        cwd = getenv("PWD");
        nwd = malloc(sizeof(char) * (strlen(path) + strlen(cwd) + 2));
        strcpy(nwd, cwd);
        strcat(nwd, "/");
        strcat(nwd, path);
    } else {
        nwd = malloc(sizeof(char) * (strlen(path) + 1));
        strcpy(nwd, path);
    }
    if (!chdir(nwd)) {
        getcwd(nwd, 1023);
        setenv("PWD", nwd, 1);
        return nwd;
    }
    return cwd;
}


tasksADT tasksNew(char *head, char *tail, int append) {
    tasksADT tasks;

    tasks = malloc(sizeof(struct tasksCDT));
    tasks->total = 0;
    tasks->append = append;
    tasks->head = head;
    tasks->tail = tail;
    tasks->list = malloc(sizeof(taskADT) * 512);

    return tasks;
}




tasksADT tasksInsert(tasksADT tasks, taskADT task) {
    if (task == NULL || tasks == NULL)
        return tasks;
    tasks->list[tasks->total++] = task;
    return tasks;
}

int tasksExec(tasksADT tasks) {
    int fildes[2];
    int fin = 0, fout = 1;
    int i;

    if (tasks->head != NULL)
        fin = open(tasks->head, O_RDONLY);
    if (tasks->tail != NULL)
        if (tasks->append)
            fout = open(tasks->tail, O_CREAT | O_WRONLY | O_APPEND, 0644);
        else
            fout = open(tasks->tail, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    for (i = 0;i < tasks->total;i++) {
        if (i != tasks->total - 1)
            pipe(fildes);
        else
            fildes[1] = fout;
        if (i != 0) {
            fin = fildes[0];
        }
        printf("%d: %s %d %d\n", i, tasks->list[i]->argv[0], fin, fildes[1]);
        tasks->list[i]->input = fin;
        tasks->list[i]->output = fildes[1];
        taskExec(tasks->list[i]);
    }
}







int main(int argc, char *argv[], char *envp[]) {
    char line[256];
    char *pwd;
    int input;
    int i;
    taskADT task;
    tasksADT tasks;

    pwd = malloc(sizeof(char) * 1024);
    strcpy(pwd, getenv("PWD"));
    /*
       for(;;){
       printf("[%s]$",pwd);
       gets(line);
       if(!strcmp(line,"exit"))
       return 0;
       if(line[0]=='c'&&line[1]=='d'&&line[2]==' '){
       pwd=cmd_cd(line+3);
       continue;
       }

       task=taskNew(line);

       input=open("./input.txt",O_RDONLY);
       if(taskExec(task,input,1)>0)
       waitpid(task->pid,NULL,0);
       else
       return -1;
       }
       */

    tasks = tasksNew("input.txt", "output.txt", 0);
    task = taskNew("cat");
    tasksInsert(tasks, task);
    task = taskNew("cat");
    tasksInsert(tasks, task);
    /*
    task=taskNew("cat");
    tasksInsert(tasks,task);
    */
    tasksExec(tasks);
    for (i = 0;i < tasks->total;i++) {
        printf("waiting for %d\n", tasks->list[i]->pid);
        waitpid(tasks->list[i]->pid, NULL, 0);
        close(tasks->list[i]->output);
    }
    return 0;

}



