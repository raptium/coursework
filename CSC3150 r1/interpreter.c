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
#include "interpreter.h"

//#include <regex.h>
/*
 * int token_chk(const char *token);
 * input token as a sting, check whether it contains any invalid characters.
 * return 1 when the token is valid.
 * 0 will be returned when any invalid character is found.
 *
 */

int token_chk(const char *token) {
  int i;
  char ch[2] = "";
  char chain[] = " \t><|*\"!'`";

  if (token == NULL)
    return 0;
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
  if ((strcmp(token2, "cd") != 0) && (strcmp(token2, "fg") != 0) && (strcmp(token2, "jobs") != 0) && (strcmp(token2, "exit") != 0))
    return 0;
  if ((strcmp(token2, "cd") == 0) || (strcmp(token2, "fg") == 0)) {
    if (strtok(NULL, " ") != NULL)
      if (strtok(NULL, " ") != NULL)
        return 0;
  } else if ((strcmp(token2, "jobs") == 0) || (strcmp(token2, "exit") == 0)) {
    if (strtok(NULL, " ") != NULL)
      return 0;
  }
  return 1;
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
  if (build_cmd_chk(cmd))
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
  return -1;
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
      cmdptr->ptr[cmdptr->number_cmd-1]->number_arg++;
      if (strlen(left + strlen(token)) == 0)
        return 0;
      left = left + strlen(token) + 1;
      token = strtok(NULL, " ");
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

int firstck(char *line) {
  char *temp;
  if (line[0] == ' ' || line[strlen(line)-1] == ' ')
    return 0;
  temp = malloc(sizeof(char) * (strlen(line) + 1));
  strcpy(temp, "  ");
  while (strlen(temp) <= strlen(line)) {
    if (strstr(line, temp) != NULL)
      return 0;
    strcat(temp, " ");
  }
  return 1;
}



















