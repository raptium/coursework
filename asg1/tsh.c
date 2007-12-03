#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tsh.h"


static struct plist *execute(struct tnode *t, int  *pin, int *pout) {
  struct tnode *t1;
  struct plist *pids;
  int f, pfd[2];

  if (t == NULL)
    return NULL;
  switch (t->type) {
  case TBDIN:
    if (t->argv == NULL || t->argv[0] == NULL) {
      perror("Invalid command");
      return NULL;
    }
    exec1(t);
    return NULL;
  case TPIPE:
    pids = malloc(sizeof(struct plist));
    if ((t1 = t->next) == NULL) {
      pids->pid = exec2(t, pin, NULL);
      pids->next = NULL;
      return pids;
    }
    if (pipe(pfd) == -1) {
      perror("Cannot create pipe");
      if (pin != NULL) {
        close(pin[0]);
        close(pin[1]);
      }
      return NULL;
    }
    pids->pid = exec2(t, pin, pfd);
    t->flags |= FPOUT;
    f = t->flags;
    t1->flags |= FPIN | (f & FAPN);
    t1->fout = t->fout;
    pids->next = execute(t1, pfd, pout);
    close(pfd[0]);
    close(pfd[1]);
    return pids;
  }
  return NULL;
}

static void exec1(struct tnode *t) {
  char cmd[8];
  char arg[1024];

  if (t == NULL)
    return;
  if (t->type != TBDIN)
    return;
  strcpy(cmd, t->argv[0]);
  if (t->argv[1] != NULL)
    strcpy(arg, t->argv[1]);
  else
    strcpy(arg, "");
  if (cmd[0] == 'e') {
    if (jlist->s == 1)
      printf("There is a suspended job.\n");
    else if (jlist->s > 1)
      printf("There are %d suspended jobs.\n", jlist->s);
    else
      exit(0);
  }
  if (cmd[0] == 'c') {
    cmd_cd(arg);
    return;
  }
  if (cmd[0] == 'f') {
    cmd_fg(arg);
    return;
  }
  if (cmd[0] == 'j') {
    cmd_jobs();
    return;
  }
}


static pid_t exec2(struct tnode *t, int *pin, int *pout) {
  int f, i, cpid;

  f = t->flags;
  if ((cpid = fork()) == -1) {
    perror("fork failed!");
    return 0;
  }
  /****** Parent! ******/
  if (cpid != 0) {

    if (pin != NULL && (f & FPIN) != 0) {
      close(pin[0]);
      close(pin[1]);
    }

    if ((f&FPOUT) == 0) {
      //pwait(cpid);
    }
    return cpid;
  }
  /****** Child! ******/
  signal(SIGTSTP, SIG_DFL);
  signal(SIGINT,  SIG_DFL);
  signal(SIGTERM, SIG_DFL);
  signal(SIGQUIT, SIG_DFL);
  signal(SIGTSTP, SIG_DFL);
  signal(SIGSTOP, SIG_DFL);


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
    if (i == -1) {
      perror(t->fin);
      exit(errno);
    }
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
    if (i == -1) {
      perror(t->fout);
      exit(errno);
    }
    if (dup2(i, STDOUT_FILENO) == -1)
      perror("File redirect faild");
    close(i);
  }
  setenv("PATH", "/bin:/usr/bin:.", 1);
#ifdef DEBUG
  puts(t->argv[0]);
  if (t->argv[1] != NULL)
    puts(t->argv[1]);
  if (t->argv[2] != NULL)
    puts(t->argv[2]);
#endif
  execvp(t->argv[0], (char *const *)t->argv);
  /* ERROR HERE!!! */
  perror(t->argv[0]);
  exit(errno);
}

/*
static void pwait(pid_t gid) {
  int s;
  pid_t t;
  struct task *ct;

  if (gid == 0)
    return;
  for (;;) {
    t = wait3(&s, WUNTRACED, NULL);
    if (t == gid) {
      if (WIFEXITED(s) || WIFSIGNALED(s)) {
        deltask(jlist, gid);
        return;
      }
      if (WIFSTOPPED(s)) {
        jlist->fg = -1;
        ct = gettask(jlist, gid);
        jlist->stop[jlist->s++] = ct;
        printf("[%d] %s\n", jlist->s, ct->cmd);
        return;
      }
    }// else if (t == -1)
    // break;
  }
}
*/

static void lwait(struct plist *l) {
  int s;
  struct task *ct;

  while (l != NULL) {
    waitpid(l->pid, &s, WUNTRACED);
    if (WIFEXITED(s) || WIFSIGNALED(s)) {
      deltask(jlist, l);
      return;
    }
    if (WIFSTOPPED(s)) {
      jlist->fg = -1;
      ct = gettask(jlist, l);
      jlist->stop[jlist->s++] = ct;
      printf("[%d] %s\n", jlist->s, ct->cmd);
      return;
    }
  }

}


static void cmd_fg(char *ch) {
  int jid;
  int i;
  struct plist *l, *ol;

  jid = atoi(ch);
  if (strlen(ch) != 0 && (jid <= 0 || jid > jlist->s)) {
    printf("No such job.\n");
    return;
  }
  if (jlist->s <= 0)
    return;
  if (strlen(ch) == 0 && jid == 0) {
    jlist->fg = jlist->s;
    l = jlist->stop[jlist->s-1]->tids;
    printf("Job wake up:  %s\n", jlist->stop[jlist->s-1]->cmd);
    ol = l;
    while (l != NULL) {
      kill(l->pid, SIGCONT);
      l = l->next;
    }
    jlist->s--;
    lwait(ol);
    return;
  }
  l = jlist->stop[jid-1]->tids;
  printf("Job wake up:  %s\n", jlist->stop[jid-1]->cmd);
  ol = l;
  for (i = jid - 1;i < jlist->s;i++)
    jlist->stop[i] = jlist->stop[i+1];
  jlist->s--;
  while (l != NULL) {
    kill(l->pid, SIGCONT);
    l = l->next;
  }
  lwait(ol);
}
static void cmd_cd(const char *path) {
  char *cwd, *nwd = NULL;
  if (path == NULL || strlen(path) == 0) {
    printf("cd: No argument\n");
    return;
  }
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
    return;
  }
  perror(path);
  return;
}

static void cmd_jobs(void) {
  int i;

  if (jlist->s == 0)
    return;
  for (i = 0;i < jlist->s;i++)
    printf("[%d] %s\n", i + 1, jlist->stop[i]->cmd);
}


static struct task *gettask(struct jobs *jlist, struct plist *l) {
  int i;
  if (l == NULL)
    return NULL;
  for (i = 0;i < jlist->n;i++)
    if (jlist->job[i]->tids == l)
      return jlist->job[i];
  return NULL;
}

static void deltask(struct jobs *jlist, struct plist *l) {
  int i, j;

  if (l == NULL)
    return;
  for (i = 0;i < jlist->n;i++)
    if (jlist->job[i]->tids == l) {
      free(gettask(jlist, l));
      for (j = i;i < jlist->n - 1;i++)
        jlist->job[j] = jlist->job[j+1];
    }
  jlist->n--;
  for (i = 0;i < jlist->s;i++)
    if (jlist->stop[i]->tids == l) {
      for (j = i;i < jlist->s - 1;i++)
        jlist->stop[j] = jlist->stop[j+1];
    }
}

