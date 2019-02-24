#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

typedef unsigned long long int num_t;

num_t count=0;
clock_t start;
clock_t end=0;

num_t ackermann(num_t m, num_t n)
{
	count++;
	if(m == 0)
		return n+1;
	if(m > 0 && n == 0)
		return ackermann(m - 1, 1);
	if(m > 0 && n > 0)
		return ackermann(m - 1, ackermann(m, n - 1));

	return 0; /* Might be possible to reach here? */
}

void quit(int signal)
{
	fprintf(stderr, "\rC=%llu\n", count);
	fprintf(stderr, "T=%ld\n", clock() - start);
	fputs("==========\n", stderr);
	if(signal == SIGQUIT)
		exit(0);
}

int main(int argc, char **argv)
{
	num_t arg_m, arg_n;
	num_t ret=0;
	start = clock();

	if(argc < 3)
	{
		printf("?ARG\n");
		exit(1);
	}

	signal(SIGINT, quit);
	signal(SIGQUIT, quit);

	arg_m = strtoull(argv[1], NULL, 10);
	arg_n = strtoull(argv[2], NULL, 10);

	ret = ackermann(arg_m, arg_n);
	printf("%ld usec, %llu times called, ACKERMANN(%llu, %llu) = %llu\n",
			(end = clock()) - start,
			count, arg_m, arg_n, ret);
	printf("%lld c/usec\n", count / (end - start));
	return 0;
}
