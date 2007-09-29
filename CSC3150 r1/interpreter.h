typedef struct {
  char cmdname[256];
  char arg[256];
  int number_arg;
}
cmdcell;

typedef struct {
  cmdcell *ptr[100];
  char tail[256];
  char head[256];
  int number_cmd;
  int indicater;
}
cmd_ptr;

int start_chk(const char *cmd);
int token_chk(const char *token);
int build_cmd_chk(const char *token);
int cmd_chk(const char *cmd);
int trm_chk(const char *trm);
int resv_chk(const char *resv);
void getcmd3(char * input, cmd_ptr * cmdptr);
void getcmd4(char * input, cmd_ptr * cmdptr);
void getcmd5(char * input, cmd_ptr * cmdptr);
void getcmd6(char * input, cmd_ptr * cmdptr);
void getcmd7(char * input, cmd_ptr * cmdptr);
void getcmd8(char * input, cmd_ptr * cmdptr);
char* packcmd(char *left, cmd_ptr * cmdptr);
char * packtrm(char *left, cmd_ptr * cmdptr, int indicater);
char* packresv(char *left, cmd_ptr * cmdptr);
char* packhead(char *left, cmd_ptr * cmdptr);
void initialize2(cmd_ptr *cmdptr);
cmdcell* initialize(cmdcell *cell);
int firstck(char *line);
