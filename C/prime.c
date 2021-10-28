/* Find primes the heuristic way */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void)
{
	typedef unsigned long long int num_t;
	num_t i = 0, j = 1;
	unsigned int p = 0;
	double sqrj;

	num_t *primes;
	num_t num = 65536;
	primes = calloc(num, sizeof(num_t));

	primes[0] = 2;
	p = 1;

next:
	while(j++ < num)
	{
		sqrj = sqrt(j);
		for(i = 0; i < p && primes[i] < sqrj; i++)
		{
			if(j % primes[i] == 0)
				goto next;
		}
		for(i = primes[i - 1]; i <= sqrj; i++)
		{
			if(j % i == 0)
				goto next;
		}
		printf("%llu\n", primes[p++] = j);
	}
}
