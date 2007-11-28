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
    if(Q->cnt){
      Q->tail->next = E;
      Q->tail = E;
    }else{
      Q->head=E;
      Q->tail=E;
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



int main(int argc, char const *argv[]) {
  bootEntryStruct *bootEntry;
  int fid;
  char fn[128];
  char *pos;
  unsigned long clus;

  if (argc < 2) {
    printf("Usage:%s <disk>\n", argv[0]);
    return 0;
  }


  fid = open(argv[1], O_RDWR);
  if (fid == -1) {
    perror("open");
    return errno;
  }

  if(argc>2)
    tryAllRcvy(fid,argv[2]);

  bootEntry = getBootEntry(fid);
  if (bootEntry != NULL) {
    printf("Boot Sector Information about %s:\n", argv[1]);
    printf("nft=%d bps=%d spc=%d res=%d\n", bootEntry->BPB_NumFATs, bootEntry->BPB_BytsPerSec, bootEntry->BPB_SecPerClus, bootEntry->BPB_RsvdSecCnt);
    printf("root Clus:%d\n", (int)bootEntry->BPB_RootClus);
  }

  read_root(fid);
  printf("input a filename to recovery:");
  scanf("%s", fn);
  clus = findClus(fid, 2, fn);
  pos = strchr(fn, '/');
  while (strchr(pos + 1, '/') != NULL)
    pos = strchr(pos + 1, '/');
  rcvy(fid, clus, (char *)(pos + 1));
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

void rcvy(int fid, unsigned long clus, char *filename) {
  bootEntryStruct *bootE;
  dirEntryStruct *dirE;
  int bps, spc, rsc, nft, rc, ftsz32, numBytes;
  int i, len = 0;
  unsigned char *buf;
  char name[16];

  dirE = malloc(sizeof(dirEntryStruct));
  bootE = getBootEntry(fid);
  if (bootE == NULL)
    return;
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
    numBytes = read_sectors(fid, bps, (clus - rc + rsc) * spc + nft * ftsz32, buf, spc);
    if (numBytes == -1)
      return;


    for (i = 0;i < numBytes / sizeof(dirEntryStruct);i++) {
      memcpy(dirE, buf + i*sizeof(dirEntryStruct), sizeof(dirEntryStruct));

      if(dirE->DIR_Name[0]==0)
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
            dirE->DIR_Name[0] = filename[0];
            memcpy(buf + i*sizeof(dirEntryStruct), dirE, sizeof(dirEntryStruct));
            write_sectors(fid, bps, (clus - rc + rsc) * spc + nft * ftsz32, buf, spc);
            updateFAT(fid, FstClus, EOF);
            break;
          }
        }
      }
    }
    clus = lookupFAT(fid, clus);
    if (!clus || clus >= 0xffffff0)
      break;
  }
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
    return 0;
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
      return 0;
    }
    buf[clus-(clusr-rsc-i*ftsz32)*bps/4] = nextClus;
    printf("sector:%d clus:%d modified %d\n", clusr*spc, clus, clus - (clusr - rsc - i*ftsz32)*bps / 4);
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
  int i, len = 0;
  unsigned char *buf;
  char *str, *pos;
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
    numBytes = read_sectors(fid, bps, (clus - rc + rsc) * spc + nfat * ftsz32, buf, spc);

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
      if(!dirE->DIR_Name[0])
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
  int i, len = 0;
  unsigned char *buf;
  char *str;
  char fn[12];
  queueADT Q;
  qEntryADT queueE;
  char delFiles[100][128];
  int foundN = 0;

  if (filename == NULL)
    return 0;

  //boot sector information read
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
  memset(delFiles,0,sizeof(delFiles));


  //WFS begin
  while (Q->cnt) { //while Q is not empty
    queueE = dequeue(Q);

    clus = queueE->FstClus;

    for (;;) {
      int j;
      numBytes = read_sectors(fid, bps, (clus - rc + rsc) * spc + nfat * ftsz32, buf, spc);

      for (i = 0;i < numBytes / sizeof(dirEntryStruct);i++) {
        memcpy(dirE, buf + i*sizeof(dirEntryStruct), sizeof(dirEntryStruct));
        if (dirE->DIR_Name[0]==0)
          break;
        if (dirE->DIR_Name[0]=='.')
          continue;

        if(dirE->DIR_Attr&ATTR_LFN)
          continue;
        
        if (dirE->DIR_Name[0] != 0xe5 && (dirE->DIR_Attr&ATTR_SUBDIR)) { //when a subdir is found, add to the Queue
          unsigned long FstClus;
          qEntryADT newE;

          FstClus = ((dirE->DIR_FstClusHI) << 16) | dirE->DIR_FstClusLO;

          newE = malloc(sizeof(qEntryCDT));
          newE->FstClus = rc;
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
  if(foundN){
    for(i=0;i<foundN;i++)
      printf("%d. %s\n",i+1,delFiles[i]);

  }
  return 0;
}

