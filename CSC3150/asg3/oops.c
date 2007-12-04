#include "oops.h"


int chkfn(char ch) {
    return (islower(ch) | isdigit(ch) | isupper(ch)) && !isspace(ch);
}


typedef struct entryS {
    unsigned long FstClus;
    char path[128];
    struct entryS *next;
}
qEntryCDT;


typedef struct queueS {
    qEntryCDT *head, *tail;
    int cnt;
}
queueCDT;

typedef queueCDT* queueADT;
typedef qEntryCDT* qEntryADT;


qEntryADT dequeue(queueADT Q) {
    qEntryADT entry;
    if (Q == NULL)
        return NULL;
    if (Q->head == NULL || Q->tail == NULL)
        return NULL;
    entry = Q->head;
    Q->head = entry->next;
    Q->cnt--;
    if (!Q->cnt)
        Q->tail = NULL;
    return entry;
}

void enqueue(queueADT Q, qEntryADT E) {
    if (Q == NULL || E == NULL)
        return;
    if (Q->cnt) {
        Q->tail->next = E;
        Q->tail = E;
    } else {
        Q->head = E;
        Q->tail = E;
    }
    Q->cnt++;
}

queueADT emptyQ(void) {
    queueADT Q;
    Q = malloc(sizeof(queueCDT));
    Q->cnt = 0;
    Q->head = NULL;
    Q->tail = NULL;

    return Q;
}



void printBoot(int fid) {
    bootEntryStruct *bootEntry;

    bootEntry = getBootEntry(fid);
    printf("Number of FATs = %d\n", bootEntry->BPB_NumFATs);
    printf("Number of bytes per sector = %d\n", bootEntry->BPB_BytsPerSec);
    printf("Number of sectors per cluster = %d\n", bootEntry->BPB_SecPerClus);
    printf("Number of reserved sectors = %d\n", bootEntry->BPB_RsvdSecCnt);

    free(bootEntry);
}


int main(int argc, char const *argv[]) {
    int fid;

    if (argc < 2 || argv[1][0] != '-' || strlen(argv[1]) != 2) {
        printf("Usage:\n%s -i\tPrint boot sector information.\n", argv[0]);
        printf("%s -l\tList root directory.\n", argv[0]);
        printf("%s -f\tRecover a deleted file\n", argv[0]);
        return 0;
    }

    fid = open("/dev/ram", O_RDWR);
    if (fid == -1) {
        perror("Open RAMdisk");
        return errno;
    }

    switch (argv[1][1]) {
        char fn[128];
        char tmp[128];
        bootEntryStruct *bootEntry;
        unsigned long clus;
        int i;

    case 'i':
        printBoot(fid);
        break;
    case 'l':
        listRoot(fid);
        break;
    case 'r':
        if (argc != 3) {
            printf("File not found!\n");
            return 0;
        }
        memcpy(fn, argv[2], strlen(argv[2]));
        fn[strlen(argv[2])] = '\0';
        bootEntry = getBootEntry(fid);
        strcpy(tmp, "/");
        strcat(tmp, fn);
        clus = findClus(fid, bootEntry->BPB_RootClus, tmp);
        rcvy(fid, clus, fn, NULL);
        free(bootEntry);
        break;
    case 'd':
        if (argc != 3) {
            printf("File not found!\n");
            return 0;
        }
        memcpy(fn, argv[2], strlen(argv[2]));
        fn[strlen(argv[2])] = '\0';
        memcpy(tmp, fn, 8);
        tmp[8] = '\0';
        if (strcmp(tmp, "/mnt/rd/")) {
            printf("File not found!\n");
            return 0;
        }
        strcpy(tmp, fn + 7);
        bootEntry = getBootEntry(fid);
        clus = findClus(fid, bootEntry->BPB_RootClus, tmp);
        i = strlen(tmp) - 1;
        while (tmp[i] != '/')
            i--;
        rcvy(fid, clus, tmp + i + 1, NULL);
        free(bootEntry);
        break;
    case 'b':
        if (argc != 3) {
            printf("File not found!\n");
            return 0;
        }
        memcpy(fn, argv[2], strlen(argv[2]));
        fn[strlen(argv[2])] = '\0';
        tryAllRcvy(fid, fn);

        break;

    default:
        printf("Unknown command -%c.\n", argv[1][1]);
    }


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

int write_sectors(int fid, int bps, int sector_number, unsigned char *buffer, int num_sectors) {
    int dest, len; /* used for error checking only */
    dest = lseek(fid, sector_number * bps, SEEK_SET);
    if (dest != sector_number*bps) {
        printf("seek:cannot locate the sector\n");
        return -1;
    }
    if (bps*num_sectors > BUF_MAX) {
        printf("sector write:buffer overflow\n");
        return -1;
    }
    len = write(fid, buffer, bps * num_sectors);
    if (len != bps*num_sectors) {
        printf("write:sector read failed\n");
        perror("write");
        return -1;
    }
    return len;
}

void listRoot(int fid) {
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
        numBytes = read_sectors(fid, bps, (clus - 2 ) * spc + rsc + nft * ftsz32, buf + len, spc);
        if (numBytes == -1)
            return;
        len += numBytes;
        //memcpy(dirE, buf + len - sizeof(dirEntryStruct), sizeof(dirEntryStruct));
        clus = lookupFAT(fid, clus);
        if (!clus || clus >= 0xffffff0)
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
            while (i && !chkfn(name[i]))
                i--;
            if (dirE->DIR_Attr&ATTR_VOLLAB || dirE->DIR_Attr&ATTR_SUBDIR)
                name[++i] = '/';
            else {
                name[++i] = '.';
                memcpy(name + i + 1, dirE->DIR_Name + 8, 3);
                i += 3;
                while (i && !chkfn(name[i]))
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
    clusr = clus * 4 / bps + rsc;
    //printf("clus=%d clurs=%d\n", clus, clusr);
    numBytes = read_sectors(fid, bps, clusr * spc, (unsigned char *)buf, spc);
    if (numBytes == -1) {
        printf("read sector: failed\n");
        return 0;
    }
    //buf_dup(buf, 512);
    return buf[clus-(clusr-rsc)*bps/4];
}

int rcvy(int fid, unsigned long clus, char *filename, char *newname) {
    bootEntryStruct *bootE;
    dirEntryStruct *dirE;
    int bps, spc, rsc, nft, rc, ftsz32, numBytes;
    unsigned char *buf;
    char name[16];

    dirE = malloc(sizeof(dirEntryStruct));
    bootE = getBootEntry(fid);
    if (bootE == NULL)
        return 0;
    bps = bootE->BPB_BytsPerSec;
    spc = bootE->BPB_SecPerClus;
    rsc = bootE->BPB_RsvdSecCnt;
    rc = bootE->BPB_RootClus;
    nft = bootE->BPB_NumFATs;
    ftsz32 = bootE->BPB_FATSz32;
    free(bootE);

    buf = malloc(sizeof(unsigned char) * bps * spc);
    for (;;) {
        int i;
        numBytes = read_sectors(fid, bps, (clus - 2) * spc + rsc + nft * ftsz32, buf, spc);
        if (numBytes == -1)
            return 0;


        for (i = 0;i < numBytes / sizeof(dirEntryStruct);i++) {
            memcpy(dirE, buf + i*sizeof(dirEntryStruct), sizeof(dirEntryStruct));

            if (dirE->DIR_Name[0] == 0)
                break;

            if (dirE->DIR_Name[0] == 0xe5 && !(dirE->DIR_Attr&(ATTR_LFN | ATTR_VOLLAB | ATTR_SUBDIR))) {
                int j = 7;
                unsigned long FstClus;
                memcpy(name, dirE->DIR_Name, 8);
                while (j && !chkfn(name[j]))
                    j--;
                name[++j] = '.';
                memcpy(name + j + 1, dirE->DIR_Name + 8, 3);
                j += 3;
                while (j && !chkfn(name[j]))
                    j--;
                name[++j] = '\0';
                strlow(name);
                if (!strcmp(name + 1, filename + 1)) {
                    FstClus = ((dirE->DIR_FstClusHI) << 16) | dirE->DIR_FstClusLO;
                    if (dirE->DIR_FileSize <= bps*spc) {
                        dirE->DIR_Name[0] = toupper(filename[0]);
                        if (newname != NULL) {
                            int i = 0, j;
                            while (i < strlen(newname)) {
                                if (newname[i] == '.')
                                    break;
                                dirE->DIR_Name[i] = toupper(newname[i]);
                                i++;
                            }
                            j = 7;
                            while (i < strlen(newname) && j < 12) {
                                i++;
                                j++;
                                dirE->DIR_Name[j] = toupper(newname[i]);
                            }

                        }
                        memcpy(buf + i*sizeof(dirEntryStruct), dirE, sizeof(dirEntryStruct));
                        write_sectors(fid, bps, (clus - 2) * spc + rsc + nft * ftsz32, buf, spc);
                        updateFAT(fid, FstClus, EOF);
                        return 1;
                    }
                }
            }
        }
        clus = lookupFAT(fid, clus);
        if (!clus || clus >= 0xffffff0)
            break;
    }
    return 0;
}

void updateFAT(int fid, unsigned long clus, unsigned long nextClus) {
    bootEntryStruct *bootE;
    unsigned long *buf;
    unsigned short rsc;
    unsigned char nfat;
    unsigned long ftsz32, clusr, rc;
    unsigned short bps;
    unsigned char spc;
    int numBytes, i;

    bootE = getBootEntry(fid);
    if (bootE == NULL)
        return;
    rsc = bootE->BPB_RsvdSecCnt;
    ftsz32 = bootE->BPB_FATSz32;
    bps = bootE->BPB_BytsPerSec;
    spc = bootE->BPB_SecPerClus;
    rc = bootE->BPB_RootClus;
    nfat = bootE->BPB_NumFATs;
    free(bootE);

    buf = (unsigned long *)malloc(sizeof(unsigned char) * bps * spc);
    for (i = 0;i < nfat;i++) {
        clusr = clus * 4 / bps + rsc + i * ftsz32;
        //printf("clus=%d clurs=%d\n", clus, clusr);
        numBytes = read_sectors(fid, bps, clusr * spc, (unsigned char *)buf, spc);
        if (numBytes == -1) {
            printf("read sector: failed\n");
            return;
        }
        buf[clus-(clusr-rsc-i*ftsz32)*bps/4] = nextClus;
        //printf("sector:%d clus:%d modified %d\n", clusr*spc, clus, clus - (clusr - rsc - i*ftsz32)*bps / 4);
        write_sectors(fid, bps, clusr * spc, (unsigned char *)buf, spc);
        //buf_dup(buf, 512);
    }
}

unsigned long findClus(int fid, unsigned long clus, char *filename) {
    bootEntryStruct *bootE;
    dirEntryStruct *dirE;
    int numBytes;
    unsigned short rsc;
    unsigned long ftsz32, rc;
    unsigned char nfat;
    unsigned short bps;
    unsigned char spc;
    int i;
    unsigned char *buf;
    char *pos;
    char fn[9], fn2[9];

    if (filename == NULL || !clus)
        return 0;

    bootE = getBootEntry(fid);
    if (bootE == NULL)
        return 0;
    rsc = bootE->BPB_RsvdSecCnt;
    ftsz32 = bootE->BPB_FATSz32;
    bps = bootE->BPB_BytsPerSec;
    spc = bootE->BPB_SecPerClus;
    rc = bootE->BPB_RootClus;
    nfat = bootE->BPB_NumFATs;
    free(bootE);

    pos = strchr(filename + 1, '/');
    buf = malloc(sizeof(unsigned char) * bps * spc);
    dirE = malloc(sizeof(dirEntryStruct));
    if (pos == NULL)
        return 2;
    for (;;) {
        int j;
        numBytes = read_sectors(fid, bps, (clus - 2) * spc + rsc + nfat * ftsz32, buf, spc);

        for (i = 0;i < numBytes / sizeof(dirEntryStruct);i++) {
            memcpy(dirE, buf + i*sizeof(dirEntryStruct), sizeof(dirEntryStruct));
            if (dirE->DIR_Name[0] && dirE->DIR_Name[0] != 0xe5 && (dirE->DIR_Attr&ATTR_SUBDIR)) {
                unsigned long FstClus;

                FstClus = ((dirE->DIR_FstClusHI) << 16) | dirE->DIR_FstClusLO;
                memcpy(fn, dirE->DIR_Name, 8);
                j = 7;
                while (j && !chkfn(fn[j]))
                    j--;
                fn[j+1] = 0;
                j = 0;
                while (chkfn(filename[j+1])) {
                    fn2[j] = filename[j+1];
                    j++;
                }
                fn2[j] = 0;
                strlow(fn);
                strlow(fn2);
                if (!strcmp(fn2, fn)) {
                    if (strchr(pos + 1, '/') == NULL)
                        return FstClus;
                    else
                        return findClus(fid, FstClus, pos);
                }
            }
            if (!dirE->DIR_Name[0])
                break;
        }

        clus = lookupFAT(fid, clus);
        if (!clus || clus >= 0xffffff0)
            break;
    }
    return 0;
}


void tryAllRcvy(int fid, char *filename) {
    bootEntryStruct *bootE;
    dirEntryStruct *dirE;
    int numBytes;
    unsigned short rsc;
    unsigned long ftsz32, rc, clus;
    unsigned char nfat;
    unsigned short bps;
    unsigned char spc;
    int i;
    unsigned char *buf;
    char fn[12];
    queueADT Q;
    qEntryADT queueE;
    char delFiles[100][128];
    int foundN = 0;

    if (filename == NULL)
        return;

    //boot sector information read
    bootE = getBootEntry(fid);
    if (bootE == NULL)
        return;
    rsc = bootE->BPB_RsvdSecCnt;
    ftsz32 = bootE->BPB_FATSz32;
    bps = bootE->BPB_BytsPerSec;
    spc = bootE->BPB_SecPerClus;
    rc = bootE->BPB_RootClus;
    nfat = bootE->BPB_NumFATs;
    free(bootE);

    // Queue initialize
    Q = emptyQ();
    // queueE initialize, add ROOT to queue as the start point of WFS
    queueE = malloc(sizeof(qEntryCDT));
    queueE->FstClus = rc;
    strcpy(queueE->path, "/");
    enqueue(Q, queueE);


    //buffer initialize
    buf = malloc(sizeof(unsigned char) * bps * spc);
    dirE = malloc(sizeof(dirEntryStruct));
    memset(delFiles, 0, sizeof(delFiles));


    //WFS begin
    while (Q->cnt) { //while Q is not empty
        queueE = dequeue(Q);
        clus = queueE->FstClus;

        for (;;) {
            int j;
            numBytes = read_sectors(fid, bps, (clus - 2) * spc + rsc + nfat * ftsz32, buf, spc);
            memset(fn, 0, sizeof(fn));

            for (i = 0;i < numBytes / sizeof(dirEntryStruct);i++) {
                memcpy(dirE, buf + i*sizeof(dirEntryStruct), sizeof(dirEntryStruct));
                if (dirE->DIR_Name[0] == 0)
                    break;
                if (dirE->DIR_Name[0] == '.')
                    continue;

                if (dirE->DIR_Attr&ATTR_LFN)
                    continue;

                if (dirE->DIR_Name[0] != 0xe5 && (dirE->DIR_Attr&ATTR_SUBDIR)) { //when a subdir is found, add to the Queue
                    unsigned long FstClus;
                    qEntryADT newE;

                    FstClus = ((dirE->DIR_FstClusHI) << 16) | dirE->DIR_FstClusLO;

                    newE = malloc(sizeof(qEntryCDT));
                    strcpy(newE->path, queueE->path);

                    memcpy(fn, dirE->DIR_Name, 8);
                    j = 7;
                    while (j && !chkfn(fn[j]))
                        j--;
                    fn[++j] = '/';
                    fn[j+1] = '\0';
                    strlow(fn);

                    strcat(newE->path, fn);
                    newE->FstClus = FstClus;

                    enqueue(Q, newE);


                } else if (dirE->DIR_Name[0] == 0xe5 && !(dirE->DIR_Attr&ATTR_SUBDIR)) {
                    memcpy(fn, dirE->DIR_Name, 8);
                    j = 7;
                    while (j && !chkfn(fn[j]))
                        j--;
                    j++;
                    fn[j++] = '.';
                    memcpy(fn + j, dirE->DIR_Name + 8, 3);
                    j += 3;
                    while (j && !chkfn(fn[j]))
                        j--;
                    fn[j+1] = '\0';
                    strlow(fn);
                    strlow(filename);
                    if (!strcmp(filename + 1, fn + 1)) {
                        strcpy(delFiles[foundN], queueE->path);
                        strcat(delFiles[foundN], filename);
                        foundN++;
                    }
                }
            }

            clus = lookupFAT(fid, clus);
            if (!clus || clus >= 0xffffff0)
                break;
        }
    }
    //search finished
    if (!foundN) {
        printf("No deleted _%s is found\n", filename + 1);
    } else {
        char tmp[128], tmp2[128];
        int c;
        unsigned long clus;
        bootEntryStruct *bootE;

        if (foundN == 1) {
            printf("1 deleted _%s is found\n", filename + 1);
            c = 1;
        } else {
            printf("%d deleted _%s are found, please select one of them to recover:\n", foundN, filename + 1);
            for (i = 0;i < foundN;i++) {
                strcpy(tmp, delFiles[i]);
                tmp[strlen(tmp)-strlen(filename)] = '_';
                printf("%d.  %s\n", i + 1, tmp);
            }
            printf("Enter your choice : ");
            scanf("%d", &c);
            if (c < 1 || c > foundN) {
                printf("Invalid choice!\n");
                return;
            }
        }
        c--;

        bootE = getBootEntry(fid);
        clus = findClus(fid, bootE->BPB_RootClus, delFiles[c]);

        strcpy(tmp, filename);
        strcpy(tmp2, delFiles[i]);
        tmp2[strlen(tmp2)-strlen(filename)] = '\0';
        strcat(tmp2, tmp);
        while (hasFile(fid, clus, tmp)) {
            printf("Recovering %s fails due to another file %s exists.\n", tmp2, tmp2);
            printf("Enter another file name : ");
            scanf("%s", tmp);
            strcpy(tmp2, delFiles[c]);
            tmp2[strlen(tmp2)-strlen(filename)] = '\0';
            strcat(tmp2, tmp);
        }

        if (rcvy(fid, clus, filename, tmp))
            printf("%s is recovered\n", tmp2);
    }
    return;
}

int hasFile(int fid, unsigned clus, char *filename) {
    bootEntryStruct *bootE;
    dirEntryStruct *dirE;
    int bps, spc, rsc, nft, rc, ftsz32, numBytes;
    unsigned char *buf;
    char name[16];

    dirE = malloc(sizeof(dirEntryStruct));
    bootE = getBootEntry(fid);
    if (bootE == NULL)
        return 0;
    bps = bootE->BPB_BytsPerSec;
    spc = bootE->BPB_SecPerClus;
    rsc = bootE->BPB_RsvdSecCnt;
    rc = bootE->BPB_RootClus;
    nft = bootE->BPB_NumFATs;
    ftsz32 = bootE->BPB_FATSz32;
    free(bootE);

    buf = malloc(sizeof(unsigned char) * bps * spc);
    for (;;) {
        int i;
        numBytes = read_sectors(fid, bps, (clus - 2) * spc + rsc + nft * ftsz32, buf, spc);
        if (numBytes == -1)
            return 0;

        for (i = 0;i < numBytes / sizeof(dirEntryStruct);i++) {
            memcpy(dirE, buf + i*sizeof(dirEntryStruct), sizeof(dirEntryStruct));

            if (dirE->DIR_Name[0] == 0)
                break;

            if (dirE->DIR_Name[0] != 0xe5 && !(dirE->DIR_Attr&(ATTR_LFN | ATTR_VOLLAB | ATTR_SUBDIR))) {
                int j = 7;
                memcpy(name, dirE->DIR_Name, 8);
                while (j && !chkfn(name[j]))
                    j--;
                name[++j] = '.';
                memcpy(name + j + 1, dirE->DIR_Name + 8, 3);
                j += 3;
                while (j && !chkfn(name[j]))
                    j--;
                name[++j] = '\0';
                strlow(name);
                if (!strcmp(name, filename))
                    return 1;
            }
        }
        clus = lookupFAT(fid, clus);
        if (!clus || clus >= 0xffffff0)
            break;
    }
    return 0;
}

