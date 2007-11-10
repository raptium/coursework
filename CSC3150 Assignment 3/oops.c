#include "oops.h"

int main(int argc, char const *argv[]) {
    bootEntryStruct *bootEntry;
    int fid;

    if (argc != 2) {
        printf("Usage:%s <disk>\n", argv[0]);
        return 0;
    }

    fid = open(argv[1], O_RDONLY);
    if (fid == -1) {
        perror("open");
        return errno;
    }

    bootEntry = getBootEntry(fid);
    if (bootEntry != NULL) {
        printf("Boot Sector Information about %s:\n", argv[1]);
        printf("nft=%d bps=%d spc=%d res=%d\n", bootEntry->BPB_NumFATs, bootEntry->BPB_BytsPerSec, bootEntry->BPB_SecPerClus, bootEntry->BPB_RsvdSecCnt);
        printf("root Clus:%d\n", (int)bootEntry->BPB_RootClus);
    }

    read_root(fid);
    close(fid);
    return 0;
}

/* get boot sector of a specified disk drive
return value is a pointer of bootEntryStruct
NULL is returned if error occurs
*/

bootEntryStruct *getBootEntry(int fid) {
    bootEntryStruct *bootEntry;
    int numBytes;

    bootEntry = malloc(sizeof(bootEntryStruct));
    lseek(fid, 0, SEEK_SET);
    numBytes = read(fid, bootEntry, 90);
    if (numBytes == -1) {
        perror("read");
        return NULL;
    }
    return bootEntry;
}

/* go to the sector indicated by "sector_number", then read n contiguous sectors (n =
num_sectors), and write the result into "buffer" */
/* Remember to clear the buffer first */
int read_sectors(int fid, int bps, int sector_number, unsigned char *buffer, int num_sectors) {
    int dest, len; /* used for error checking only */
    dest = lseek(fid, sector_number * bps, SEEK_SET);
    if (dest != sector_number*bps) {
        printf("seek:cannot locate the sector\n");
        return -1;
    }
    if (bps*num_sectors > BUF_MAX) {
        printf("sector read:buffer overflow\n");
        return -1;
    }
    len = read(fid, buffer, bps * num_sectors);
    if (len != bps*num_sectors) {
        printf("read:sector read failed\n");
        return -1;
    }
    return len;
}

void read_root(int fid) {
    bootEntryStruct *bootE;
    dirEntryStruct *dirE;
    lfnEntryStruct *lfnE;
    int bps, spc, rsc, nft, rc, ftsz32, numBytes;
    int i, len = 0;
    unsigned char *buf;
    unsigned long clus;
    char name[256];

    dirE = malloc(sizeof(dirEntryStruct));
    lfnE = malloc(sizeof(lfnEntryStruct));
    bootE = getBootEntry(fid);
    if (bootE == NULL)
        return;
    bps = bootE->BPB_BytsPerSec;
    spc = bootE->BPB_SecPerClus;
    rsc = bootE->BPB_RsvdSecCnt;
    rc = bootE->BPB_RootClus;
    nft = bootE->BPB_NumFATs;
    ftsz32 = bootE->BPB_FATSz32;
    clus = rc;
    buf = malloc(sizeof(unsigned char) * bps * spc);
    for (;;) {
        numBytes = read_sectors(fid, bps, (clus - rc + rsc) * spc + nft * ftsz32, buf + len, spc);
        if (numBytes == -1)
            return;
        len += numBytes;
        memcpy(dirE, buf + len - sizeof(dirEntryStruct), sizeof(dirEntryStruct));
        clus = lookupFAT(fid, clus);
        if (!clus || clus == 0xfffffff || clus == 0xffffff0)
            break;
    }

    for (i = 0; i < len / sizeof(dirEntryStruct); ++i) {
        memcpy(dirE, buf + i*sizeof(dirEntryStruct), sizeof(dirEntryStruct));
        if (dirE->DIR_Name[0] == 0)
            return;
        if (dirE->DIR_Attr&ATTR_LFN) {
            memcpy(lfnE, dirE, sizeof(lfnEntryStruct));
        } else if (dirE->DIR_Name[0] != 0xe5) {
            int i = 7;
            memcpy(name, dirE->DIR_Name, 8);
            while (name[i] == 0x20)
                i--;
            if (dirE->DIR_Attr&ATTR_VOLLAB || dirE->DIR_Attr&ATTR_SUBDIR)
                name[++i] = '/';
            else {
                name[++i] = '.';
                memcpy(name + i + 1, dirE->DIR_Name + 8, 3);
                i += 4;
                while (name[i] == 0x20)
                    i--;
                name[++i] = '+';
            }

            name[++i] = '\0';
            strlow(name);
            printf("%s\n", name);
        }
    }

    //buf_dup(dirE,32);
    if (dirE != NULL)
        free(dirE);
    if (lfnE != NULL)
        free(lfnE);
    if (bootE != NULL)
        free(bootE);
    if (buf != NULL)
        free(buf);
    return;
}

void buf_dup(void *buf, int size) {
    unsigned char *bufd;
    int i;
    bufd = (unsigned char *)buf;
    for (i = 0; i < size; ++i) {
        printf("%d ", bufd[i]);
    }
}

int strlow(char *str) {
    int i = 0;
    if (str == NULL)
        return -1;
    while (str[i]) {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += ('a' -'A');
        i++;
    }
    return i;
}

unsigned long lookupFAT(int fid, unsigned long clus) {
    bootEntryStruct *bootE;
    unsigned long *buf;
    unsigned short rsc;
    unsigned long ftsz32, clusr;
    unsigned short bps;
    unsigned char spc;
    int numBytes;

    bootE = getBootEntry(fid);
    if (bootE == NULL)
        return 0;
    rsc = bootE->BPB_RsvdSecCnt;
    ftsz32 = bootE->BPB_FATSz32;
    bps = bootE->BPB_BytsPerSec;
    spc = bootE->BPB_SecPerClus;
    free(bootE);
    buf = (unsigned long *)malloc(sizeof(unsigned char) * bps * spc);
    clusr = clus / bps * 4 + rsc;
    //printf("clus=%d clurs=%d\n", clus, clusr);
    numBytes = read_sectors(fid, bps, clusr * spc, (unsigned char *)buf, spc);
    if (numBytes == -1) {
        printf("read sector: failed\n");
        return 0;
    }
    //buf_dup(buf, 512);
    return buf[clus-(clusr-rsc)*bps/4];
}