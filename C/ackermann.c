#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

typedef unsigned long long int num_t;

num_t ackermann(num_t m, num_t n)
{
	if(m == 0)
		return n+1;
	if(m > 0 && n == 0)
		return ackermann(m - 1, 1);
	if(m > 0 && n > 0)
		return ackermann(m - 1, ackermann(m, n - 1));

	return 0; /* Might be possible to reach here? */
}

int main(int argc, char **argv)
{
	num_t arg_m, arg_n;

	if(argc < 3)
	{
		printf("?ARG\n");
		exit(1);
	}

	arg_m = strtoull(argv[1], NULL, 10);
	arg_n = strtoull(argv[2], NULL, 10);

	printf("ACKERMANN(%llu, %llu) = %llu\n", arg_m, arg_n, ackermann(arg_m, arg_n));

	return 0;
}
