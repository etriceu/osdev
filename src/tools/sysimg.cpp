#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include <cstdint>

#include "../include/myfsCommon.h"

using namespace std;

typedef uint32_t osSize_t;

constexpr int SECTOR_SIZE = MY_FS_SECTOR_SIZE;
constexpr int BOOTSECTOR_SIZE = SECTOR_SIZE*2;

constexpr uint8_t KERNEL_SECTOR = 3;
constexpr uint8_t KERNEL_CYLINDER = 0;
constexpr uint8_t KERNEL_HEAD = 0;

constexpr int INFO_SECTOR_SIZE = 4*sizeof(uint8_t)+2*sizeof(uint32_t);

struct FileInfo
{
	string fileName;
	size_t fileSize;
};

void writeZero(fstream &f, int size)
{
	for(int n = 0; n < size; n++)
		f.put(0);
}

void calcBlockInfo(uint32_t &blockSize, uint16_t &lastSectorSize)
{
	if(blockSize <= SECTOR_SIZE-MY_FS_BLOCK_HEADER_SIZE)
	{
		lastSectorSize = blockSize;
		blockSize = 1;
	}
	else
	{
		blockSize -= SECTOR_SIZE-MY_FS_BLOCK_HEADER_SIZE;
		lastSectorSize = blockSize%SECTOR_SIZE;
		blockSize /= SECTOR_SIZE;
		blockSize++;
		if(lastSectorSize != 0)
			blockSize++;
	}
}

int main(int argc, char** argv)
{
	if(argc <= 2 || !strcmp(argv[0], "-h") || !strcmp(argv[0], "--help"))
	{
		printf("Usage: %s [KERNEL] [OUTPUT] [DISK_SIZE] [FILES]...\n", argv[0]);
		printf("KERNEL - path to kernel.bin file\n");
		printf("OUTPUT - output file\n");
		printf("DISK_SIZE - number of sectors\n");
		printf("FILES - additional files\n");
		return 0;
	}
	
	fstream file, result;
	
	file.open(argv[1], ios::in | ios::binary);
	result.open(argv[2], ios::out | ios::binary);
	
	if(!file.good())
	{
		printf("Unable to open \"%s\" file.\n", argv[1]);
		return 1;
	}
	
	if(!result.good())
	{
		printf("Unable to open \"%s\" file.\n", argv[2]);
		return 1;
	}
	
	uint32_t diskSize = atoi(argv[3]);
	
	file.seekg(0, file.end);
	size_t kernelSize = (size_t)file.tellg()-BOOTSECTOR_SIZE;
	file.seekg(0, file.beg);
	
	char buf[SECTOR_SIZE];
	
	//write bootsector
	file.read(buf, SECTOR_SIZE);
	result.write(buf, SECTOR_SIZE);
	
	//write kernel info & first partition info
	uint8_t sizeInSectors = kernelSize/SECTOR_SIZE + (kernelSize%SECTOR_SIZE ? 1 : 0);
	printf("Kernel size: %d sectors\n", sizeInSectors);
	result.write((const char*)&sizeInSectors, sizeof(sizeInSectors));
	result.write((const char*)&KERNEL_SECTOR, sizeof(KERNEL_SECTOR));
	result.write((const char*)&KERNEL_CYLINDER, sizeof(KERNEL_CYLINDER));
	result.write((const char*)&KERNEL_HEAD, sizeof(KERNEL_HEAD));
	
	uint32_t myfsStart = sizeInSectors+BOOTSECTOR_SIZE/SECTOR_SIZE;
	uint32_t myfsEnd = diskSize-1;
	result.write((const char*)&myfsStart, sizeof(myfsStart));
	result.write((const char*)&myfsEnd, sizeof(myfsEnd));
	writeZero(result, SECTOR_SIZE-INFO_SECTOR_SIZE);
	
	//write kernel
	file.seekg(BOOTSECTOR_SIZE);
	do {
		file.read(buf, SECTOR_SIZE);
		result.write(buf, file.gcount());
	} while(!file.eof());
	writeZero(result, (SECTOR_SIZE-(result.tellg()%SECTOR_SIZE))%SECTOR_SIZE);
	file.close();
	
	//load files
	uint32_t blockSize = MY_FS_NODE_SIZE;
	uint16_t lastSectorSize = 0;
	vector<FileInfo> files;
	for(int n = 4; n < argc; n++)
	{
		file.open(argv[n], ios::in | ios::ate);
		if(result.good())
		{
			size_t size = file.tellg();
			file.close();
			files.push_back({argv[n], size});
			blockSize += MY_FS_NODE_SIZE;
			blockSize += files.back().fileName.size();
		}
		else
			printf("Unable to open \"%s\" file.\n", argv[n]);
	}
	
	calcBlockInfo(blockSize, lastSectorSize);

	//write node table
	uint32_t firstBlock = myfsStart;
	result.write((const char*)&blockSize, sizeof(blockSize));
	writeZero(result, sizeof(uint32_t));
	result.write((const char*)&lastSectorSize, sizeof(lastSectorSize));
	result.write((const char*)&firstBlock, sizeof(firstBlock));
	writeZero(result, sizeof(osSize_t));
	firstBlock += blockSize;
	for(FileInfo f : files)
	{
		osSize_t nameSize = f.fileName.size();
		blockSize = f.fileSize;
		calcBlockInfo(blockSize, lastSectorSize);
		result.write((const char*)&lastSectorSize, sizeof(lastSectorSize));
		result.write((const char*)&firstBlock, sizeof(firstBlock));
		result.write((const char*)&nameSize, sizeof(nameSize));
		result.write(f.fileName.c_str(), nameSize);
		firstBlock += blockSize;
	}
	writeZero(result, (SECTOR_SIZE-(result.tellg()%SECTOR_SIZE))%SECTOR_SIZE);
	
	//write files
	for(FileInfo f : files)
	{
		file.open(f.fileName, ios::in | ios::binary);
		blockSize = f.fileSize;
		calcBlockInfo(blockSize, lastSectorSize);
		result.write((const char*)&blockSize, sizeof(blockSize));
		writeZero(result, sizeof(uint32_t));
		do {
			file.read(buf, SECTOR_SIZE);
			result.write(buf, file.gcount());
		} while(!file.eof());
		writeZero(result, (SECTOR_SIZE-(result.tellg()%SECTOR_SIZE))%SECTOR_SIZE);
		file.close();
	}
	
	//resize disk
	if(result.tellg()/SECTOR_SIZE < diskSize)
		writeZero(result, (diskSize-result.tellg()/SECTOR_SIZE)*SECTOR_SIZE);
	else
		printf("Warning: disk size is too small\n");
	
	result.close();
	
	return 0;
}
