/* Find primes the heuristic way */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INITIAL_SIZE	65536
#define ALLOC_STEP	16384
#define ALLOC_THRESHOLD	1024

#define NULLCHK(x)	\
	if((x) == NULL)			\
	{				\
		puts("Out of memory");	\
		exit(1);		\
	}

int main(int argc, char **argv)
{
	typedef unsigned long long int num_t;
	num_t i = 0, j = 1;
	size_t p = 0;
	size_t size = INITIAL_SIZE;

	num_t *primes;
	num_t num = 1;
	primes = calloc(size, sizeof(num_t));
	NULLCHK(primes);

	primes[0] = 2;

	if(argc < 2)
	{
		puts("No enough arguments");
		exit(1);
	}

	num = strtoull(argv[1], NULL, 10);

next:
	while(j++ < num)
	{
		for(i = 0; primes[i] <= ceil(sqrt(j)) && i < p; i++)
		{
			if(j % primes[i] == 0)	// Is divisible by prime
				goto next; // The almighty goto
		}

		if(size - p < ALLOC_THRESHOLD)
		{
			primes = realloc(primes, (size += ALLOC_STEP) * sizeof(num_t));
			NULLCHK(primes);
		}

		printf("%llu\n", primes[p++] = j);
	}

	return 0;
}
