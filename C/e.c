/* Calculate e with some bignum magic */
/* NOTE: index 0 is the highest word */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <inttypes.h>
#include <assert.h>
#include <string.h>
#include <omp.h>

#define true	1
#define false	0

#define WORD_SIZE (64)
#define HEX_WIDTH "016"

typedef uint64_t word_t;
#if __SIZEOF_INT128__ != 16
	#error "No 128-bit integer support"
#endif
typedef __uint128_t dword_t; // necessary
typedef uint8_t byte;

// Calculating the accurate value of log2(n!) isn't that prohibitive on modern hardware
double log2fractorial(word_t n)
{
	return log2(2 * M_PI)/2 + log2(n) * (n + 0.5) - n / log(2);
}

void dump_frac(word_t *frac, size_t n)
{
	for(size_t i = 0; i < n; i++)
	{
		printf("_%" HEX_WIDTH PRIx64, frac[i]);
	}
	putchar('\n');
}

size_t to_digits_precision(size_t n, size_t word_size)
{
	const size_t digits = floor(log(2)/log(10) * n * word_size);
	return digits;
}

static inline word_t intpow10(byte n)
{
	word_t r = 1;
	assert(n <= 19);
	for(word_t i = 0; i < n; i++)
		r *= 10;
	return r;
}

#define GROUP_SIZE (19)
const word_t pow10_19 = 10000000000000000000ULL;

void print_fraction(word_t *frac, size_t n, size_t digits)
{
	// in groups of digits
	for(size_t i = 0; i < digits/GROUP_SIZE; i++)
	{
		dword_t wide = 0;
		for(ssize_t j = n - 1; j >= 0; j--)
		{
			wide = (dword_t)frac[j] * pow10_19 + wide;
			frac[j] = wide; // lower 64 bits
			wide >>= WORD_SIZE; // carry
			//dump_frac(efrac, n);
		}
		printf("%019" PRIu64, (word_t)(wide));
	}

	// the rest in one group
	size_t rest = digits % GROUP_SIZE;
	char fmtspec[32];

	dword_t wide = 0;
	for(ssize_t j = n - 1; j >= 0; j--)
	{
		wide = (dword_t)frac[j] * intpow10(rest) + wide;
		frac[j] = wide; // lower 64 bits
		wide >>= WORD_SIZE;
		//dump_frac(efrac, n);
	}
	sprintf(fmtspec, "%%0%zu" PRIu64, rest);
	printf(fmtspec, (word_t)(wide));
}

volatile word_t ctr = 0;
volatile word_t secs = 0;
word_t terms = 5;

void display(union sigval sigval)
{
	// silence the compiler
	(void)sigval;

	static word_t last = 0;
	secs += 1;
	fprintf(stderr, ">%7.3f%% (%" PRIu64 "/%" PRIu64 ") @%lluT/s (%lluT/s avg.)\n",
		(float)ctr * 100 / terms,
		ctr,
		terms,
		(long long unsigned int)(ctr - last),
		(long long unsigned int)(ctr / secs)
	);
	last = ctr;
}

/* Algorithm by Steve Wozniak in 1980
 * https://archive.org/details/byte-magazine-1981-06/page/n393/mode/1up
 * 
 * Parallelized by me
 */

// Long Fixed-Point Division with remainder
 static inline word_t lfixdiv(word_t *efrac, size_t current, word_t divisor, word_t remainder)
 {
	if(divisor <= 1)
		return 0;

	dword_t tmp_partial_dividend = remainder;
	tmp_partial_dividend = (tmp_partial_dividend << WORD_SIZE) | efrac[current];
	efrac[current] = tmp_partial_dividend / divisor;
	tmp_partial_dividend %= divisor;
	return (word_t)tmp_partial_dividend;
 }

 static inline void efrac_calc(word_t *efrac, size_t start, size_t end, word_t divisor, word_t *remainders, word_t intensity)
 {
	if(divisor <= 1 || start == end)
		return;
	for(size_t i = start; i < end; i++)
	{
		for(word_t j = 0; j < intensity; j++)
		{
			remainders[j] = lfixdiv(efrac, i, divisor - j, remainders[j]);
		}
	}
 }

 static inline void ecalc_parallel(size_t efrac_size, word_t *efrac, const word_t intensity, word_t remainders[][intensity], word_t *divisors_pipeline)
 {
	#pragma omp single
	for(size_t i = 0; i < intensity; i++)
	{
		remainders[0][i] = 1;
	}

	#pragma omp parallel shared(remainders, efrac, efrac_size, divisors_pipeline, intensity)
	{
		int t = omp_get_thread_num();
		size_t chunk_size = efrac_size / omp_get_num_threads();
		size_t start = t * chunk_size;
		size_t end = (t + 1) * chunk_size;
		if(t == omp_get_num_threads() - 1)
			end = efrac_size;
		efrac_calc(efrac, start, end, divisors_pipeline[t], remainders[t], intensity);
	}

	#pragma omp barrier
	#pragma omp single
	{
		for(int i = omp_get_max_threads() - 1; i > 0; i--)
		{
			memcpy(remainders[i], remainders[i - 1], sizeof(remainders[0]));
		}
	}
 }

static inline void ecalc(word_t *efrac, size_t efrac_size, word_t terms, word_t intensity)
{
	int maxt = omp_get_max_threads();
	word_t divisors_pipeline[maxt];
	word_t remainders[maxt][intensity];

	// initialize the pipeline
	for(int i = 0; i < maxt; i++)
		divisors_pipeline[i] = 0;

	memset(remainders, 0, sizeof(remainders));

	if(efrac_size < (size_t)maxt)
	{
		fprintf(stderr, "calculating e with 1 thread\n");
		for(word_t divisor = terms; divisor >= intensity; divisor -= intensity)
		{
			for(size_t i = 0; i < intensity; i++)
			{
				remainders[0][i] = 1;
			}
			efrac_calc(efrac, 0, efrac_size, divisor, remainders[0], intensity);
			ctr++;
		}
		for(word_t divisor = terms % intensity; divisor > 1; divisor--)
		{
			remainders[0][0] = 1;
			efrac_calc(efrac, 0, efrac_size, divisor, remainders[0], 1);
			ctr++;
		}

	}
	else
	{
		fprintf(stderr, "calculating e with %d threads, intensity = %zd\n", maxt, intensity);
		// divisor = 1 is impossible as we don't really store the integer part
		for(word_t divisor = terms; divisor >= intensity; divisor -= intensity)
		{
			for(size_t i = maxt - 1; i > 0; i--)
				divisors_pipeline[i] = divisors_pipeline[i - 1];
			divisors_pipeline[0] = divisor;

			// print out the array
			/*
			fprintf(stderr, "divisor =\t");
			for(int i = 0; i < maxt; i++)
				fprintf(stderr, "%" PRIu64 "\t", divisors_pipeline[i]);
			fprintf(stderr, "\n");
			*/
			ecalc_parallel(efrac_size, efrac, intensity, remainders, divisors_pipeline);
			ctr += intensity;
		}

		// finish the pipeline
		for(int i = 0; i < maxt; i++)
		{
			for(int i = maxt - 1; i > 0; i--)
				divisors_pipeline[i] = divisors_pipeline[i - 1];
			divisors_pipeline[0] = 0;
			ecalc_parallel(efrac_size, efrac, intensity, remainders, divisors_pipeline);
		}

		for(word_t divisor = terms % intensity; divisor > 1; divisor--)
		{
			for(size_t i = maxt - 1; i > 0; i--)
				divisors_pipeline[i] = divisors_pipeline[i - 1];
			divisors_pipeline[0] = divisor;

			// print out the array
			/*
			fprintf(stderr, "divisor =\t");
			for(int i = 0; i < maxt; i++)
				fprintf(stderr, "%" PRIu64 "\t", divisors_pipeline[i]);
			fprintf(stderr, "\n");
			*/
			ecalc_parallel(efrac_size, efrac, 1, remainders, divisors_pipeline);
			ctr++;
		}
		fprintf(stderr, "Finalizing...\n");
		// finish the pipeline
		for(int i = 0; i < maxt; i++)
		{
			for(int i = maxt - 1; i > 0; i--)
				divisors_pipeline[i] = divisors_pipeline[i - 1];
			divisors_pipeline[0] = 0;
			ecalc_parallel(efrac_size, efrac, 1, remainders, divisors_pipeline);
		}

	}
}

int main(int argc, char **argv)
{
	int hex_mode = false;
	word_t intensity = 1;
	if(argc >= 2)
	{
		if(argv[1][0] == '-')
		{
			hex_mode = true;
			argv[1]++;
		}
		sscanf(argv[1], "%" SCNu64, &terms);
	}

	if(argc == 3)
	{
		sscanf(argv[2], "%" SCNu64, &intensity);
	}

	if(terms == 0)
	{
		fprintf(stderr, "Invalid term count\n");
		return 1;
	}

	if(intensity == 0 || intensity > terms)
	{
		fprintf(stderr, "Invalid intensity\n");
		return 1;
	}

	double precision = log2fractorial(terms);
	fprintf(stderr, "estimated required precision: log2(%" PRIu64 "!) ~= %lfbits\n", terms, precision);
	
	size_t efrac_size = ceil(precision / WORD_SIZE);
	word_t *efrac = calloc(efrac_size, sizeof(word_t));
	fprintf(stderr, "allocated %zd %dbit words (%zu bit)\n", efrac_size, WORD_SIZE, efrac_size * WORD_SIZE);

	// how many decimal digits do we have?
	size_t digits = to_digits_precision(efrac_size, WORD_SIZE);
	if(!hex_mode)
	{
		fprintf(stderr, "will print %zu digits\n", digits);
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

	// calculate e
	ecalc(efrac, efrac_size, terms, intensity);

	timer_delete(timer);

	putc('\n', stderr);

	// Print the result
	if(hex_mode)
	{
		for(size_t n = 0; n < efrac_size; n++)
			printf("%" HEX_WIDTH PRIx64 "_", efrac[n]);
		putchar('\n');
		return 0;
	}
	else
	{
		printf("e = 2.");
		print_fraction(efrac, efrac_size, digits);
		putchar('\n');
	}

	free(efrac);
	return 0;
}
