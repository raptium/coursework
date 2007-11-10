#ifndef BOOT_H
#define BOOT_H 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


#define BUF_MAX 32768	/* Maximum value of the buffer used in sector read */

/* File Attributes Flags */
#define ATTR_RDONLY 0x01
#define ATTR_HIDN 0x02
#define ATTR_SYS 0x04
#define ATTR_VOLLAB 0x08
#define ATTR_SUBDIR 0x10
#define ATTR_ARCV 0x20
#define ATTR_LFN 0x0f


#pragma pack(push,1)			// BYTE align in memory (no padding)
typedef struct BootEntry {
    unsigned char BS_jmpBoot[3];	/* Assembly instruction to jump to boot code */
    unsigned char BS_OEMName[8]; 	/* OEM Name in ASCII */
    unsigned short BPB_BytsPerSec; 	/* Bytes per sector. Allowed values include 512, 1024, 2048, and 4096 */
    unsigned char BPB_SecPerClus; 	/* Sectors per cluster (data unit). Allowed values are powers of 2, but the cluster size must be 32KB or smaller */
    unsigned short BPB_RsvdSecCnt;	/* Size in sectors of the reserved area */
    unsigned char BPB_NumFATs; 		/* Number of FATs */
    unsigned short BPB_RootEntCnt;	/* Maximum number of files in the root directory for FAT12 and FAT16. This is 0 for FAT32 */
    unsigned short BPB_TotSec16; 	/* 16-bit value of number of sectors in file system */
    unsigned char BPB_Media; 		/* Media type */
    unsigned short BPB_FATSz16;		/* 16-bit size in sectors of each FAT for FAT12 and FAT16. For FAT32, this field is 0 */
    unsigned short BPB_SecPerTrk; 	/* Sectors per track of storage device */
    unsigned short BPB_NumHeads;	/* Number of heads in storage device */
    unsigned long BPB_HiddSec;		/* Number of sectors before the start of partition */
    unsigned long BPB_TotSec32;		/* 32-bit value of number of sectors in file system. Either this value or the 16-bit value above must be 0 */
    unsigned long BPB_FATSz32;		/* 32-bit size in sectors of one FAT */
    unsigned short BPB_ExtFlags;	/* A flag for FAT */
    unsigned short BPB_FSVer;		/* The major and minor version number */
    unsigned long BPB_RootClus;		/* Cluster where the root directory can be found */
    unsigned short BPB_FSInfo;		/* Sector where FSINFO structure can be found */
    unsigned short BPB_BkBootSec;	/* Sector where backup copy of boot sector is located */
    unsigned char BPB_Reserved[12];	/* Reserved */
    unsigned char BS_DrvNum;		/* BIOS INT13h drive number */
    unsigned char BS_Reserved1;		/* Not used */
    unsigned char BS_BootSig;		/* Extended boot signature to identify if the next three values are valid */
    unsigned long BS_VolID;			/* Volume serial number */
    unsigned char BS_VolLab[11];	/* Volume label in ASCII. User defines when creating the file system */
    unsigned char BS_FilSysType[8];	/* File system type label in ASCII */
} bootEntryStruct;


typedef struct DirEntry {
    unsigned char DIR_Name[11]; /* File name */
    unsigned char DIR_Attr; /* File attributes */
    unsigned char DIR_NTRes; /* Reserved */
    unsigned char DIR_CrtTimeTenth; /* Created time (tenths of second) */
    unsigned short DIR_CrtTime; /* Created time (hours, minutes, seconds) */
    unsigned short DIR_CrtDate; /* Created day */
    unsigned short DIR_LstAccDate; /* Accessed day */
    unsigned short DIR_FstClusHI; /* High 2 bytes of the first cluster address */
    unsigned short DIR_WrtTime; /* Written time (hours, minutes, seconds */
    unsigned short DIR_WrtDate; /* Written day */
    unsigned short DIR_FstClusLO; /* Low 2 bytes of the first cluster address */
    unsigned long DIR_FileSize; /* File size in bytes. (0 for directories) */
} dirEntryStruct;

typedef struct LFNEntry {
    unsigned char LFN_SeqNum; /* Sequence Number */
    unsigned short LFN_Name1[5]; /* Name characters (five UTF-16 characters) */
    unsigned char LFN_Attr; /* File attributes (always 0x0f) */
    unsigned char LFN_NTRes; /* Reserved */
    unsigned char LFN_ChkSum; /* Checksum of DOS file name */
    unsigned short LFN_Name2[6]; /* Name characters (six UTF-16 characters) */
    unsigned short LFN_FstClus; /* First cluster (always 0x0000) */
    unsigned short LFN_Name3[2]; /* Name characters (two UTF-16 characters) */
} lfnEntryStruct;
#pragma pack(pop)			// BYTE align in memory (no padding)

bootEntryStruct *getBootEntry(int fid);
int read_sectors(int fid, int bps, int sector_number, unsigned char *buffer, int num_sectors);
void read_root(int fid);
void buf_dup(void *buf, int size);
unsigned long lookupFAT(int fid, unsigned long clus);
int strlow(char *str);
#endif



