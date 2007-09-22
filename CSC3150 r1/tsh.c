#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


struct tnode {
  struct tnode *next;
  int type;
  int flags;
  char **argv;
  char *fin;
  char *fout;
};

/*
 * Node Type
 * */
#define TBDIN 0
#define TPIPE 1

/*
 * Node Flags
 */
#define FFIN 0001 // redirect stdin from file
#define FAPN 0002 // append to output file
#define FPIN 0004 // redirect stdin from pipe
#define FPOUT 0010 // redirect stdout to pipe


static void execute(struct tnode *t, int *pin, int *pout);
static void exec1(struct tnode *t);
static void exec2(struct tnode *t, int *pin, int *pout);
static void pwait(pid_t cp);


static void execute(struct tnode *t, int  *pin, int *pout) {
  struct tnode *t1;
  pid_t cpid;
  int f, pfd[2];

  if (t == NULL)
    return;
  switch (t->type) {
  case TBDIN:
    if (t->argv == NULL || t->argv[0] == NULL) {
      perror("Invalid command");
      return;
    }
    //exec1(t);
    return;
  case TPIPE:
    if ((t1 = t->next) == NULL) {
      exec2(t, pin, NULL);
      return;
    }
    if (pipe(pfd) == -1) {
      perror("Cannot create pipe");
      if (pin != NULL) {
        close(pin[0]);
        close(pin[1]);
      }
      return;
    }
    exec2(t, pin, pfd);
    t->flags |= FPOUT;
    f = t->flags;
    t1->flags |= FPIN | (f & FAPN);
    t1->fout = t->fout;
    execute(t1, pfd, pout);
    close(pfd[0]);
    close(pfd[1]);
    return;
  }
}

static void exec2(struct tnode *t, int *pin, int *pout) {
  struct tnode *t1;
  int f, i, cpid;
  const char *cmd;

  f = t->flags;
  if ((cpid = fork()) == -1) {
    perror("fork failed!");
    return;
  }
  /****** Parent! ******/
  if (cpid != 0) {
    if (pin != NULL && (f & FPIN) != 0) {
      close(pin[0]);
      close(pin[1]);
    }
    if ((f&FPOUT) == 0)
      pwait(cpid);
    return;
  }
  /****** Child! ******/

  /* Redirect from/to pipe */
  if (pin != NULL && (f&FPIN) != 0) {
    if (dup2(pin[0], STDIN_FILENO) == -1)
      perror("File redirect faild");
    close(pin[0]);
    close(pin[1]);
  }

  if (pout != NULL && (f&FPOUT) != 0) {
    if (dup2(pout[1], STDOUT_FILENO) == -1)
      perror("File redirect faild");
    close(pout[0]);
    close(pout[1]);
  }

  /* Redirect from/to file */
  if (t->fin != NULL && (f&FPIN) == 0) {
    f |= FFIN;
    i = open(t->fin, O_RDONLY);
    if (i == -1)
      perror("Input file open failed");
    if (dup2(i, STDIN_FILENO) == -1)
      perror("File redirect faild");
    close(i);
  }

  if (t->fout != NULL && (f&FPOUT) == 0) {
    if ((f&FAPN) != 0)
      i = O_WRONLY | O_APPEND | O_CREAT;
    else
      i = O_WRONLY | O_TRUNC | O_CREAT;
    i = open(t->fout, i, 0666) ;
    if (i == -1)
      perror("Output file open failed");
    if (dup2(i, STDOUT_FILENO) == -1)
      perror("File redirect faild");
    close(i);
  }

  execvp(t->argv[0], (char *const *)t->argv);
  /* ERROR HERE!!! */
  perror("Execute failed");
}


static void pwait(pid_t cp) {
  pid_t tp;
  int s;

  if (cp == 0)
    return;
  for (;;) {
    tp = wait(&s);
    if (tp == -1)
      break;
  //if (s != 0) {}
    if (tp == cp)
      break;
  }
}
