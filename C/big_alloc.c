#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Memory Eater                                        *
 * Neo_Chen <chenkolei@gmail.com>                      *
 * Released to Public Domain                           *
 * Should work on any Platform with 64Bit long long int*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned long long int size=0, i=0, pattern=0x0000000000000000;

int main(void)
{
	puts("SIZE? (IN MiB)");
	scanf("%lld", &size);
	puts("PATTERN? (64Bit, Hex)");
	scanf("%llx", &pattern);
	puts("ALLOCATE BEGIN\n");
	unsigned long long int *p = malloc(size*1024*1024);
	for(i=0; i < ( size * 1024 * 1024 / 8 ); ++i)
		p[i]=pattern;
	puts("INPUT ANY NUMBER");
	scanf("%lld", &i);
	FILE *output=fopen("/dev/null", "w");
	fprintf(output, "%llx %p", i, p);
	return 0;
}
