#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/* Big Coredump Generating Program
 * Neo_Chen <chenkolei@gmail.com>
 * Released to Public Domain
 */

int main(void)
{
	unsigned long long int size=0, i=0;
	puts("SIZE? (IN MiB)");
	scanf("%lld", &size);
	puts("ALLOCATE BEGIN\n");
	unsigned char *p = malloc(size*1024*1024);
	for(i=0; i < ( size * 1024 * 1024 ); ++i)
		p[i]=0x00;
	puts("RAISEING A SIGABRT\n");
	raise(SIGABRT);
	return 8;
}
