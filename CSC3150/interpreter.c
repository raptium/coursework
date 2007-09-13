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
#include <regex.h>
#include "interpreter.h"
/*
 * int token_chk(const char *token);
 * input token as a sting, check whether it contains any invalid characters.
 * return 1 when the token is valid.
 * 0 will be returned when any invalid character is found.
 *
 */
int token_chk(const char *token) {
    int status;
    regex_t re;
    regcomp(&re, "[ \t><\\|\\*\"!'`]", REG_EXTENDED | REG_NOSUB);
    status = regexec(&re, token, (size_t)0, NULL, 0);
    regfree(&re);
    return status == REG_NOMATCH;
}

int build_cmd_chk(const char *token) {
    if ((strcmp(token, "cd") == 0) || (strcmp(token, "exit") == 0) || (strcmp(token, "fg") == 0) || (strcmp(token, "jobs") == 0))
        return 1;
    return 0;
}


/*
 * int cmd_chk(const char *cmd);
 * check whether cmd is a valid command
 * [command] := [command name] [args]*
 * 1 is return if cmd is valid, otherwise 0 is returned.
 */

int start_chk(const char *cmd) {
    char *token;
    char *cmd_p;
    cmd_p = malloc(sizeof(char) * (strlen(cmd) + 1));
    strcpy(cmd_p, cmd);
    //situation 1
    if (!strlen(cmd))
        return 1;

    token = strtok(cmd_p, " ");

    if (!token_chk(token))
        return 0;

    //situation 2
    if (build_cmd_chk(token)) {
        if (token_chk(cmd_p + strlen(token) + 1))
            return 2;
        else
            return 0;
    }

    //situation 3 and 4 and 6
    if (strstr(cmd_p, "<") != NULL) {
        if (!cmd_chk(token))
            return 0;
        token = strtok(NULL, " ");

        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            if (!token_chk(token))
                return 0;
            if (resv_chk(cmd_p))
                return 3;
            if (trm_chk(cmd_p))
                return 4;
            else
                return 0;
        }
        if (!trm_chk(token))
            return 0;
        token = strtok(NULL, " ");

        if (strcmp(token, "<") == 0)
            if (token_chk(cmd_p))
                return 6;
            else
                return 0;
        else
            return 0;
    }
    //situation 5&7
    if (!cmd_chk(token))
        return 0;
    if (resv_chk(cmd_p))
        return 5;
    if (trm_chk(cmd_p))
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
        return 1;
    if (trm[0] == '>' && trm[1] == ' ')
        if (token_chk(trm + 2))
            return 2;
    if (trm[0] == '>' && trm[1] == '>' && trm[2] == ' ')
        if (token_chk(trm + 3))
            return 3;
    return 0;
}

/*
 * int resv_chk(const char *resv);
 *
 */
int resv_chk(const char *resv) {
    char *token;
    char *resv_cpy;
    int pos;
    resv_cpy = malloc(sizeof(char) * (strlen(resv) + 1));
    strcpy(resv_cpy, resv);
    if (resv[0] != '|' || resv[1] != ' ')
        return 0;
    token = strtok(resv_cpy + 2, " ");
    if (!token_chk(token))
        return 0;
    pos = 2 + strlen(token);
    if (pos > strlen(resv))
        pos = strlen(resv) - 1;
    if (resv[pos] != 0 && resv[pos] != ' ')
        return 0;
    pos++;
    return resv_chk(resv + pos) || trm_chk(resv + pos);
}


int main(int argc, char *argv[]) {
    char test[] = "| man | 1.txt";
    printf("%s\n", test);
    printf("%d\n", resv_chk(test));
    return 0;
}

