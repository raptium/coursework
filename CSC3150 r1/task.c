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


void initialzie_jobs(struct jobs* jlist) {
  jlist = malloc(sizeof(struct jobs));
  jlist->n = 0;
  jlist->s = 0;
  jlist->fg = -1;
}

struct tnode *initialzie_tnode(void) {
  nodelist = malloc(sizeof(struct tnode));
  nodelist->fin = NULL;
  nodelist->fout = NULL;
  nodelist->next = NULL;
  nodelist->flags = 0;
  nodelist->type = -1;
  return nodelist;
}

struct tnode * newnode(cmd_ptr *cmdptr) {
  int j, n;
  char *token, *tempstring;
  struct tnode* tempnode, *tempnode2;
  for (j = 0;j < cmdptr->number_cmd;j++) {
    struct tnode* nodelist;
    nodelist = initialzie_tnode();
    nodelist->argv[0] = malloc(sizeof(char) * strlen(cmdptr->ptr[j]->cmdname));
    nodelist->argv[0] = strcpy(cmdptr->ptr[j]->cmdname);
    tempstring = malloc(sizeof(char) * strlen((cmdptr->ptr[j]->arg) + 1);
                        strcpy(tempstring, cmdptr->ptr[j]->arg);
                        token = strtok(tempstring, " ");
    for (n = 0;n < cmdptr->ptr[j]->number_arg;n++) {
    nodelist->argv[n+1] = malloc(sizeof(char) * (strlen(token) + 1));
      strcpy(nodelist->argv[n+1], token);
      token = strtok(NULL, " ");
    }
    if (j == 0) {
    tempnode = nodelist;
    tempnode2 = nodelist;
  } else {
    tempnode->next = nodelist;
    tempnode = nodelist;
  }
}
if (strlen(cmdptr->head)) {
    tempnode2->fin = malloc(sizeof(char) * strlen((cmdptr->head) + 1));
    strcpy(tempnode2->fin, cmdptr->head);
    tempnode2->flags = FFIN;
  }
  if (strlen(cmdptr->tail)) {
    tempnode2->fout = malloc(sizeof(char) * strlen((cmdptr->tail) + 1));
    strcpy(tempnode2->fout, cmdptr->tail);
  }
  if (cmdptr->indicater == 2)
    tempnode2->flags = tempnode2->flags | FAPN;
  return tempnode2;
}


int main(int argc, char *argv[], char *envp[]) {
  char line[256];
  char ch;
  char cmd[256];
  char *pwd;
  int input;
  int i;
  taskADT task;
  tasksADT tasks;
  int clean;
  cmd_ptr *cmdptr = NULL;
  pid_t pid;


  pwd = malloc(sizeof(char) * 1024);
  strcpy(pwd, getenv("PWD"));
  initialzie_jobs(jlist);
  //signal(SIGCHLD, SIG_DFL);
  signal(SIGINT,  SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGSTOP, SIG_IGN);



  for (;;) {
    struct task *newtask;
    struct tnode *nodelist,
          printf("%s$", pwd);

    line[0] = 0;
    i = 0;
    while ((ch = getchar()) != '\n')
      line[i++] = ch;
    line[i] = 0;
    if (line[0] == 0)
      continue;

    cmdptr = malloc(sizeof(cmd_ptr));
    initialize2(cmdptr);
    newtask = malloc(sizeof(struct task));
    newtask->cmd = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(newtask->cmd, line);
    jlist->job[jlist->n++] = newtask;

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


    nodelist = newnode(cmdptr);
    newtask->tid = execute(t, NULL, NULL, 0);
  }


  return 0;
}
