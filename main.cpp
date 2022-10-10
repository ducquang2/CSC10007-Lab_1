#include "Header.h"

// Nguon tham khao: https://w...content-available-to-author-only...e.nl/~aeb/linux/fs/fat/fat-1.html

int main()
{
	char name;
	cout << "Enter drive's letter: ";
	cin >> name;

	BYTE SECTOR[512];
	NTFS_BootRecord ntfs;
	FAT32_BootSector fat32;
	BYTE entry[512];
	BYTE rootEntry[32];

	mainEntry mEntry[100];
	int posEntry = 0;
	extraEntry exEntry;

	inforEntry inforEntry[100];
	int pos = 0;
	int size = 0;

	string s = "\\\\.\\";
	s.push_back(name);
	s.push_back(':');

	const size_t cSize = strlen(s.c_str()) + 1;
	wchar_t* path = new wchar_t[cSize];
	mbstowcs(path, s.c_str(), cSize);

	fSectorReading(path, 0, SECTOR, 512);

	string type = ReadtoString(SECTOR, "52", 5);
	if (type == "FAT32")
	{
		memcpy(&fat32, SECTOR, 512);
		size = fPrintFloppyInformation_FAT32(fat32);
	}
	else if (ReadtoString(SECTOR, "03", 4) == "NTFS") {
		memcpy(&ntfs, SECTOR, 512);
		fPrintFloppyInformation_NTFS(ntfs);
	}
	else cout << "NO";

	while (true)
	{
		fSectorReading(path, 0, entry, size);
		BYTE* pEntry = entry;

		for (int i = 0; i < 512 / 32; i++)
		{
			memcpy(&rootEntry, pEntry, 32);

			if (rootEntry[0] == 0x00)
			{
				prinfInforOfEntry(inforEntry, pos);
				printf_TXT_file(mEntry, posEntry);
				return 0;
			}

			if (rootEntry[0] == 0xE5)
			{
				inforEntry[pos].fileStatus = "Deleted";
				inforEntry[pos].fileSize = 0;
				inforEntry[pos].sectorIndex = 0;
				inforEntry[pos].name = "";
				pEntry += 32;
				pos++;
				continue;
			}
			if (rootEntry[11] == 0x0F)
			{
				memcpy(&exEntry, pEntry, 32);
				string part = prinfInforOfExtraEntry(exEntry);
				inforEntry[pos].name = part + inforEntry[pos].name;
			}
			else
			{
				memcpy(&mEntry[posEntry], pEntry, 32);
				inforEntry[pos].fileStatus = "Exist";
				inforEntry[pos].fileSize = fByteReverse(mEntry[posEntry].filesize, 4);
				inforEntry[pos].sectorIndex = size;
				if (inforEntry[pos].name == "")
				{
					inforEntry[pos].name = getStr(mEntry[posEntry]);
					mEntry[pos].fullname = inforEntry[pos].name;
				}
				else
					mEntry[posEntry].fullname = inforEntry[pos].name;
				pos++;
				posEntry++;
			}
			pEntry += 32;
		}
		size += 1;
	}
	return 0;
}