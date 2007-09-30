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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "interpreter.c"
#include "tsh.c"



static struct jobs *jobs_init(void) {
  struct jobs *j;
  j = malloc(sizeof(struct jobs));
  j->fg = -1;
  j->n = 0;
  j->s = 0;
  j->job = (struct task **)malloc(sizeof(struct task*) * 64);
  j->stop = (struct task **)malloc(sizeof(struct task*) * 64);
  return j;
}

struct tnode *new_tnode(void) {
  struct tnode *nodelist;
  nodelist = malloc(sizeof(struct tnode));
  nodelist->fin = NULL;
  nodelist->fout = NULL;
  nodelist->next = NULL;
  nodelist->flags = 0;
  nodelist->type = -1;
  nodelist->argv = malloc(sizeof(char*)*128);
  return nodelist;
}

struct tnode *newnode(cmd_ptr *cmdptr) {
  int j, n;
  char *token, *tempstring;
  struct tnode *t = NULL, *t1=NULL;
  for (j = 0;j < cmdptr->number_cmd;j++) {
    if (t != NULL) {
      t->next = new_tnode();
      t = t->next;
    } else
      t = new_tnode();
    if (j == 0)
      t1 = t;
    t->type = TPIPE;
    t->argv[0] = malloc(sizeof(char) * strlen(cmdptr->ptr[j]->cmdname));
    strcpy(t->argv[0], cmdptr->ptr[j]->cmdname);
    tempstring = malloc(sizeof(char) * strlen((cmdptr->ptr[j]->arg) + 1));
    strcpy(tempstring, cmdptr->ptr[j]->arg);
    token = strtok(tempstring, " ");
    for (n = 0;n < cmdptr->ptr[j]->number_arg;n++) {
      t->argv[n+1] = malloc(sizeof(char) * (strlen(token) + 1));
      strcpy(t->argv[n+1], token);
      token = strtok(NULL, " ");
    }
  }
  if (strlen(cmdptr->head)) {
    t1->fin = malloc(sizeof(char) * strlen((cmdptr->head) + 1));
    strcpy(t1->fin, cmdptr->head);
    t1->flags |= FFIN;
  }
  if (strlen(cmdptr->tail)) {
    t1->fout = malloc(sizeof(char) * strlen((cmdptr->tail) + 1));
    strcpy(t1->fout, cmdptr->tail);
  }
  if (cmdptr->indicater == 2)
    t1->flags |= FAPN;
  free(cmdptr);
  return t1;
}

static struct tnode *bnode(char *line) {
  char *token;
  struct tnode *t;
  int i = 0;
  t = new_tnode();
  t->type = TBDIN;
  token = strtok(line, " ");
  while (token != NULL) {
    t->argv[i] = malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(t->argv[i++], token);
    token = strtok(NULL, " ");
  }
  t->argv[i] = NULL;
  return t;
}

int main(int argc, char *argv[], char *envp[]) {
  char line[256];
  char ch;
  char *pwd;
  int i;
  cmd_ptr *cmdptr = NULL;

  pwd = malloc(sizeof(char) * 1024);
  jlist = jobs_init();
  //signal(SIGCHLD, SIG_DFL);
  signal(SIGINT,  SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGSTOP, SIG_IGN);


  for (;;) {
    struct task *newtask;
    struct tnode *nodelist;
    strcpy(pwd, getenv("PWD"));
    printf("[CSC3150 shell:%s]$", pwd);

    line[0] = 0;
    i = 0;
    while ((ch = getchar()) != '\n')
      line[i++] = ch;
    line[i] = 0;
    if (line[0] == 0)
      continue;
    //temp=malloc(sizeof(char)*(strlen(line)+1));
    //strcpy(temp,line);
    if (!firstck(line)) {
      printf("Error: invalid input command line\n");
      continue;
    }
    //printf("Error\n");
    cmdptr = malloc(sizeof(cmd_ptr));
    initialize2(cmdptr);
    newtask = malloc(sizeof(struct task));
    newtask->cmd = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(newtask->cmd, line);
    jlist->job[jlist->n++] = newtask;

    if (start_chk(line) == 2) {
      nodelist = bnode(line);
      exec1(nodelist);
      continue;
    }
    if (start_chk(line) == 3)
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

    else {
      printf("Error: invalid input command line\n");
      continue;
    }


    nodelist = newnode(cmdptr);
    newtask->tids = execute(nodelist, NULL, NULL);
    if (newtask->tids != NULL) {
      jlist->job[jlist->n] = newtask;
      jlist->n++;
    }
    lwait(newtask->tids);

  }


  return 0;
}
