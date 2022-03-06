#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

void writeZero(fstream &f, int size)
{
	for(int n = 0; n < size; n++)
		f.put(0);
}

int main(int argc, char** argv)
{
	if(argc <= 2 || !strcmp(argv[0], "-h") || !strcmp(argv[0], "--help"))
	{
		printf("Usage: %s [KERNEL] [OUTPUT] [FREE_SPACE] [FILES]...\n", argv[0]);
		printf("KERNEL - path to kernel.bin file\n");
		printf("OUTPUT - output file\n");
		printf("FREE_SPACE - number of free sectors at the end\n");
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

	vector <int> nodes;
	size_t n = 0, size = 3;
	char buf[512];
	
	//add kernel
	nodes.push_back(1);
	int gg = 0;
	for(; !file.eof(); n++)
	{
		file.read(buf, 512);
		if(n == 1)
			size = buf[0]-1; //size offset
		
		result.write(buf, file.gcount());
	}
	writeZero(result, 512-file.gcount());
	for(; n < size; n++)
		writeZero(result, 512);
	
	file.close();
	
	//add files
	for(int a = 4; a < argc; a++)
	{
		nodes.push_back(n);
		file.open(argv[a], ios::in | ios::binary);
		if(!file.good())
		{
			printf("Unable to open \"%s\" file.\n", argv[a]);
			return 1;
		}
		file.seekg(0, file.end);
		size_t fsize = file.tellg();
		file.seekg(0, file.beg);
		int s = ceil(fsize/512.0)+1;
		result.write((const char*)&s, 3);
		s = 0;
		result.write((const char*)&s, 3);
		s = fsize%512;
		result.write((const char*)&s, 2);
		s = strlen(argv[a]);
		result.write(argv[a], s);
		writeZero(result, 512-3-3-2-s);

		for(n++; !file.eof(); n++)
		{
			file.read(buf, 512);
			result.write(buf, file.gcount());
		}
		writeZero(result, 512-file.gcount());
		file.close();
	}
	
	//add free space
	for(int n = 0; n < atoi(argv[3]); n++)
		writeZero(result, 512);
	
	//add nodes
	writeZero(result, 512);
	size = 0;
	for(int n : nodes)
	{
		result.write((const char*)&n, 3);
		size += 3;
		if(size == 510)
		{
			writeZero(result, 2);
			size = 0;
		}
	}
	if(size == 510)
		size += 2;
	
	writeZero(result, 512-size);
	
	//save the end of myfs
	result.flush();
	fstream tmp;
	tmp.open(argv[2], ios::in | ios::ate);
	size_t fsize = tmp.tellg()/512-1;
	tmp.close();
	result.seekp(file.beg+512+509);
	result.write((const char*)&fsize, 3);
	result.close();
	
	return 0;
}
