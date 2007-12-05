#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>
#include <bzlib.h>
#ifdef HAVE_LIBNCURSES
#include <ncurses.h>
#endif
#include "symtab.h"
#define MAXDATASIZE 256

void sigint_handler(int sig) {
#ifdef HAVE_LIBNCURSES
    endwin();
#endif
    exit(0);
}

char *decompressData(char *dest, char *source, int sourceLen) {
    char *buf;
    unsigned int bufLen;

    bufLen = (int)(sourceLen * 4);
    buf = (char *)malloc(sizeof(char) * bufLen);
    BZ2_bzBuffToBuffDecompress(buf, &bufLen, source, sourceLen, 1, 0);
    memcpy(dest, buf, bufLen);
    dest[bufLen] = '\0';
    free(buf);
    return dest;
}

int getStockCode(char *data, char **stockCode) {
    int i, j, k;
    char tempCode[16];
    char ch;


    tempCode[0] = '\0';
    j = 0;
    k = 0;

    for (i = 11;i < strlen(data);i++) {
        ch = data[i];
        if (isspace(ch) && j != 0) {
            tempCode[j] = '\0';
            j = 0;
            strcpy(stockCode[k], tempCode);
            k++;
        } else {
            tempCode[j] = ch;
            j++;
        }
    }
    return k;
}

int updateStockPrice(char *data, char **stockCode, symtabADT priceTable) {
    int i, j, k;
    char *tempPrice, *timeStamp;
    char ch;


    j = 0;
    k = 0;

    tempPrice = (char *)malloc(sizeof(char) * 8);
    timeStamp = (char *)malloc(sizeof(char) * 12);
    memcpy(timeStamp, data + 1, 8);
    timeStamp[8] = '\0';
    Enter(priceTable, "TIME", timeStamp);
    for (i = 10;i < strlen(data);i++) {
        ch = data[i];
        if (isspace(ch) && j != 0) {
            tempPrice[j] = '\0';
            j = 0;
            Enter(priceTable, stockCode[k], tempPrice);
            tempPrice = (char *)malloc(sizeof(char) * 8);
            k++;
        } else {
            tempPrice[j] = ch;
            j++;
        }
    }
    return k;
}

void printStockPrice(char **stockCode, int stockNum, symtabADT priceTable) {
    int i;


#ifdef HAVE_LIBNCURSES
    werase(stdscr);
    printw("Time: ");
    printw((char *)Lookup(priceTable, "TIME"));
    printw("\n");
    refresh();
#else
    int k = 0;
    printf("Time: ");
    printf((char *)Lookup(priceTable, "TIME"));
    printf("\n\n");
#endif

#ifdef HAVE_LIBNCURSES
    for (i = 0;i < stockNum;i++) {
        move(i / 10*5 + 2, i % 10*8);
        printw(stockCode[i]);
        move(i / 10*5 + 3, i % 10*8 + 1);
        printw((char *)Lookup(priceTable, stockCode[i]));
    }
#else
    for (i = 0;i < stockNum || k == 0;i++) {
        if (k == 0)
            printf("%8s", stockCode[i]);
        else
            printf("%7s ", (char *)Lookup(priceTable, stockCode[i]));
        if (i % 10 == 9) {
            printf("\n");
            if (k == 0)
                i -= 10;
            else
                printf("\n");
            k = !k;
        }
    }
    printf("\n\n");
#endif
#ifdef HAVE_LIBNCURSES
    refresh();
#endif
}

int handshake(int sockfd) {
    char *buf;
    int numbytes;

    buf = (char *)malloc(sizeof(char) * 256);
    memset(buf, 0, sizeof(buf));
    buf[0] = 'R';
    if (send(sockfd, buf, 256, 0) == -1) {
        perror("send");
        close(sockfd);
        exit(errno);
    }
    if ((numbytes = recv(sockfd, buf, 256, 0)) == -1) {
        perror("receive");
        return errno;
    }
    if (numbytes == 0)
        return 0;
    if (buf[0] == 'A')
        return 1;
    else
        return 0;
}

void updateLog(symtabADT priceTable, char **stockCode, int stockNum, char *logfile) {
    FILE *logf;
    int i;

    if (priceTable == NULL) {
        if ((logf = fopen(logfile, "w+")) == NULL) {
            perror("Log file create failed");
            exit(errno);
        }
        fprintf(logf, "Stock Code");
        for (i = 0;i < stockNum;i++)
            fprintf(logf, "\t%s", stockCode[i]);
        fprintf(logf, "\n");
        fclose(logf);
    } else {
        if ((logf = fopen(logfile, "a")) == NULL) {
            perror("Log file write failed");
            exit(errno);
        }
        fprintf(logf, (char *)Lookup(priceTable, "TIME"));
        for (i = 0;i < stockNum;i++)
            fprintf(logf, "\t%s", (char *)Lookup(priceTable, stockCode[i]));
        fprintf(logf, "\n");
        fclose(logf);
    }
}



int main(int argc, char *const argv[]) {
    int port = 2333;
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    char *data;
    char **stockCode;
    char hostname[64];
    int stockNum;
    struct hostent *host;
    struct sockaddr_in server_addr;
    symtabADT priceTable;

    int i;

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }


    stockCode = (char **)malloc(sizeof(char *) * 64);
    for (i = 0;i < 64;i++)
        stockCode[i] = (char *)malloc(sizeof(char) * 16);

    priceTable = EmptySymbolTable();

    if (argc < 2) {
        printf("Usage: %s <log file> [host [port]]\n", argv[0]);
        return 0;
    }

    if (argc > 2)
        strcpy(hostname, argv[2]);
    else
        strcpy(hostname, "localhost");

    if ((host = gethostbyname(hostname)) == NULL) {
        perror("host resolve");
        return errno;
    }
    if (argc > 3)
        port = atoi(argv[3]);
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == 1) {
        perror("socket");
        return errno;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);
    memset(&(server_addr.sin_zero), '\0', 8);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        return errno;
    }

    if (handshake(sockfd))
        printf("Server Connected.\n");
    else {
        printf("Server Refused!\n");
        close(sockfd);
        exit(0);
    }


    data = (char *)malloc(sizeof(char) * 512);
    if ((numbytes = recv(sockfd, buf, 256, 0)) == -1) {
        perror("receive");
        return errno;
    }
    if (numbytes == 0)
        return 0;
    data = decompressData(data, buf + 2, (unsigned char)buf[1]);
    stockNum = getStockCode(data, stockCode);
    updateLog(NULL, stockCode, stockNum, argv[1]);
#ifdef HAVE_LIBNCURSES
    initscr();
    curs_set(0);
#endif
    while (1) {
        if ((numbytes = recv(sockfd, buf, 256, 0)) == -1) {
            perror("receive");
            return errno;
        }
        if (numbytes == 0)
            break;
        data = decompressData(data, buf + 2, (unsigned char) buf[1]);
        updateStockPrice(data, stockCode, priceTable);
        printStockPrice(stockCode, stockNum, priceTable);
        updateLog(priceTable, stockCode, stockNum, argv[1]);
    }
    close(sockfd);
#ifdef HAVE_LIBNCURSES
    werase(stdscr);
    move(10, 20);
    printw("Server Down! Exit in 3sec(s)\n");
    refresh();
    sleep(1);
    move(10, 41);
    printw("2");
    refresh();
    sleep(1);
    move(10, 41);
    printw("1");
    refresh();
    sleep(1);
    endwin();
#else
    printf("Server Down! Exit in 3secs\n");
    sleep(3);
#endif
    return 0;
}
