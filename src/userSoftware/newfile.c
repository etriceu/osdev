#include "../include/screen.h"
#include "../include/myfs.h"

size_t strlen(char* str)
{
	size_t n = 0;
	for(; str[n] != '\0' && n < 99999; n++);
	return n;
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		print(argv[0]);
		print(" filename\n");
		return 0;
	}
	
	struct mount *mnt = getMountPoints();
	if(mnt)
	{
		struct node *node = findFile(mnt, argv[1]);
		if(node)
		{
			print("File already exists.\n");
			return 0;
		}
		
		node = newFile(mnt, argv[1], strlen(argv[1]));
		if(node)
			saveNodes(mnt);
		else
			print("Unable to create file\n");
	}
	else
		print("No available mount point\n");
	
	return 0;
}
