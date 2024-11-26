#include "../include/screen.h"
#include "../include/myfs.h"

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
			removeFile(node);
			saveNodes(mnt);
		}
		else
			print("File not found.\n");
	}
	else
		print("No available mount point\n");
	
	return 0;
}
