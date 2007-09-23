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


struct jobs {
  int n, s, fg;
  struct task **job;
  struct task **stop;
};

struct task {
  struct tnode *start;
  char *cmd;
  pid_t tid;
};


/*
 * Globe Variables
 *
 *
 */

struct jobs *jlist;

static pid_t execute(struct tnode *t, int *pin, int *pout, pid_t pgid);
static pid_t exec1(struct tnode *t);
static pid_t exec2(struct tnode *t, int *pin, int *pout, pid_t pgid);
static void pwait(pid_t cp);
static void gwait(pid_t cg);
static struct task *gettask(struct jobs *jlist, pid_t pid);
static void deltask(struct jobs *jlist, pid_t pid);

static pid_t execute(struct tnode *t, int  *pin, int *pout, pid_t pgid) {
  struct tnode *t1;
  pid_t cpid;
  int f, pfd[2];

  if (t == NULL)
    return 0;
  switch (t->type) {
  case TBDIN:
    if (t->argv == NULL || t->argv[0] == NULL) {
      perror("Invalid command");
      return 0;
    }
    return exec1(t);
  case TPIPE:
    if ((t1 = t->next) == NULL) {
      exec2(t, pin, NULL, pgid);
      return 0;
    }
    if (pipe(pfd) == -1) {
      perror("Cannot create pipe");
      if (pin != NULL) {
        close(pin[0]);
        close(pin[1]);
      }
      return 0;
    }
    exec2(t, pin, pfd, pgid);
    t->flags |= FPOUT;
    f = t->flags;
    t1->flags |= FPIN | (f & FAPN);
    t1->fout = t->fout;
    execute(t1, pfd, pout, pgid);
    close(pfd[0]);
    close(pfd[1]);
    return 0;
  }
}

static void exec2(struct tnode *t, int *pin, int *pout, pid_t pgid) {
  struct tnode *t1;
  int f, i, cpid;
  char *cmd;

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

    if ((f&FPOUT) == 0) {
      gwait(pgid == 0 ? cpid : pgid);
    }
    return;
  }
  /****** Child! ******/
  signal(SIGTSTP, SIG_DFL);
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
  setenv("PATH", "/bin:/usr/bin:.", 1);
  execvp(t->argv[0], (char *const *)t->argv);
  /* ERROR HERE!!! */
  perror("Execute failed");
  exit(errno);
}


static void gwait(pid_t gid) {
  int s, i, j, t;
  struct task *ct;

  if (gid == 0)
    return;
  for (;;) {
    t = waitpid(-gid, &s, WUNTRACED);
    if (t == -1) {
      t = waitpid(gid, &s, WUNTRACED);
      if (WIFEXITED(s)) {
        deltask(jlist, gid);
        return;
      }
      if (WIFSTOPPED(s)) {
        jlist->fg = -1;
        ct = gettask(jlist, gid);
        jlist->stop[jlist->s++] = ct;
        printf("[%d] %d\n", jlist->n, gid);
        return;
      }
    }
  }
}

/*
static void pwait(pid_t cp) {
  pid_t tp, pgid;
  int s, i, j;

  if (cp == 0)
    return;
  for (;;) {
    tp = wait(&s);
    if (tp == -1)
      break;
    if (tp == cp)
      break;
    if ((pgid = getpgid(cp)) == nowait)
      break;
  }
  for (i = 0;i < jlist->n;i++)
    if (cp == jlist->glist[i])
      for (j = i;j < jlist->n - 1;j++) {
        jlist->glist[j] = jlist->glist[j+1];
        strcmp(jlist->gcmd[j], jlist->gcmd[j+1]);
      }
  (jlist->n)--;
  if (jlist->glist[jlist->fg] == pgid)
    jlist->fg = -1;
}
*/

void cmd_fg(char *ch) {
  int jid;
  int i;
  pid_t gid;

  jid = atoi(ch);
  if (jlist->n == 0)
    return;
  if (jid == 0) {
    jlist->fg = jlist->s;
    gid = jlist->stop[jlist->s]->tid;
    kill(gid, SIGCONT);
    jlist->s--;
    gwait(gid);
  }
}



static struct task *gettask(struct jobs *jlist, pid_t pid) {
  int i;
  if (pid == 0)
    return NULL;
  for (i = 0;i < jlist->n;i++)
    if (jlist->job[i]->tid == pid)
      return jlist->job[i];
  return NULL;
}

static void deltask(struct jobs *jlist, pid_t pid) {
  int i, j;

  if (pid == 0)
    return;
  for (i = 0;i < jlist->n;i++)
    if (jlist->job[i]->tid == pid) {
      free(gettask(jlist, pid));
      for (j = i;i < jlist->n - 1;i++)
        jlist->job[j] = jlist->job[j+1];
    }
  jlist->n--;
  for (i = 0;i < jlist->s;i++)
    if (jlist->stop[i]->tid == pid) {
      for (j = i;i < jlist->s - 1;i++)
        jlist->stop[j] = jlist->stop[j+1];
    }
}

