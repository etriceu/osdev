#include "../include/systemCall.h"

struct
{
	char key;
	char shift;
} keyMap[] = {
	{'`', '~'}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'},
	{'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'}, {'-', '_'},
	{'=', '+'}, {'[', '{'}, {']', '}'}, {';', ':'}, {'\'', '"'}, {'\\', '|'},
	{',', '<'}, {'.', '>'}, {'/', '?'}};

void printNum(int num)
{
	if(num < 0)
	{
		print("-");
		num = -num;
	}
	
	if(num/10 != 0)
		printNum(num/10);
	
	char c = '0'+num%10;
	printn(&c, 1);
}

int strlen(char *str)
{
	int n = 0;
	for(; n < VIDEO_WIDTH && str[n] != '\0'; n++);
	return n;
}

void newLine(char ***lines, uint32_t *size, volatile int row)
{
	*size += 1;
	char **tmp = malloc(*size*sizeof(char*));
	for(int n = 0; n < row; n++)
		tmp[n] = (*lines)[n];
	
	tmp[row] = malloc(VIDEO_WIDTH);
	
	for(int n = row+1; n < *size; n++)
		tmp[n] = (*lines)[n-1];
	
	if(*lines != NULL)
		free(*lines);
	*lines = tmp;
}

void delLine(char ***lines, uint32_t *size, uint32_t row)
{
	if(*size == 0)
		return;
	
	*size -= 1;
	char **tmp = malloc(*size*sizeof(char*));
	
	for(int n = 0; n < row; n++)
		tmp[n] = (*lines)[n];
	
	for(int n = row; n < *size; n++)
		tmp[n] = (*lines)[n+1];
	
	free((*lines)[row]);
	free(*lines);
	*lines = tmp;
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		print(argv[0]);
		print(" [filename]\n");
		return 1;
	}

	struct mount *mnt = getMountPoints();
	if(mnt == NULL)
	{
		print("No available mount point\n");
		return 1;
	}

	struct node *nod = findFile(mnt, argv[1]);
	if(nod == NULL)
	{
		print("File not found.\n");
		return 1;
	}
	
	char **lines = NULL;
	uint32_t num = 0;
	int col = 0, row = 0, offset = 0;
	
	uint64_t size;
	struct file* file = fopen(nod, &size, MYFS_READ);
	char *buf = malloc(size);
	fread(file, size, buf);
	fclose(file);

	newLine(&lines, &num, 0);

	for(uint32_t a = 0, b = 0, c = 0; a < size; a++, c++)
		if(c >= VIDEO_WIDTH-1 || buf[a] == '\n')
		{
			c = -1;
			b++;
			newLine(&lines, &num, num);
		}
		else
			lines[b][c] = buf[a];

	free(buf);
	uint8_t quit = 0, refresh = 1;
	
	while(!quit)
	{
		for(uint8_t key = pullKeys(); key != KEY_NONE; key = pullKeys())
		{
			if(key < KEY_SPECIAL)
				key = keys[key];
			
			if(getKeyStatus(keyID(KEY_LCTRL)))
			{
				if(key == 's')
				{
					removeFile(nod);
					nod = newFile(mnt, argv[1]);
					file = fopen(nod, NULL, MYFS_WRITE);
					
					for(int y = 0; y < num; y++)
					{
						for(int x = 0; x < VIDEO_WIDTH && lines[y][x] != '\0'; x++)
							fwrite(file, 1, &lines[y][x]);
						fwrite(file, 1, "\n");
					}
					
					fclose(file);
				}
				else if(key == 'x')
				{
					quit = 1;
					break;
				}
			}
			else
			{
				if(key == KEY_ENTER)
					key = '\0';
				
				if((key == KEY_LEFT || key == KEY_BACKSPACE) &&
					(row > 0 || (row == 0 && col > 0)))
				{
					col--;
					if(col < 0 && row > 0)
					{
						if(key == KEY_BACKSPACE)
							delLine(&lines, &num, row);

						row--;
						col = strlen(lines[row]);
					}
					else if(key == KEY_BACKSPACE)
					{
						int n = col+1;
						for(; n < VIDEO_WIDTH-1 && lines[row][n] != '\0'; n++)
							lines[row][n-1] = lines[row][n];
						
						lines[row][n-1] = '\0';
					}
				}
				else if(key == KEY_RIGHT && (row < num-1 ||
					(row == num-1 && col < VIDEO_WIDTH-1 && lines[row][col] != '\0')))
				{
					col++;
					if((col >= VIDEO_WIDTH || lines[row][col-1] == '\0') && row < num-1)
					{
						col = 0;
						row++;
					}
				}
				else if(key == KEY_UP && row > 0)
				{
					col = 0;
					row--;
				}
				else if(key == KEY_DOWN && row < num-1)
				{
					col = 0;
					row++;
				}
				else if(key <= '~')
				{
					if(getKeyStatus(keyID(KEY_LSHIFT)) ||
						getKeyStatus(keyID(KEY_RSHIFT)))
					{
						if(key >= 'a' && key <= 'z')
							key -= 32;
						else
							for(int n = 0; n < sizeof(keyMap)/sizeof(keyMap[0]); n++)
								if(keyMap[n].key == key)
								{
									key = keyMap[n].shift;
									break;
								}
					}
					
					for(int n = VIDEO_WIDTH-1; n > col; n--)
						lines[row][n] = lines[row][n-1];
					
					lines[row][col] = key;
					col++;
					if(key == '\0' || col >= VIDEO_WIDTH-1)
					{
						row++;
						newLine(&lines, &num, row);
						for(int n = 0; n < VIDEO_WIDTH-col; n++)
						{
							lines[row][n] = lines[row-1][n+col];
							lines[row-1][n+col] = '\0';
						}
							
						col = 0;
					}
				}
				
				if(row > offset+VIDEO_HEIGHT-3) offset++;
				else if(row < offset) offset--;
				
				refresh = 1;
			}
		}
		if(refresh)
		{
			clear();
			print(argv[1]);
			print("; "); printNum(row); print("; "); printNum(col);
			print("; Ctrl + s to save; Ctrl + x to exit;\n");
			for(uint32_t n = offset; n < num && n < offset+VIDEO_HEIGHT-2; n++)
				printn(lines[n], VIDEO_WIDTH);
			
			refresh = 0;
			moveCursor(VIDEO_WIDTH*(row+1-offset)+col);
		}
		sleep(10);
	}
	
	clear();
	for(int n = 0; n < num; n++)
		free(lines[n]);
	
	free(lines);
	return 0;
}
