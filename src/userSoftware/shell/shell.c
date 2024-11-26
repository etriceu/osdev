#include <stdint.h>
#include "shell.h"
#include "funcs.h"
#include "../../include/myfs.h"
#include "../../include/malloc.h"
#include "../../include/screen.h"
#include "../../include/keyboard.h"
#include "../../include/timer.h"

const struct KeyUpMap keyMap[] = {
	{'`', '~'}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'},
	{'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'}, {'-', '_'},
	{'=', '+'}, {'[', '{'}, {']', '}'}, {';', ':'}, {'\'', '"'}, {'\\', '|'},
	{',', '<'}, {'.', '>'}, {'/', '?'}
};

void num2str(size_t num, char *out)
{
	const int DIGITS = sizeof(size_t) > 4 ? 20 : 10;
	int n = 0;
	do
	{
		out[n] = '0' + num%10;
		num /= 10;
	} while(n++ < DIGITS && num > 0);
	out[n] = '\0';
	n--;
	
	for(int m = 0; m < n; m++, n--)
	{
		char c = out[n];
		out[n] = out[m];
		out[m] = c;
	}
}

void parseStrings(char **code, size_t *size, char ***strings, size_t *strCount)
{
	uint8_t isQuota = 0;
	
	size_t newSize = 0;
	char *newCode = NULL;
	
	size_t strSize;
	char *string = NULL;
	for(size_t n = 0; n < *size; n++)
	{
		if((*code)[n] == '"')
		{
			isQuota = !isQuota;
			strSize = 0;
			if(!isQuota)
			{
				(*strCount)++;
				*strings = (char**)realloc(*strings, (*strCount)*sizeof(char*));
				(*strings)[(*strCount)-1] = string;
				char num[21];
				num2str((size_t)string, num);
				for(int i = 0; i < 21 && num[i] != '\0'; i++)
				{
					newCode = (char*)realloc(newCode, ++newSize);
					newCode[newSize-1] = num[i];
				}
				string = NULL;
			}
		}
		else if((*code)[n] == '\\' && isQuota && ++n < *size)
		{
			string = (char*)realloc(string, ++strSize);
			
			switch((*code)[n])
			{
				case '0': string[strSize-1] = '\0'; break;
				case 'n': string[strSize-1] = '\n'; break;
				case 't': string[strSize-1] = '\t'; break;
				default: string[strSize-1] = (*code)[n];
			}
		}
		else if(isQuota)
		{
			string = (char*)realloc(string, ++strSize);
			string[strSize-1] = (*code)[n];
		}
		else
		{
			newCode = (char*)realloc(newCode, ++newSize);
			newCode[newSize-1] = (*code)[n];
		}
	}

	free(*code);
	*size = newSize;
	*code = newCode;
}

void parseArgs(char **code, size_t *size, int argc, char **argv)
{
	size_t newSize = 0;
	char *newCode = NULL;
	
	for(size_t n = 0; n < *size; n++)
	{
		if((*code)[n] == '@' || (*code)[n] == '$')
		{
			char num[21];
			if((*code)[n] == '@')
				num2str(argc, num);
			else
				num2str((size_t) argv, num);
			for(int i = 0; i < 21 && num[i] != '\0'; i++)
			{
				newCode = (char*)realloc(newCode, ++newSize);
				newCode[newSize-1] = num[i];
			}
		}
		else
		{
			newCode = (char*)realloc(newCode, ++newSize);
			newCode[newSize-1] = (*code)[n];
		}
	}

	free(*code);
	*size = newSize;
	*code = newCode;
}

int getInstructionID(char **code, const char *end)
{
	for(size_t n = 1; n < FUNC_COUNT; n++)
	{
		int offset = strcmp(*code, end, (char*)NAMES[n]);
		if(offset)
		{
			*code += offset+1;
			return n;
		}
	}
	return 0; //nop
}

void execute(char *code, size_t size)
{
	const char *end = code+size;
	const char *start = code;
	
	for(; code < end; code++)
	{
		if(*code == ':' || *code == '#')
		{
			for(; code < end && *code != EOL; code++);
			continue;
		}
		else if(*code <= ' ' || *code > '~')
			continue;
		else
		{
			int inst = getInstructionID(&code, end);
			FUNCS[inst](&code, end, start);
		}
	}
}

void clearLine(char *line)
{
	for(size_t n = 0; n < VIDEO_WIDTH; n++)
		line[n] = '\0';
}

int main(int argc, char **argv)
{
	size_t strCount = 0;
	char **strings = NULL;
	
	uint8_t isScriptFile = 0;
	if(argc == 2)
	{
		struct mount *mnt = getMountPoints();
		if(mnt)
		{
			struct node *node = findFile(mnt, argv[1]);
			if(node)
			{
				struct file *file = fopen(node);
				fseek(file, -1);
				size_t size = ftell(file);
				fseek(file, 0);
				char *code = malloc(size);
				fread(file, size, (uint8_t*)code);
				fclose(file);
				isScriptFile = 1;
				
				parseStrings(&code, &size, &strings, &strCount);
				parseArgs(&code, &size, argc-1, argv+1);
				execute(code, size);
			}
			else
			{
				print("File '");
				print(argv[1]);
				print("' not found.\n");
			}
		}
		else
			print("No mount points.\n");
	}
	
	if(!isScriptFile)
	{
		char line[VIDEO_WIDTH];
		uint8_t col = 0;
		uint8_t refresh = 1;
		uint8_t quit = 0;
		
		clearLine(line);
		while(!quit)
		{
			for(uint8_t key = pollKeys(); key != KEY_NONE; key = pollKeys())
			{
				if(key < KEY_SPECIAL)
					key = KEYS[key];
				
				if(getKeyStatus(keyID(KEY_LCTRL)))
				{
					if(key == 'x')
					{
						quit = 1;
						break;
					}
				}
				else
				{
					if(key == KEY_ENTER)
					{
						print("\n");
						col = 0;
						
						size_t size = VIDEO_WIDTH;
						char *code = malloc(size);
						for(size_t n = 0; n < size; n++)
							code[n] = line[n];
						
						parseStrings(&code, &size, &strings, &strCount);
						execute(code, size);
						
						print("\n");
						
						free(code);
						clearLine(line);
						refresh = 1;
						continue;
					}
					
					if((key == KEY_LEFT || key == KEY_BACKSPACE) && col > 0)
					{
						col--;
						if(key == KEY_BACKSPACE)
						{
							int n = col+1;
							for(; n < VIDEO_WIDTH-1 && line[n] != '\0'; n++)
								line[n-1] = line[n];
							
							line[n-1] = '\0';
						}
					}
					else if(key == KEY_RIGHT && col < VIDEO_WIDTH-1 && line[col] != '\0')
						col++;
					else if(key <= '~' && col < VIDEO_WIDTH-1)
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
							line[n] = line[n-1];
						
						line[col] = key;
						col++;
					}
					
					refresh = 1;
				}
			}
			if(refresh)
			{
				moveCursor(VIDEO_WIDTH*(VIDEO_HEIGHT-1));
				print(">");
				printn(line, VIDEO_WIDTH-1);
				moveCursor(VIDEO_WIDTH*(VIDEO_HEIGHT-1)+col+1);
				refresh = 0;
			}
			sleep(10);
		}
	}
	
	for(size_t n = 0; n < strCount; n++)
		free(strings[n]);
	
	free(strings);
}
