#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char **argv)
{
	uint8_t addr8[4];
	uint8_t masklen;
	if(argc < 3)
		exit(1);

	sscanf(argv[1], "%hhu.%hhu.%hhu.%hhu,%hhu",
		&addr8[3], &addr8[2], &addr8[1], &addr8[0], &masklen);
	printf("%hhu.%hhu.%hhu.%hhu\n", addr8[3], addr8[2], addr8[1], addr8[0] + atoi(argv[2]));
}
