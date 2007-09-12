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
#include <unistd.h>

struct taskCDT{
  int argc;
  pid_t pid;
  char **argv;
  char *pwd;
  int input,output;
};

struct tasksCDT{
  char *head,*tail;
  int create;
  taskADT *list;
};


taskADT taskNew(const char *cmd){
  taskADT task;
  char *cmd_cpy;
  char *token;

  task=malloc(sizeof(struct taskCDT));
  task->argc=0;
  task->pid=-1;
  task->argv=malloc(sizeof(char *)*32);
  task->pwd=malloc(sizeof(char)*1024);
  task->input=0;  //file descriptor for stdin
  task->output=1; //file descriptor for stdout

  cmd_cpy=malloc(sizeof(char)*(strlen(cmd)+1));
  strcpy(cmd_cpy,cmd);
  token=strtok(cmd_cpy," ");
  while(token!=NULL){
    task->argv[task->argc]=malloc(sizeof(char)*(strlen(token)+1));
    strcpy(task->argv[task->argc],token);
    task->argc++;
    token=strtok(NULL," ");
  }

  return task;
}

pid_t taskExec(taskADT task,int input,int output){
  pid_t pid;
  pid=fork();
  if(pid)
    task->pid=pid;
  else
    pid=execvp(task->argv[0],task->argv);
  return pid;
}


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

char *cmd_cd(const char *path){
  char *cwd,*nwd;
  if(path[0]!='/'){
    cwd=getenv("PWD");
    nwd=malloc(sizeof(char)*(strlen(path)+strlen(cwd)+2));
    strcpy(nwd,cwd);
    strcat(nwd,"/");
    strcat(nwd,path);
  }else{
    nwd=malloc(sizeof(char)*(strlen(path)+1));
    strcpy(nwd,path);
  }
  if(!chdir(nwd)){
    nwd=update_cwd(nwd);
    setenv("PWD",nwd,1);
    return nwd;
  }
  return cwd;
}

int main(int argc, char *argv[], char *envp[]){
  char line[256];
  char *pwd;
  taskADT task;

  pwd=getenv("PWD");
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
    if(taskExec(task,0,0)>0)
      waitpid(task->pid,NULL,0);
    else
      return -1;
  }


  return 0;

}



