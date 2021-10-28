/* Find primes the heuristic way */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void)
{
	typedef unsigned long long int num_t;
	num_t i = 0, j = 1;
	unsigned int p = 0;

	num_t *primes;
	num_t num = 65536;
	primes = malloc(num * sizeof(num_t));

	primes[0] = 2;

next:
	while(j++ < num)
	{
		for(i = 0; primes[i] <= ceil(sqrt(j)) && i < p; i++)
		{
			if(j % primes[i] == 0)	// Is divisible by prime
				goto next; // The almighty goto
		}
		printf("%llu\n", primes[p++] = j);
	}
}
