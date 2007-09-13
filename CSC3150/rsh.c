/* CSC3150 Assignemnt 1
 * ratpium@gmail.com
 * rsh.c
 *
 * my simple UNIX shell
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


pid_t mypid, mypgid, mysid;

static char *getpwd(void) {
    char *dir = getcwd(0, 0);
    return dir ?  dir : 0;
}

static int cmdloop(int inter) {
    char *cmd;
    cmd = malloc(sizeof(char) * 256);
    for (;;) {
        printf("%s@localhost %s$", getlogin(), getpwd());
        gets(cmd);

        if (fork() == 0) {
            printf("%s", cmd);
            execlp(cmd, cmd, NULL);
            fprintf(stderr, "ERROR!!!");
        } else {
            wait(NULL);
        }

    }
    return 0;
}


char *input_cmd(void) {
    char *cmd;
    char sign = '#';

    cmd = malloc(sizeof(char) * 256);

    if (strcmp(getlogin(), "root"))sign = '$';
    for (;;) {
        gets(cmd);
        if (!strcmp(cmd, "exit"))
            break;
    }

}

int main(int argc, char *argv[]);
int main(int argc, char *argv[]) {

    //init_sigval();
    mypid = getpid();
    mypgid = getpgid(mypid);
    mysid = getsid(mypid);

    cmdloop(1);

    return 0;
}
