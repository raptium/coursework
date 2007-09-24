/*
   CSC3150 Operating System
   Assignment 1
   Simple Unix Shell
   GUAN Hao
   WAN Yujie

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <regex.h>
/*
 * int token_chk(const char *token);
 * input token as a sting, check whether it contains any invalid characters.
 * return 1 when the token is valid.
 * 0 will be returned when any invalid character is found.
 *
 */
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

int token_chk(const char *token) {
  int i;
  char ch[2] = "";
  char chain[] = " \t><|*\"!'`";

  for (i = 0;i < 10;i++) {
    ch[0] = chain[i];
    ch[1] = '\0';
    if (strstr(token, ch) != NULL)
      return 0;
  }
  return 1;
}

int build_cmd_chk(const char *token) {
  char *cpy_token, *token2;

  cpy_token = malloc(sizeof(char) * (strlen(token) + 1));
  cpy_token[0] = 0;
  strcpy(cpy_token, token);
  token2 = strtok(cpy_token, " ");
  if ((strcmp(token2, "cd") == 0) || (strcmp(token2, "exit") == 0) || (strcmp(token2, "fg") == 0) || (strcmp(token2, "jobs") == 0))
    return 1;
  else return 0;
}

int start_chk(const char *cmd) {
  char *token, *token2; //token2 any for situation one
  char *cmd_p, *cmd_p2, *cmd_p3; //cmd_p2 any for situation 2
  int indicater = 0;
  char *cmd_name_temp;
  char *termcell_temp;

  termcell_temp = malloc(sizeof(char) * strlen(cmd) + 1);
  cmd_name_temp = malloc(sizeof(char) * strlen(cmd) + 1);
  cmd_name_temp[0] = '\0';
  termcell_temp[0] = '\0';
  cmd_p = malloc(sizeof(char) * (strlen(cmd) + 1));
  cmd_p2 = malloc(sizeof(char) * (strlen(cmd) + 1));
  cmd_p3 = malloc(sizeof(char) * (strlen(cmd) + 1));
  strcpy(cmd_p, cmd);
  strcpy(cmd_p2, cmd);
  strcpy(cmd_p3, cmd);
  //situation 1
  if (!strlen(cmd))
    return 1;
  token = strtok(cmd_p, " ");
  if (!token_chk(token))
    return 0;
  while (token != NULL && token[0] != '|' && token[0] != '>' && token[0] != '<') {   //get command
    if (indicater)													//and  chech
      strcat(cmd_name_temp, " ");									//useful when there are multiple
    strcat(cmd_name_temp, token);									//aguments
    token = strtok(NULL, " ");											//
    indicater++;													//
  }
  if (!cmd_chk(cmd_name_temp))
    return 0;
  token2 = strtok(cmd_p2, " ");
  //situation 2
  if (build_cmd_chk(token2))
    return 2;
  if (strstr(cmd, "<") == NULL && strstr(cmd, ">") == NULL && strstr(cmd, "|") == NULL)
    return 8;
  //situation 3 and 4 and 6
  if (strstr(cmd, "<") != NULL) {
    token = strtok(cmd_p3 + strlen(cmd_name_temp) + 1, " ");
    if (strcmp(token, "<") == 0) {
      token = strtok(NULL, " ");
      if (!token_chk(token))
        return 0;
      if (resv_chk(cmd_p + strlen(cmd_name_temp) + 4 + strlen(token)))
        return 3;
      if (strstr(cmd, ">") == NULL) {
        if (trm_chk(cmd_p + strlen(cmd_name_temp) + 3 + strlen(token)))
          return 4;
      }
      if (trm_chk(cmd_p + strlen(cmd_name_temp) + 4 + strlen(token)))
        return 4;
      else
        return 0;
    }
    if (!(token[0] == '>'))
      return 0;
    else {
      strcat(termcell_temp, token);
      strcat(termcell_temp, " ");
      token = strtok(NULL, " ");
      if (!token_chk(token))
        return 0;
      strcat(termcell_temp, token);
      if (!trm_chk(termcell_temp))
        return 0;
      token = strtok(NULL, " ");
      if (strcmp(token, "<") != 0)
        return 0;
      token = strtok(NULL, " ");
      if (!token_chk(token))
        return 0;
      return 6;
    }
  }
  //situation 5&7
  if (resv_chk(cmd + strlen(cmd_name_temp) + 1))
    return 5;
  if (trm_chk(cmd + strlen(cmd_name_temp) + 1))
    return 7;
  else
    return 0;
}
/*
 * int cmd_chk(const char *cmd);
 * check whether cmd is a valid command
 * [command] := [command name] [args]*
 * 1 is return if cmd is valid, otherwise 0 is returned.
 */
int cmd_chk(const char *cmd) {
  char *token;
  char *cmd_cpy;
  cmd_cpy = malloc(sizeof(char) * (strlen(cmd) + 1));
  strcpy(cmd_cpy, cmd);
  token = strtok(cmd_cpy, " ");
  while (token != NULL) {
    if (!token_chk(token))
      return 0;
    token = strtok(NULL, " ");
  }
  return 1;
}

/*
 *
 * int trm_chk(const char *trm);
 */

int trm_chk(const char *trm) {
  if (!strlen(trm))
    return 10;
  if (trm[0] == '>' && trm[1] == ' ')
    if (token_chk(trm + 2))
      return 11;
  if (trm[0] == '>' && trm[1] == '>' && trm[2] == ' ')
    if (token_chk(trm + 3))
      return 12;
  return 0;
}

/*
 * int resv_chk(const char *resv);
 *
 */
int resv_chk(const char *resv) {
  char *token;
  char *resv_cpy;
  char *cmd_name_temp;
  int indicater = 0;
  cmd_name_temp = malloc(sizeof(char) * (strlen(resv) + 1));
  resv_cpy = malloc(sizeof(char) * (strlen(resv) + 1));
  cmd_name_temp[0] = '\0';
  strcpy(resv_cpy, resv);
  if (resv[0] != '|' || resv[1] != ' ')
    return 0;
  token = strtok(resv_cpy + 2, " ");
  if (!token_chk(token))
    return 0;
  while (token != NULL && token[0] != '|' && token[0] != '>') {   //get command
    if (indicater)									//and  chech
      strcat(cmd_name_temp, " ");					//useful when there are multiple
    strcat(cmd_name_temp, token);					//aguments
    token = strtok(NULL, " ");							//
    indicater++;									//
  }
  if (!cmd_chk(cmd_name_temp))
    return 0;
  if ((resv[2+strlen(cmd_name_temp)] != ' ') && strlen(resv + 2 + strlen(cmd_name_temp)) != 0)
    return 0;
  if (resv_chk(resv + 3 + strlen(cmd_name_temp)))
    return 8;
  if (trm_chk(resv + 3 + strlen(cmd_name_temp)) || trm_chk(resv + 2 + strlen(cmd_name_temp)))
    return 9;
  return 0;
}




void getcmd3(char * input, cmd_ptr * cmdptr) {
  char *left;

  left = input;
  left = packcmd(left, cmdptr);
  left = packhead(left, cmdptr);
  left = packresv(left, cmdptr);
}
void getcmd4(char * input, cmd_ptr * cmdptr) {
  char *left;

  left = input;
  left = packcmd(left, cmdptr);
  left = packhead(left, cmdptr);
  if (strlen(left) != 0 && left[1] == '>')
    packtrm(left + 3, cmdptr, 2);
  else if (strlen(left) != 0 && left[0] == '>')
    packtrm(left + 2, cmdptr, 1);
  else
    packtrm(left, cmdptr, 0);
}

void getcmd5(char * input, cmd_ptr * cmdptr) {
  char *left;
  char *temp;

  left = input;
  left = packcmd(left, cmdptr);
  temp = malloc(sizeof(char) * (strlen(left) + 3));
  temp[0] = '\0';
  strcat(temp, "| ");
  strcat(temp, left);
  left = temp;
  left = packresv(left, cmdptr);
}

void getcmd6(char * input, cmd_ptr * cmdptr) {
  char *left;
  int indicater = 0;
  left = input;
  if (strstr(input, ">>") != NULL)
    indicater = 2;
  else if (strstr(input, ">") != NULL)
    indicater = 1;
  left = packcmd(left, cmdptr);
  if (indicater == 2)
    left = packtrm(left, cmdptr, 5);
  else if (indicater == 1)
    left = packtrm(left, cmdptr, 4);
  else
    left = packtrm(left, cmdptr, 0);
  left = packhead(left, cmdptr);
}
void getcmd7(char * input, cmd_ptr * cmdptr) {
  char *left;
  int indicater = 0;
  left = input;
  if (strstr(input, ">>") != NULL)
    indicater = 2;
  else if (strstr(input, ">") != NULL)
    indicater = 1;
  left = packcmd(left, cmdptr);
  left = packtrm(left, cmdptr, indicater);
}


void getcmd8(char * input, cmd_ptr * cmdptr) {
  char *cpy_left;
  char *token;
  char *left = input;
  cmdcell *cell = NULL;
  cell = initialize(cell);
  cpy_left = malloc(sizeof(char) * strlen(left) + 1);
  strcpy(cpy_left, left);
  token = strtok(cpy_left, " ");
  cmdptr->number_cmd++;
  cmdptr->ptr[cmdptr->number_cmd-1] = cell;
  strcpy(cmdptr->ptr[cmdptr->number_cmd-1]->cmdname, token);
  if (strlen(left + strlen(token)) != 0) {
    left = left + strlen(token) + 1;

    do {
      token = strtok(NULL, " ");
      strcat(cmdptr->ptr[cmdptr->number_cmd-1]->arg, token);
      strcat(cmdptr->ptr[cmdptr->number_cmd-1]->arg, " ");
      cmdptr->ptr[cmdptr->number_cmd-1]->number_arg++;

      if (strlen(left + strlen(token)) != 0) {
        //token=strtok(NULL," ");
        left = left + strlen(token) + 1;
      } else
        break;
    } while (1);
  }
}




char* packcmd(char *left, cmd_ptr * cmdptr) {
  char *cpy_left;
  char *token;
  cmdcell *cell = NULL;
  cell = initialize(cell);
  cpy_left = malloc(sizeof(char) * strlen(left) + 1);
  strcpy(cpy_left, left);
  token = strtok(cpy_left, " ");
  cmdptr->number_cmd++;
  cmdptr->ptr[cmdptr->number_cmd-1] = cell;
  strcpy(cmdptr->ptr[cmdptr->number_cmd-1]->cmdname, token);
  if (strlen(left + strlen(token)) != 0) {
    left = left + strlen(token) + 1;
    token = strtok(NULL, " ");
    while (strcmp(token, "<") != 0 && token[0] != '|' && token[0] != '>') {
      strcat(cmdptr->ptr[cmdptr->number_cmd-1]->arg, token);
      strcat(cmdptr->ptr[cmdptr->number_cmd-1]->arg, " ");
      left = left + strlen(token) + 1;
      token = strtok(NULL, " ");
      cmdptr->ptr[cmdptr->number_cmd-1]->number_arg++;
    }
    return left + strlen(token) + 1;
  } else
    return NULL;
}




char * packtrm(char *left, cmd_ptr * cmdptr, int indicater) {
  char * token, *left_cpy;

  if (indicater == 1 || indicater == 2) {
    strcpy(cmdptr->tail, left);
    cmdptr->indicater = indicater;
    return NULL;
  } else if (indicater == 4 || indicater == 5) {
    left_cpy = malloc(sizeof(char) * (strlen(left) + 1));
    strcpy(left_cpy, left);
    token = strtok(left_cpy, " ");
    strcpy(cmdptr->tail, token);
    cmdptr->indicater = indicater % 3;
    return left + 3 + strlen(token);
  }
  return NULL;
}

char* packresv(char *left, cmd_ptr * cmdptr) {
  //char *cpy_left;
  //cpy_left=malloc(sizeof(char)*strlen(left));
  //strcpy(cpy_left,left);
  char *temp;
  int indicater = 0;
  if (left[0] != '|') {
    temp = malloc((strlen(left) + 3) * sizeof(char));
    temp[0] = '\0';
    strcat(temp, "| ");
    strcat(temp, left);
    left = temp;
  }

  if (resv_chk(left) == 8) {
    left = packcmd(left + 2, cmdptr);
    left = packresv(left, cmdptr);
    return left;
  } else {
    if (strstr(left, ">>") != NULL)
      indicater = 2;
    else if (strstr(left, ">") != NULL)
      indicater = 1;
    if (left[0] != '|')
      left = packcmd(left, cmdptr);
    else
      left = packcmd(left + 2, cmdptr);

    packtrm(left, cmdptr, indicater);
  }
  return NULL;
}
char* packhead(char *left, cmd_ptr * cmdptr) {
  char *cpy_left;
  char *token;
  cpy_left = malloc(sizeof(char) * strlen(left) + 1);
  strcpy(cpy_left, left);
  token = strtok(cpy_left, " ");
  strcpy(cmdptr->head, token);
  return left + strlen(token) + 1;
}
cmdcell* initialize(cmdcell *cell) {
  cell = malloc(sizeof(cmdcell));
  strcpy(cell->arg, " ");
  strcpy(cell->cmdname, " ");
  cell->number_arg = 0;
  return cell;
}

void initialize2(cmd_ptr *cmdptr) {
  strcpy(cmdptr->head, "");
  strcpy(cmdptr->tail, "");
  cmdptr->number_cmd = 0;
  cmdptr->indicater = 0;
}



/*
int main(void){
  char test2[14][40];
  char test[256];
  int i,j;
  cmd_ptr *cmdptr=NULL;
  	strcpy(test2[0],"man ");
        strcpy(test2[1],"man 1.txt");
        strcpy(test2[2],"man 1.txt 2.txt");
        strcpy(test2[3],"man");
        strcpy(test2[4],"man 1.txt < 2.txt | man | man");
        strcpy(test2[5],"man 1.txt < 2.txt | man | man >> 2.txt");
        strcpy(test2[6],"man 1.txt < 2.txt | man | man > 2.txt");
        strcpy(test2[7],"man 1.txt < 2.txt");
        strcpy(test2[8],"man 1.txt < 2.txt > 3.txt");
        strcpy(test2[9],"man 1.txt 2.txt |ls -l | man | ls -m >> 4.txt");
        strcpy(test2[10],"man 1.txt < 2.txt");
        strcpy(test2[11],"man 1.txt > 2.txt < 3.txt");
        strcpy(test2[12],"man 1.txt >> 2.txt");
        strcpy(test2[13],"man 1.txt >> 2.txt");
        printf("typetail     tail          head\n");

  //for(i=0;i<13;i++)
  while(1)
  {
    gets(test);
    cmdptr=malloc(sizeof(cmd_ptr));
    initialize2(cmdptr);
    //test=malloc(sizeof(char)*(strlen(test2[i])+1));
    //strcpy(test,test2[i]);
    if(strstr(test,"  ")){
      printf("invalid input\n");
      //free(test);
      free(cmdptr);
      continue;
    }
    if(test[strlen(test)-1]==' ')
      test[strlen(test)-1]='\0';
    if(start_chk(test)==2)
      getcmd8(test,cmdptr);
    else if(start_chk(test)==3)
      getcmd3(test,cmdptr);
    else if(start_chk(test)==4)
      getcmd4(test,cmdptr);
    else if(start_chk(test)==5)
      getcmd5(test,cmdptr);
    else if(start_chk(test)==6)
      getcmd6(test,cmdptr);
    else if(start_chk(test)==7)
      getcmd7(test,cmdptr);
    else if(start_chk(test)==8)
      getcmd8(test,cmdptr);

    else{
      printf("invalid input\n");
      //free(test);
      free(cmdptr);
      continue;
    }
    //printf("\n\n%d\n",i);
    printf("\t%d\t%s\t %s",cmdptr->indicater,cmdptr->tail,cmdptr->head);
    for(j=0;j<cmdptr->number_cmd;j++)
      printf("\n\t%d\t%s\t%s\n",cmdptr->ptr[j]->number_arg,cmdptr->ptr[j]->cmdname,cmdptr->ptr[j]->arg);
    //free(test);
    free(cmdptr);
  }
  return 0;
}

*/

















