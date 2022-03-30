#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

typedef unsigned long long int num_t;

volatile num_t count=0;
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

void display(union sigval sigval)
{
	clock_t ticks = clock() - start;
	fprintf(stderr, "\rC=%5.3fk\tT=%5.3fk\t@%5.3f MT/s",
		(float)count/1000.0,
		(float)(ticks)/CLOCKS_PER_SEC,
		(float)(count/ticks));
}

int main(int argc, char **argv)
{
	num_t arg_m, arg_n;
	num_t ret=0;

	if(argc < 3)
	{
		printf("?ARG\n");
		exit(1);
	}

	timer_t timer;
	struct sigevent ev =
	{
		.sigev_notify = SIGEV_THREAD,
		.sigev_notify_function = display,
		.sigev_notify_attributes = NULL
	};
	timer_create(CLOCK_MONOTONIC, &ev, &timer);

	struct itimerspec period =
	{
		.it_value.tv_sec=1,
		.it_interval.tv_sec=0,
		.it_interval.tv_nsec=50000000L
	};

	timer_settime(timer, TIMER_ABSTIME, &period, NULL);

	arg_m = strtoull(argv[1], NULL, 10);
	arg_n = strtoull(argv[2], NULL, 10);

	start = clock();
	ret = ackermann(arg_m, arg_n);
	end = clock();
	printf("\nTotal %0.6lf sec, %llu time(s) called, ACKERMANN(%llu, %llu) = %llu\n",
			(double)(end - start) / 1000000L,
			count, arg_m, arg_n, ret);
	printf("%0.2lf c/usec\n", (double)count / (double)(end - start));
	return 0;
}
