#include "oops.h"

int main(int argc, char const *argv[]) {
  bootEntryStruct *bootEntry;
  int fid;
  char fn[16];

  if (argc != 2) {
    printf("Usage:%s <disk>\n", argv[0]);
    return 0;
  }

  fid = open(argv[1], O_RDWR);
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
  printf("input a filename to recovery:");
  scanf("%s",fn);
  rcvy(fid,fn);
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
      while (!((name[i]>='a'&&name[i]<='z')||(name[i]>='A'&&name[i]<='Z')||(name[i]>='0'&&name[i]<='9')))
        i--;
      if (dirE->DIR_Attr&ATTR_VOLLAB || dirE->DIR_Attr&ATTR_SUBDIR)
        name[++i] = '/';
      else {
        name[++i] = '.';
        memcpy(name + i + 1, dirE->DIR_Name + 8, 3);
        i += 3;
        while (!((name[i]>='a'&&name[i]<='z')||(name[i]>='A'&&name[i]<='Z')||(name[i]>='0'&&name[i]<='9')))
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

void rcvy(int fid,char *filename){
  bootEntryStruct *bootE;
  dirEntryStruct *dirE;
  int bps, spc, rsc, nft, rc, ftsz32, numBytes;
  int i, len = 0;
  unsigned char *buf;
  unsigned long clus;
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
  clus=rc;

  buf = malloc(sizeof(unsigned char) * bps * spc);
  for (;;) {
    int i;
    numBytes = read_sectors(fid, bps, (clus - rc + rsc) * spc + nft * ftsz32, buf, spc);
    if (numBytes == -1)
      return;


    for(i=0;i<numBytes/sizeof(dirEntryStruct);i++){
      memcpy(dirE,buf+i*sizeof(dirEntryStruct),sizeof(dirEntryStruct));

      if (dirE->DIR_Name[0] == 0xe5 && !(dirE->DIR_Attr&ATTR_LFN)) {
        int j = 7;
        unsigned long FstClus;
        memcpy(name, dirE->DIR_Name, 8);
        while (!((name[j]>='a'&&name[j]<='z')||(name[j]>='A'&&name[j]<='Z')||(name[j]>='0'&&name[j]<='9')))
          j--;
        if (!(dirE->DIR_Attr&ATTR_VOLLAB || dirE->DIR_Attr&ATTR_SUBDIR)){
          name[++j] = '.';
          memcpy(name + j + 1, dirE->DIR_Name + 8, 3);
          j += 3;
          while (!((name[j]>='a'&&name[j]<='z')||(name[j]>='A'&&name[j]<='Z')||(name[j]>='0'&&name[j]<='9')))
            j--;
        }
        name[++j] = '\0';
        strlow(name);
        if(!strcmp(name+1,filename+1)){
          printf("?%s found!!\n",name+1);
          FstClus=((dirE->DIR_FstClusHI)<<16)|dirE->DIR_FstClusLO;
          printf("first cluster is %d,next %d\n",FstClus,dirE->DIR_Attr&ATTR_LFN);
          if(dirE->DIR_FileSize<=bps*spc){
            printf("?%s:%dBytes is less than one sector, it can be recoveried.\n",name+1,dirE->DIR_FileSize);
            dirE->DIR_Name[0]=filename[0];
            memcpy(buf+i*sizeof(dirEntryStruct),dirE,sizeof(dirEntryStruct));
            write_sectors(fid, bps, (clus - rc + rsc) * spc + nft * ftsz32, buf, spc);
            updateFAT(fid,FstClus,0xffffff8);
            break;
          }
          else
            printf("?%s:%dBytes is larger than one sector, it cannot be recoveried.\n",name+1,dirE->DIR_FileSize);
        }
      }
    }
    clus = lookupFAT(fid, clus);
    if (!clus || clus >= 0xffffff0)
      break;
  }
}

void updateFAT(int fid,unsigned long clus,unsigned long nextClus){
  bootEntryStruct *bootE;
  unsigned long *buf;
  unsigned short rsc;
  unsigned char nfat;
  unsigned long ftsz32, clusr;
  unsigned short bps;
  unsigned char spc;
  int numBytes,i;

  bootE = getBootEntry(fid);
  if (bootE == NULL)
    return 0;
  rsc = bootE->BPB_RsvdSecCnt;
  ftsz32 = bootE->BPB_FATSz32;
  bps = bootE->BPB_BytsPerSec;
  spc = bootE->BPB_SecPerClus;
  nfat = bootE->BPB_NumFATs;
  free(bootE);

  buf = (unsigned long *)malloc(sizeof(unsigned char) * bps * spc);
  for(i=0;i<nfat;i++){
    clusr = clus *4 / bps + rsc + i*ftsz32;
    //printf("clus=%d clurs=%d\n", clus, clusr);
    numBytes = read_sectors(fid, bps, clusr * spc, (unsigned char *)buf, spc);
    if (numBytes == -1) {
      printf("read sector: failed\n");
      return 0;
    }
    buf[clus-(clusr-rsc-i*ftsz32)*bps/4]=nextClus;
    printf("sector:%d clus:%d modified %d\n",clusr*spc,clus,clus-(clusr-rsc-i*ftsz32)*bps/4);
    write_sectors(fid, bps, clusr * spc, (unsigned char *)buf, spc);
    //buf_dup(buf, 512);
  }
}
