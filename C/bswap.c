#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char *line;
size_t len=0;
uint8_t *revout;

uint8_t hex2byte(char *str)
{
	char buf[3];
	uint8_t ret;
	strncpy(buf, str, 2);
	sscanf(buf, "%hhx", &ret);
	return ret;
}

int main(int argc, char **argv)
{
	int index=0;
	int bytes=0;

	if(argc < 2)
		exit(1);

	line = argv[1];
	len = strlen(argv[1]);

	if(len % 2)
		exit(1);
	bytes = len / 2;
	revout = malloc((size_t)bytes);

	for(index = 0; index < bytes; index++)
	{
		revout[index] = hex2byte(line + index * 2);
	}
	index--;
	for(; index >= 0; --index)
	{
		if(index == 0)
			printf("%02hhx\n", revout[index]);
		else
			printf("%02hhx", revout[index]);
	}
	return 0;
}
