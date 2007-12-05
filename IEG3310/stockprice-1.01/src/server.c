#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <bzlib.h>
#ifdef HAVE_LIBNCURSES
#include <ncurses.h>
#endif

#define BACKLOG 10
#define SWPFILE "/tmp/.chldlist"

void sigchld_handler(int s) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void sigint_handler(int sig) {
#ifdef HAVE_LIBNCURSES
    endwin();
#endif
    exit(0);
}

char *readdata(FILE *fp, char *data, int numbyte) {
    int i, j = 0;
    char ch;

    for (i = 0;i < numbyte;i++) {
        fread(&ch, sizeof(char), 1, fp);
        if (isalnum(ch) || ispunct(ch)) {
            data[j] = ch;
            j++;
        } else if (isspace(ch) && !isspace(data[j-1])) {
            data[j] = ' ';
            j++;
        }
    }
    data[j] = '\n';
    data[j+1] = '\0';
    return data;
}

int compressData(char *data) {
    char *buf;
    unsigned int bufLen;

    bufLen = (int)(strlen(data) * 1.001 + 12);
    buf = (char *)malloc(sizeof(char) * bufLen);
    BZ2_bzBuffToBuffCompress(buf, &bufLen, data, strlen(data), 5, 0, 0);
    memcpy(data, buf, bufLen);
    free(buf);
    return bufLen;
}


void printlist(void) {
    FILE *swp;
    char *cip;
    time_t now;

    swp = fopen(SWPFILE, "w");
    fclose(swp);

    cip = (char *)malloc(sizeof(char) * 30);
#ifdef HAVE_LIBNCURSES
    initscr();
    curs_set(0);
#endif
    while (1) {
        swp = fopen(SWPFILE, "r+");
#ifdef HAVE_LIBNCURSES
        refresh();
        werase(stdscr);
#endif
        time(&now);
#ifdef HAVE_LIBNCURSES
        printw(asctime(localtime(&now)));
        printw("\n");
        printw("Children:\n");
#else
        printf(asctime(localtime(&now)));
        printf("\n");
        printf("Children:\n");
#endif
        while (!feof(swp)) {
            if (fgets(cip, 29, swp) != NULL)
#ifdef HAVE_LIBNCURSES
                printw(cip);
#else
                printf(cip);
#endif
        }
#ifdef HAVE_LIBNCURSES
        printw("\n");
#else
        printf("\n");
#endif
        fclose(swp);
        swp = fopen(SWPFILE, "w");
        fclose(swp);
        sleep(2);
    }
}

void updatelist(struct sockaddr_in clnt_addr) {
    FILE *swp;
    char *cip;

    swp = fopen(SWPFILE, "r+");
    cip = (char *)malloc(sizeof(char)*30);
    while (!feof(swp)) {
        fgets(cip, 29, swp);
        cip[strlen(cip)-1] = '\0';
        if (!strcmp(cip, inet_ntoa(clnt_addr.sin_addr)))
            return;
    }
    fclose(swp);
    swp = fopen(SWPFILE, "a+");
    sprintf(cip, "%s\n", inet_ntoa(clnt_addr.sin_addr));
    fputs(cip, swp);
    fclose(swp);
    free(cip);
}

void senddata(int new_fd, struct sockaddr_in clnt_addr, char *datafile) {
    FILE *infp;
    time_t now, start;
    struct tm starttime;
    char *data, ch;
    char *frame;
    int offset, dataLen, tOffset;
    int timeFlag;

    time(&start);
    memcpy(&starttime, gmtime(&start), sizeof(struct tm));
    starttime.tm_sec = 0;
    starttime.tm_min = 0;
    starttime.tm_hour = 10;
    start = mktime(&starttime);
    infp = fopen(datafile , "r");
    if (infp == NULL) {
        perror("Data File Open Failed");
        exit(errno);
    }
    data = (char *)malloc(sizeof(char) * 512);
    frame = (char *)malloc(sizeof(char)*256);
    readdata(infp, data, 282);
    dataLen = compressData(data);
    memset(frame, 0, sizeof(frame));
    frame[0] = 'D';
    frame[1] = dataLen;
    memcpy(frame + 2, data, dataLen);
    if (send(new_fd, frame, 256, 0) == -1) {
        perror("send");
        close(new_fd);
        exit(errno);
    }
    while (1) {
        updatelist(clnt_addr);
        sleep(2);
        time(&now);
	tOffset=(int)difftime(now,start);
	if(tOffset>9000)
		tOffset-=7200;
        offset = 246 * tOffset / 2 + 282 - ftell(infp);
        if (fseek(infp, offset, SEEK_CUR))
            printf("fseek failed!\n");
        timeFlag = 0;
        while (timeFlag != 2) {
            fread(&ch, sizeof(char), 1, infp);
            if (ch == ':')
                timeFlag++;
        }
        fseek(infp, -7, SEEK_CUR);
        readdata(infp, data, 246);
        dataLen = compressData(data);
        memset(frame, 0, sizeof(frame));
        frame[0] = 'D';
        frame[1] = dataLen;
        memcpy(frame + 2, data, dataLen);
        if (send(new_fd, frame, 256, 0) == -1) {
            perror("send");
            close(new_fd);
            fclose(infp);
            exit(errno);
        }
    }
}

int handshake(int new_fd) {
    char *buf;
    int numbytes;

    buf = (char *)malloc(sizeof(char) * 256);
    while (1) {
        if ((numbytes = recv(new_fd, buf, 256, 0)) == -1) {
            perror("receive");
            return errno;
        }
        if (numbytes == 0)
            break;
        if (buf[0] == 'R') {
            memset(buf, 0, sizeof(buf));
            buf[0] = 'A';
            if (send(new_fd, buf, 256, 0) == -1) {
                perror("send");
                close(new_fd);
                exit(errno);
            }
            break;
        } else {
            memset(buf, 0, sizeof(buf));
            buf[0] = 'R';
            if (send(new_fd, buf, 256, 0) == -1) {
                perror("send");
                close(new_fd);
                exit(errno);
            }
        }
    }
    return 0;
}


int main(int argc, char *const argv[]) {
    int sockfd, new_fd;
    struct sockaddr_in srv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    int port = 2333;


    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    if (argc < 2) {
        printf("Usage: %s <data file> [port]\n", argv[0]);
        return 0;
    }

    if (argc > 2)
        port = atoi(argv[2]);

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(srv_addr.sin_zero), '\0', 8);

    if (bind(sockfd, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    if (fork() == 0)
        printlist();

    sin_size = sizeof(struct sockaddr_in);
    while (1) {
        if ((new_fd = accept(sockfd, (struct sockaddr *) & clnt_addr, &sin_size)) == -1) {
            if (errno != EINTR)
                perror("accpet");
            continue;
        }
        if (fork() == 0) {
            close(sockfd);
            handshake(new_fd);
            senddata(new_fd, clnt_addr, argv[1]);
        }
        close(new_fd);
    }
    return 0;
}
