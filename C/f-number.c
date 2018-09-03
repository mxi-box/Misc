#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int rounds=0, passed=0;
	unsigned long long int last=0, current=1, next=0;
	if(argc == 2)
		sscanf(argv[1], "%d", &rounds);
	else
		return 0;

	while(passed <= rounds)
	{
		passed++;
		last=current;
		current=next;
		next=(last + current);
		printf("%d\t%lld\n", passed, current);
	}
}

