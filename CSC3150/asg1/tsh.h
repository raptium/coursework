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

struct plist{
    struct plist *next;
    pid_t pid;
};

struct task {
  struct tnode *start;
  char *cmd;
  pid_t tid;
  struct plist *tids;
};




/*
 * Globe Variables
 *
 *
 */

struct jobs *jlist;

static struct plist *execute(struct tnode *t, int *pin, int *pout);
static void exec1(struct tnode *t);
static pid_t exec2(struct tnode *t, int *pin, int *pout);
static void lwait(struct plist *l);
static struct task *gettask(struct jobs *jlist, struct plist *l);
static void deltask(struct jobs *jlist, struct plist *l);
static void cmd_fg(char *ch);
static void cmd_cd(const char *path);
static void cmd_jobs(void);
