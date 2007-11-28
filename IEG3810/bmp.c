#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>


void byte2hex(unsigned char B) {
    printf("0x");
    if (B >> 4 > 9)
        printf("%c", (B >> 4) - 10 + 'a');
    else
        printf("%c", (B >> 4) + '0');
    B &= 0x0f;
    if (B > 9)
        printf("%c", B - 10 + 'a');
    else
        printf("%c", B + '0');
    return;
}

void printScreen(unsigned char screen[8][128]) {
    int i, j, l, r, s, e;

    l = 0;
    r = 0;
    for (i = 0;i < 8;i++) {
        s = 0;
        for (;;) {
            if (screen[i][s] || s == 63)
                break;
            s++;
        }
        e = 63;
        for (;;) {
            if (screen[i][e] || e == 0)
                break;
            e--;
        }
        if (e >= s) {
            printf("glcd_pos(%d, %d, %d);\n", 2, i, s);
            for (j = s;j <= e; j++) {
                int k = 0;
                while (j + k != e && screen[i][j+k] == screen[i][j])
                    k++;
                k--;
                if (k <= 2) {
                    printf("glcd_wd(2, ");
                    byte2hex(screen[i][j]);
                    printf(");\n");
                } else {
                    if (screen[i][j]) {
                        printf("for(i=0;i<%d;i++)\n", k);
                        printf("\tglcd_wd(2, ");
                        byte2hex(screen[i][j]);
                        printf(");\n");
                    } else
                        printf("glcd_pos(2, %d, %d);\n", i, j + k + 1);
                    j += k;
                }
            }
        }

        s = 64;
        for (;;) {
            if (screen[i][s] || s == 127)
                break;
            s++;
        }
        e = 127;
        for (;;) {
            if (screen[i][e] || e == 64)
                break;
            e--;
        }
        if (e < s)
            continue;
        printf("glcd_pos(%d, %d, %d);\n", 1, i, s - 64);
        for (j = s;j <= e; j++) {
            int k = 0;
            while (j + k != e && screen[i][j+k] == screen[i][j])
                k++;
            k--;
            if (k <= 2) {
                printf("glcd_wd(1, ");
                byte2hex(screen[i][j]);
                printf(");\n");
            } else {
                if (screen[i][j]) {
                    printf("for(i=0;i<%d;i++)\n", k);
                    printf("\tglcd_wd(1, ");
                    byte2hex(screen[i][j]);
                    printf(");\n");
                } else
                    printf("glcd_pos(1, %d, %d);\n", i, -64 + j + k + 1);
                j += k;
            }
        }
    }
}


int main(int argc, char const *argv[]) {
    int fid;
    int i, j, offset;
    unsigned char buf;
    unsigned char screen[8][128];

    memset(screen, 0, sizeof(screen));
    fid = open(argv[1], O_RDONLY);
    //fid = open("c:\\temp\\GameOver.bmp", O_RDONLY);
    if (fid == -1) {
        perror("open");
        return errno;
    }
    lseek(fid, 0x0a, SEEK_SET);
    read(fid, &offset, 4);
    lseek(fid, offset, SEEK_SET);
    for (i = 0;i < 128;i++) {
        for (j = 0;j < 8;j++) {
            read(fid, &buf, 1);
            screen[7-j][127-i] = ~buf;
            //byte2hex(buf);
            //printf(" ");
        }
        //printf("\n");
    }
    printScreen(screen);
    close(fid);
    system("PAUSE");
    return 0;
}
