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
int start_chk(const char *cmd);
int token_chk(const char *token);
int build_cmd_chk(const char *token);
int cmd_chk(const char *cmd);
int trm_chk(const char *trm);
int resv_chk(const char *resv);

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
    if ((strcmp(token, "cd") == 0) || (strcmp(token, "exit") == 0) || (strcmp(token, "fg") == 0) || (strcmp(token, "jobs") == 0))
        return 1;
    else return 0;
}


/*
 * int cmd_chk(const char *cmd);
 * check whether cmd is a valid command
 * [command] := [command name] [args]*
 * 1 is return if cmd is valid, otherwise 0 is returned.
 */

int start_chk(const char *cmd) {
    char *token, *token2; //token2 any for situation one
    char *cmd_p, *cmd_p2; //cmd_p2 any for situation 2
    int indicater = 0;
    char *cmd_name_temp;
    char *termcell_temp;

    termcell_temp = malloc(sizeof(char) * strlen(cmd) + 1);
    cmd_name_temp = malloc(sizeof(char) * strlen(cmd) + 1);
    cmd_name_temp[0] = '\0';
    termcell_temp[0] = '\0';

    cmd_p = malloc(sizeof(char) * (strlen(cmd) + 1));
    cmd_p2 = malloc(sizeof(char) * (strlen(cmd) + 1));
    strcpy(cmd_p, cmd);
    strcpy(cmd_p2, cmd);
    //situation 1
    if (!strlen(cmd))
        return 1;

    token = strtok(cmd_p, " ");
    if (!token_chk(token))
        return 0;




    while (token != NULL && token[0] != '|' && token[0] != '>' && token[0] != '<') {   //get command
        if (indicater)									//and  chech
            strcat(cmd_name_temp, " ");					//useful when there are multiple
        strcat(cmd_name_temp, token);					//aguments
        token = strtok(NULL, " ");							//
        indicater++;									//
    }

    if (!cmd_chk(cmd_name_temp))
        return 0;

    token2 = strtok(cmd_p2, " ");

    //situation 2
    if (build_cmd_chk(token2))
        return 2;

    //situation 3 and 4 and 6
    if (strstr(cmd, "<") != NULL) {
        token = strtok(cmd + strlen(cmd_name_temp) + 1, " ");
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            if (!token_chk(token))
                return 0;
            if (resv_chk(cmd_p + strlen(cmd_name_temp) + 4 + strlen(token)))
                return 3;
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
    if (resv[2+strlen(cmd_name_temp)] != ' ')
        return 0;
    if (resv_chk(resv + 3 + strlen(cmd_name_temp)))
        return 8;
    if (trm_chk(resv + 3 + strlen(cmd_name_temp)))
        return 9;
}


int main(int argc, char *argv[]) {
    char test[] = "man 1.txt 1.txt > sdf";
    printf("%s\n", test);
    printf("%d\n", start_chk(test));
    return 0;
}




















