/* OpenMP program to generate or crack the random seed of a random file */

#include <bits/types/timer_t.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <omp.h>

static inline int match(unsigned int seed, uint8_t *bin, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		if (rand_r(&seed) % 256 != bin[i])
			return 0;
	}
	return len;
}

static volatile unsigned long long ctr = 0;
static volatile unsigned long long last = 0;
static volatile unsigned long long secs = 0;

void display(union sigval sigval)
{
	// silence the compiler
	(void)sigval;

	secs += 1;
	fprintf(stderr, ">%7.3f%% (%llu/%u) @ %.5f Mop/s\n",
	ctr / (double)UINT_MAX * 100, ctr, UINT_MAX, ((double)(ctr - last) / 1000.0 / 1000.0));
	
	last = ctr;
}

int main(int argc, char *argv[])
{
	int p = omp_get_max_threads();
	uint8_t *bin = NULL;
	FILE *fp;

	if(argc < 2)
	{
		printf("Usage: %s <file> [seed]\n", argv[0]);
		return 1;
	}

	// Generate random file
	if(argc == 3)
	{
		unsigned int seed = atol(argv[2]);
		unsigned int temp = seed;

		bin = malloc(sizeof(char) * 1024 * 1024);
		for(int i = 0; i < 1024 * 1024; i++)
			bin[i] = rand_r(&temp) % 256;
		fp = fopen(argv[1], "wb");
		if(fp == NULL)
		{
			printf("Error: could not open file %s\n", argv[2]);
			return 1;
		}
		fwrite(bin, sizeof(char), 1024 * 1024, fp);
		printf("File %s generated with seed %u\n", argv[1], seed);
		return 0;
	}

	if(strcmp(argv[1], "-") == 0)
	{
		fp = stdin;
	}
	else
	{
		fp = fopen(argv[1], "rb");
		if(fp == NULL)
		{
			printf("Error: could not open file %s\n", argv[1]);
			return 1;
		}
	}

	// read file
	bin = malloc(sizeof(char) * 1024 * 1024);
	if(bin == NULL)
	{
		printf("Error: could not allocate memory\n");
		return 1;
	}
	size_t len = fread(bin, sizeof(char), 1024 * 1024, fp);
	if(len == 0)
	{
		printf("Error: could not read file\n");
		return 1;
	}
	
	// set-up timer for progress display
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
		.it_interval.tv_sec=1,
		.it_interval.tv_nsec=000000000L
	};

	ctr = 0;
	timer_settime(timer, TIMER_ABSTIME, &period, NULL);

	#pragma omp parallel for
	for(unsigned long long int i = 0; i <= UINT_MAX; i++)
	{
		if(match((unsigned int)i, bin, len))
		{
			printf("Seed found: %llu\n", i);
			exit(0);
		}
		#pragma omp atomic
		ctr++;
	}

	timer_delete(timer);

	printf("Seed not found\n");
	return 0;
}