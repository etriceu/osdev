#include "../include/screen.h"
#include "../include/myfs.h"

int main(int argc, char** argv)
{
	struct mount *mnt = getMountPoints();
	if(mnt)
	{
		struct node *node = mnt->nodes;
		if(node)
		{
			for(; node; node = node->next)
			{
				print(node->name);
				if(node->next)
					print(", ");
			}
			print("\n");
		}
	}
	
	return 0;
}
