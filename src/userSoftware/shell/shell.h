#ifndef SHELL_H
#define SHELL_H

#define EOL '\0'

struct KeyUpMap
{
	char key;
	char shift;
};

static int strcmp(char *a, const char *end, char *b)
{
	int i = 0;

	for(; a+i < end && a[i] == b[i] && b[i] != '\0' && b[i] != EOL; i++);
	
	if(b[i] == '\0' || b[i] == EOL)
		return i;
	
	return 0;
}

#endif // SHELL_H_INCLUDED
