#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <fileapi.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include <iomanip>

#include <cstdint>
#include <vector>

using namespace std;


struct FAT32_BootSector {
	uint8_t bootstrapJump[3];     // 	JumpCode ( 0 - 3 )
	uint8_t oem[8];               // 	OEM name/version (3 - 8)
	uint8_t bytePerSector[2];     // 	Byte per sector (B - 2)
	uint8_t sectorPerCluster;     // 	Sector per cluster (D - 1)
	uint8_t reservedSector[2];    // 	Number of reserved sectors (E - 2)
	uint8_t fatCopy;              // 	Number of FAT copies (10 - 1)
	uint8_t rdetEntry[2];         // 	Number of root directory entries (11 - 2)
	uint8_t sector_of_Volume[2];  //     Total number of sector of Volume (13 - 2)
	uint8_t mediaType;            // 	Media descriptor type (15 - 1 )
	uint8_t sectorPerFAT[2];      // 	Sector per FAT, 0 for FAT32 ( 16 - 2 )
	uint8_t sectorPerTrack[2];    // 	Sector per track ( 18 - 2 )
	uint8_t head[2];              // 	Number of heads ( 1A - 2 )
	uint8_t hiddenSector[4];      // 	Number of hidden sectors ( 1C - 4 )
	uint8_t Total_sector[4];      //     Volume size ( 20 - 4 ) Sv
	uint8_t SectorperFAT[4];      //     Sector per FAT ( 24 - 4 ) Sf
	uint8_t Extended_flag[2];     //     (28-2)
	uint8_t Version[2];           //     Version of FAT32 (2A - 2)
	uint8_t Root_cluster[4];      //     Root Cluster (2C - 4)
	uint8_t System_inf_sector[2]; //     Empty cluster inf (30 - 2)
	uint8_t Backup_boot_sec[2];   //     Backup Boot Sector (32-2)
	uint8_t S6[12];               //     (34 - C )
	uint8_t Physical_drive;       //     Physical drive(40 - 1 )
	uint8_t Reversed;             //     (41 - 1)
	uint8_t Extended_signature;   //     (42 - 1)
	uint8_t Serial[4];            //     (43 - 4)
	uint8_t Volume_label[11];     //     (47 - B)
	uint8_t Fat_name[8];          //     (52 - 8)
	uint8_t BootrapCode[420];     //     (5A - 1A4)
	uint8_t signatrue[2];         //     (1FE - 2)
};

struct NTFS_BootRecord {                                // Field Name - BYTE
	uint8_t jumpCode[3];          // Jump Instruction - 3
	uint8_t OEMID[8];             // OEM ID - 8
	uint8_t Bytes_Sector[2];         // Bytes Per Sector - 2
	uint8_t Sectors_Cluster;      // Sectors Per Cluster - 1
	uint8_t Reserved_Sector[2];      // Reserved Sectors - 2
	uint8_t alway1_0[3];          // always 0 - 3
	uint8_t not_used_by_NTFS1[2];    // not used by NTFS - 2
	uint8_t Media_Descriptor;     // Media Descriptor - 1
	uint8_t alway2_0[2];             // always 0 - 2
	uint8_t sectors_Track[2];        // Sectors Per Track - 2
	uint8_t number_of_Heads[2];      // Number Of Heads - 2
	uint8_t Hidden_sectors[4];      // Hidden Sectors - 4
	uint8_t not_used_by_NTFS2[4];   // not used by NTFS - 4
	uint8_t not_used_by_NTFS3[4];   // not used by NTFS - 4
	long long total_sectors;   // Total Sectors - 8
	long long Logical_MFT;     // Logical Cluster Number for the file $MFT - 8
	long long Logical_MFTMirr; // Logical Cluster Number for the file $MFTMirr - 8
	uint8_t Cluster_FRS[4];         // Clusters Per File Record Segment - 4
	uint8_t Cluster_Index_Buffer; // Clusters Per Index Buffer - 1
	uint8_t not_used_by_NTFS4[3]; // not used by NTFS - 3
	long long Volume;          // Volume Serial Number - 8
	uint8_t checksum[4];            // Checksum - 4
	uint8_t Bootstrap_Code[426];  // Bootstrap_Code - 426
	uint8_t EndofSectorMarker[2];    // EndofSectorMarker - 2
};

typedef struct DirectoryEntry
{
	uint8_t name[8];
	uint8_t fileType[3];
	uint8_t attrib;
	uint8_t userattrib;
	uint8_t Millisecondtime;

	uint16_t createtime;
	uint16_t createdate;
	uint16_t accessdate;
	uint16_t clusterhigh;

	uint16_t modifiedtime;
	uint16_t modifieddate;
	uint16_t clusterlow;
	uint8_t filesize[4];

	string fullname;
} mainEntry;

typedef struct ShortFileName
{
	uint8_t sequenceNo;            // Sequence number, 0xe5 for delete file
	uint8_t fileName_Part1[10];    // file name part - 5 UNICODE 
	uint8_t fileattribute;         // File attibute 

	uint8_t reserved_1;
	uint8_t checksum;

	uint8_t fileName_Part2[12];    // 6 WORD 
	uint8_t NOTHING[2];
	uint8_t fileName_Part3[4];     // 2 WORD NEXT
} extraEntry;

struct inforEntry
{
	string name;
	string fileStatus;
	int fileSize;
	int sectorIndex;
};


uint64_t fByteReverse(uint8_t* byte, unsigned int count);
int fSectorReading(LPCWSTR DRIVE, int readPoint, BYTE* sector, int posSector);

int hexCharToInt(char a);
string hexToString(string str);
string ReadtoString(BYTE* data, string offsetHex, unsigned int bytes);

int fPrintFloppyInformation_FAT32(FAT32_BootSector fat32);
void fPrintFloppyInformation_NTFS(NTFS_BootRecord ntfs);
string getName(extraEntry entry, int n, int type);
string getStr(mainEntry entry);
void prinfInforOfmMainEntry(mainEntry entry);
string prinfInforOfExtraEntry(extraEntry entry);
void prinfInforOfEntry(inforEntry entry[100], int n);
void printfTXT(string a);
void printf_TXT_file(mainEntry entry[100], int n);
