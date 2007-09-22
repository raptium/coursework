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
#include "1.c"
#include "tsh.c"



struct taskCDT {
  int argc;
  pid_t pid;
  char **argv;
  char *pwd;
  int input, output;
  int c1, c2;
};

struct tasksCDT {
  char *head, *tail;
  int append;
  int total;
  taskADT *list;
};

struct jobsCDT {
  //taskADT *bg;
  //taskADT fg;
  pid_t *bg;
  pid_t fg;
  int bgn;
};

jobsADT jobs;

jobsADT new_jobs(void) {
  jobsADT jobs;

  jobs = malloc(sizeof(struct jobsCDT));
  jobs->fg = 0;
  jobs->bg = malloc(sizeof(pid_t) * 32);
  jobs->bgn = 0;
  return jobs;
}

void bg_job(jobsADT jobs) {

  jobs->bg[jobs->bgn++] = jobs->fg;
  jobs->fg = 0;
  return;
}

void add_job(jobsADT jobs, pid_t pid) {
  int i;

  if (fg(jobs))
    bg_job(jobs);
  jobs->fg = pid;
  return;
}




void del_job(jobsADT jobs, pid_t pid) {
  int i, j;

  if (jobs->fg == pid) {
    jobs->fg = 0;
    return;
  }

  for (i = 0;i < jobs->bgn;i++)
    if (jobs->bg[i] == pid) {
      for (j = i;j < jobs->bgn - 1;j++)
        jobs->bg[j] = jobs->bg[j+1];
      jobs->bgn--;
    }

  return;
}


void update_job(jobsADT jobs) {
  int i = 0;

  while (jobs->bg[i] != 0)
    i++;
  jobs->bgn = i;
  return;
}

int jobs_clean(jobsADT jobs) {
  return !fg(jobs);
}

int fg(jobsADT jobs) {
  return jobs->fg;
}







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
  task->c1 = -1;
  task->c2 = -1;

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
  static int c1 = -1, c2 = -1;

  if (pid) {
    add_job(jobs, pid);
    task->pid = pid;
    if (c1 > 0)
      close(c1);
    c1 = 0;
    if (task->input != 0) {
      close(task->input);
      c1 = task->c1;
    }
    if (c2 > 0)
      close(c2);
    c2 = 0;
    if (task->output != 1) {
      close(task->output);
      c2 = task->c2;
    }
  } else {
    if (task->input != 0) {
      dup2(task->input, 0);
      //close(task->input);
      close(task->c1);
    }
    if (task->output != 1) {
      dup2(task->output, 1);
      //close(task->output);
      close(task->c2);
    }

    printf("%d>>%s>>%d\n", task->input, task->argv[0], task->output);
    execvp(task->argv[0], task->argv);
    exit(-1);
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
  int op[2], cp[2];
  int fin = 0, fout = 1;
  int i, n;

  if (strlen(tasks->head))
    fin = open(tasks->head, O_RDONLY);
  if (strlen(tasks->tail))
    if (tasks->append)
      fout = open(tasks->tail, O_CREAT | O_WRONLY | O_APPEND, 0644);
    else
      fout = open(tasks->tail, O_CREAT | O_WRONLY | O_TRUNC, 0644);

  tasks->list[0]->input = fin;
  if (tasks->total > 1) {
    pipe(cp);
    printf("pipe created %d <--> %d\n", cp[0], cp[1]);
  }
  tasks->list[0]->output = cp[1];
  tasks->list[0]->c2 = cp[0];
  op[0] = cp[0];
  op[1] = cp[1];
  taskExec(tasks->list[0]);
  if (tasks->total == 1)
    return 0;

  for (i = 1;i < tasks->total - 1;i++) {
    pipe(cp);
    printf("pipe created %d <--> %d\n", cp[0], cp[1]);
    tasks->list[i]->input = op[0];
    tasks->list[i]->c1 = op[1];
    tasks->list[i]->output = cp[1];
    tasks->list[i]->c2 = cp[0];
    op[0] = cp[0];
    op[1] = cp[1];
    taskExec(tasks->list[i]);
  }


  n = tasks->total - 1;
  tasks->list[n]->input = op[0];
  tasks->list[n]->c1 = op[1];
  tasks->list[n]->output = fout;
  taskExec(tasks->list[n]);

  return 0;

}



void sigchld_handler(int sig) {
  int status;
  pid_t pid;
  if (sig == SIGCHLD) {
    //waitpid(-1,NULL,WNOHANG|WUNTRACED);
    pid = wait(&status);
    //printf("Handle zombie pid=%d fg=%d\n",pid,fg);
    if (WIFSTOPPED(status)) {
      printf("bg jos\n");
      bg_job(jobs);
    } else
      del_job(jobs, pid);
  }
}

void sig_ignore(int sig) {
  if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT || sig == SIGTSTP || sig == SIGSTOP) {
    return;
  }
}




static void fgout(void) {
  static int i = 0;
  //printf("[%d] Susspended. pid=%d\n",i,(int)fg);
  i++;
  // fg=-1;
}

void sigtstp_rerouter(int sig) {

  printf("%d is going to be susspended.\n", fg(jobs));
  if (sig == SIGTSTP) {
    if (fg(jobs)) {
      kill(fg(jobs), SIGTSTP);
      bg_job(jobs);
    }
  }
}



int main(int argc, char *argv[], char *envp[]) {
  char line[256];
  char cmd[256];
  char *pwd;
  int input;
  int i;
  taskADT task;
  tasksADT tasks;
  int clean;
  cmd_ptr *cmdptr = NULL;
  struct tnode *t, *t1;

  pwd = malloc(sizeof(char) * 1024);
  strcpy(pwd, getenv("PWD"));


  signal(SIGCHLD, sigchld_handler);
  //  signal(SIGINT,sig_ignore);
  signal(SIGTERM, sig_ignore);
  signal(SIGQUIT, sig_ignore);
  signal(SIGTSTP, sigtstp_rerouter);
  signal(SIGSTOP, sig_ignore);


  jobs = new_jobs();
  for (;;) {

    printf("%s$", pwd);
    gets(line);
    if (!strcmp(line, "exit"))
      return 0;
    if (line[0] == 'c' && line[1] == 'd' && line[2] == ' ') {
      pwd = cmd_cd(line + 3);
      continue;
    }

    cmdptr = malloc(sizeof(cmd_ptr));
    initialize2(cmdptr);

    if (line[strlen(line)-1] == ' ')
      line[strlen(line)-1] = '\0';
    if (start_chk(line) == 2)
      getcmd8(line, cmdptr);
    else if (start_chk(line) == 3)
      getcmd3(line, cmdptr);
    else if (start_chk(line) == 4)
      getcmd4(line, cmdptr);
    else if (start_chk(line) == 5)
      getcmd5(line, cmdptr);
    else if (start_chk(line) == 6)
      getcmd6(line, cmdptr);
    else if (start_chk(line) == 7)
      getcmd7(line, cmdptr);
    else if (start_chk(line) == 8)
      getcmd8(line, cmdptr);


    tasks = tasksNew(cmdptr->head, cmdptr->tail, cmdptr->indicater - 1);

    t = NULL;
    for (i = 0;i < cmdptr->number_cmd;i++) {
      strcpy(cmd, "");
      strcpy(cmd, cmdptr->ptr[i]->cmdname);
      strcat(cmd, cmdptr->ptr[i]->arg);
      task = taskNew(cmd);
      if (t != NULL) {
        t->next = malloc(sizeof(struct tnode));
        t = t->next;
      } else
        t = malloc(sizeof(struct tnode));
      t->type = TPIPE;
      t->flags = 0;
      if (tasks->append)
        t->flags |= FAPN;
      t->argv = task->argv;
      t->fin = cmdptr->head;
      t->fout = cmdptr->tail;
      if (strlen(cmdptr->head) == 0)
        t->fin = NULL;
      if (strlen(cmdptr->tail) == 0)
        t->fout = NULL;
      if (i == 0)
        t1 = t;
    }
    execute(t1, NULL, NULL);
  }
  /*
     task = taskNew("cat");
     task = taskNew("cat");
     tasksInsert(tasks, task);
     task=taskNew("cat");
     tasksInsert(tasks,task);
     tasksExec(tasks);
     for (;;) {
     getchar();
  //printf("waiting for %d\n", tasks->list[i]->pid);
  //waitpid(tasks->list[i]->pid, NULL, 0);
  }
  */

  return 0;
}
