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
		print(" fileName newFileName\n");
		return 0;
	}
	
	struct mount *mnt = getMountPoints();
	if(mnt)
	{
		struct node *srcNode = findFile(mnt, argv[1]);
		struct node *dscNode = findFile(mnt, argv[2]);
		
		if(dscNode)
		{
			print("File already exists.\n");
			return 0;
		}

		if(srcNode)
		{
			dscNode = newFile(mnt, argv[2], strlen(argv[2]));
			if(!dscNode)
			{
				print("Unable to create file\n");
				return 0;
			}
			
			struct file *src = fopen(srcNode);
			struct file *dsc = fopen(dscNode);
			
			char c;
			do
			{
				if(fread(src, 1, (uint8_t*)&c))
				{
					fclose(src);
					fclose(dsc);
					saveNodes(mnt);
					return 0;
				}
				if(fwrite(dsc, 1, (uint8_t*)&c))
				{
					fclose(src);
					fclose(dsc);
					removeFile(dscNode);
					saveNodes(mnt);
					print("Not enough free space.\n");
					return 0;
				}
			} while(1);
		}
		else
			print("File not found.\n");
	}
	else
		print("No available mount point\n");
	
	return 0;
}
