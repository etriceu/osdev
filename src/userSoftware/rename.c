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
	if(argc != 3)
	{
		print(argv[0]);
		print(" filename newName\n");
		return 0;
	}
	
	struct mount *mnt = getMountPoints();
	if(mnt)
	{
		struct node *node = findFile(mnt, argv[1]);
		if(node)
		{
			renameFile(node, argv[2], strlen(argv[2]));
			saveNodes(mnt);
		}
		else
			print("File not found.\n");
	}
	else
		print("No available mount point\n");
	
	return 0;
}
