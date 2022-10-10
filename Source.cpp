#pragma warning(disable:4996)

#include "Header.h"
#include <sstream>
#include <sstream>
#include <iomanip>

int fSectorReading(LPCWSTR DRIVE, int readPoint, BYTE* sector, int posSector)
{
	int retCode = 0;
	DWORD bytesRead;
	HANDLE device = NULL;

	device = CreateFile(DRIVE,    // Drive to open
		GENERIC_READ,           // Access mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
		NULL,                   // Security Descriptor
		OPEN_EXISTING,          // How to create
		0,                      // File attributes
		NULL);                  // Handle to template

	if (device == INVALID_HANDLE_VALUE) // Open Error
	{
		printf("CreateFile: %u\n", GetLastError());
		return 1;
	}

	SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

	if (!ReadFile(device, sector, posSector, &bytesRead, NULL))
	{
		printf("ReadFile: %u\n", GetLastError());
	}
	return 0;
}

uint64_t fByteReverse(uint8_t* byte, unsigned int count)
{
	uint64_t reverse = 0;
	for (int i = count - 1; i >= 0; i--) {
		reverse = (reverse << 8) | byte[i];
	}
	return reverse;
}

int hexCharToInt(char a)
{
	if (a >= '0' && a <= '9')
		return(a - 48);
	if (a >= 'A' && a <= 'Z')
		return(a - 55);
	return(a - 87);
}

string hexToString(string str)
{
	stringstream HexString;
	for (int i = 0; i < str.length(); i++) {
		char a = str.at(i++);
		char b = str.at(i);
		int x = hexCharToInt(a);
		int y = hexCharToInt(b);
		HexString << (char)((16 * x) + y);
	}
	return HexString.str();
}

string ReadtoString(BYTE* data, string offsetHex, unsigned int bytes)
{
	const int offset = stoi(offsetHex, nullptr, 16);

	int len = offset + bytes;
	stringstream ss;
	ss << hex;
	for (int i = offset; i < len; i++)
	{
		ss << setw(2) << setfill('0') << (int)data[i];
		ss << " ";
	}
	string result;
	while (!ss.eof())
	{
		string temp;
		ss >> temp;
		result += hexToString(temp);
	}
	return result;
}


int fPrintFloppyInformation_FAT32(FAT32_BootSector fat32)
{
	cout << "FAT: " << fat32.Fat_name << endl;
	cout << "Bytes per sector: " << fByteReverse(fat32.bytePerSector, 2) << endl;
	cout << "Sector per cluster: " << fat32.sectorPerCluster << endl;
	cout << "Reserved sectors (Sb): " << fByteReverse(fat32.reservedSector, 2) << endl;
	cout << "FAT copies: " << fat32.fatCopy << endl;
	cout << "Total sector: " << fByteReverse(fat32.Total_sector, 4) << endl;
	cout << "Sector per FAT: " << fByteReverse(fat32.SectorperFAT, 4) << endl;
	cout << "Root directory Entries: " << fByteReverse(fat32.rdetEntry, 2) << endl;
	cout << "First sector of FAT1: " << fByteReverse(fat32.reservedSector, 2) << endl;

	int s1_rdet = int(fByteReverse(fat32.reservedSector, 2))
		+ int(fByteReverse(fat32.SectorperFAT, 4)) * int(fat32.fatCopy);
	cout << "First sector of RDET: " << s1_rdet << endl;

	int d1_data = s1_rdet + int(fByteReverse(fat32.rdetEntry, 2) * 32 / 512);
	cout << "First sector of DATA: " << d1_data << endl;

	return s1_rdet;
}

void fPrintFloppyInformation_NTFS(NTFS_BootRecord ntfs)
{
	cout << "Floppy Disk Information: " << endl;
	cout << "-------------------------------------------" << endl;
	cout << "FAT: " << ntfs.OEMID << endl;
	cout << "Bytes per sector: " << fByteReverse(ntfs.Bytes_Sector, 2) << endl;
	cout << "Sector per cluster: " << ntfs.Sectors_Cluster << endl;
	cout << "Reserved Sector: " << fByteReverse(ntfs.Reserved_Sector, 2) << endl;
	cout << "Total Sectors: " << ntfs.total_sectors << endl;
	cout << "Logical Cluster Number for the file $MFT: " << ntfs.Logical_MFT << endl;
	cout << "Logical Cluster Number for the file $MFTMirr: " << ntfs.Logical_MFTMirr << endl;
}


string getName(extraEntry entry, int n, int type)
{
	string name;
	for (int i = 0; i < n; i += 2)
	{
		if (type == 1)
			name += entry.fileName_Part1[i];
		if (type == 2)
			name += entry.fileName_Part2[i];
		if (type == 3)
			name += entry.fileName_Part3[i];
	}
	return name;
}

string getStr(mainEntry entry)
{
	string name;
	for (int i = 0; i < 8; i += 1)
		if (int(entry.name) != 32)
			name += entry.name[i];
	for (int i = 0; i < 3; i += 1)
		name += entry.fileType[i];
	return name;
}

void prinfInforOfmMainEntry(mainEntry entry)
{
	cout << "hello";
	cout << "file name: " << entry.fullname << endl;
	if (entry.attrib & 0x01)
		printf("File Attribute       : Read Only File\n");
	if (entry.attrib & 0x02)
		printf("File Attribute    : Hidden File\n");
	if (entry.attrib & 0x04)
		printf("File Attribute    : System File\n");
	if (entry.attrib & 0x08)
		printf("File Attribute    : Volume Label\n");
	if (entry.attrib & 0x0f)
		printf("File Attribute    : Long File Name\n");
	if (entry.attrib & 0x10)
		printf("File Attribute    : Directory\n");
	if (entry.attrib & 0x20)
		printf("File Attribute    : Archive\n");

	WORD nYear = (entry.createdate >> 9);
	WORD nMonth = (entry.createdate << 7);
	nMonth = nMonth >> 12;
	WORD nDay = (entry.createdate << 11);
	nDay = nDay >> 11;

	printf("Create Date    : %d/%d/%d\n", nDay, nMonth, (nYear + 1980));

	nYear = (entry.modifieddate >> 9);
	nMonth = (entry.modifieddate << 7);
	nMonth = nMonth >> 12;
	nDay = (entry.modifieddate << 11);
	nDay = nDay >> 11;

	printf("Modification Date    : %d/%d/%d\n", nDay, nMonth, (nYear + 1980));

	nYear = (entry.accessdate >> 9);
	nMonth = (entry.accessdate << 7);
	nMonth = nMonth >> 12;
	nDay = (entry.accessdate << 11);
	nDay = nDay >> 11;

	printf("Accessed Date        : %d/%d/%d\n", nDay, nMonth, (nYear + 1980));

	printf("Start Cluster Address: %d\n", entry.clusterlow);
	printf("File Size            : %d bytes\n", fByteReverse(entry.filesize, 4));
}

string prinfInforOfExtraEntry(extraEntry entry)
{
	//printf("File Attribute    : Long File Name\n");
	//printf("sequenceNo: %d\n", entry.sequenceNo);

	string full;
	string part;

	part = getName(entry, 10, 1);
	//cout << "fileName_Part1: " << part << endl;
	full += part;

	part = getName(entry, 12, 2);
	//cout << "fileName_Part2: " << part << endl;
	full += part;

	part = getName(entry, 4, 3);
	//cout << "fileName_Part3: " << part << endl;
	full += part;

	//cout << "FULL: " << full << endl;
	//cout << endl << endl;
	return full;
}

void prinfInforOfEntry(inforEntry entry[100], int n)
{
	for (int i = 0; i < n; i++)
	{
		cout << "/////////   INFOR OF ENTRY   /////////" << endl;
		cout << "name: " << entry[i].name << endl;
		cout << "file status: " << entry[i].fileStatus << endl;
		cout << "file Size: " << entry[i].fileSize << endl;
		cout << "sector Index: " << entry[i].sectorIndex << endl;

		system("pause");
		system("cls");
	}
}

void printfTXT(string a)
{
	cout << endl << "INFOR: " << endl;
	ifstream ifs;
	ifs.open(a);
	if (ifs.fail()) {
		cout << "cant open file" << endl;
		ifs.close();
		return;
	}

	string docfile = "", dong = "";
	while (!ifs.eof()) {
		getline(ifs, dong);
		docfile += dong + "\n";
	}
	cout << docfile << endl;

	ifs.close();
}

void printf_TXT_file(mainEntry entry[100], int n)
{
	string a;
	for (int i = 0; i < n; i++)
	{
		cout << "//////   INFOR OF TXT FILE    //////" << endl;
		a = "";
		for (int j = 0; j < 3; j++)
			a += entry[i].fileType[j];
		if (a == "TXT")
		{
			prinfInforOfmMainEntry(entry[i]);
			string a = "H:/" + entry[i].fullname;
			printfTXT(a);
		}
		else
		{
			if (entry[i].name[0] == 0xE5)
				cout << "GOT DELETED" << endl;
			else
				cout << "file name: " << entry[i].fullname << endl;
			cout << "use another program to read that file" << endl;
		}
		system("pause");
		system("cls");
	}
}