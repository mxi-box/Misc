#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	unsigned long long int num=0, i=0;

	srandom(time(NULL));
	for(i=0; i < (1ULL<<33); i++) num+=random();

	printf("RAND=%#016llx\n", num);
	return 0;
}
