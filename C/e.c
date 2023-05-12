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

#define true	1
#define false	0

#define WORD_SIZE (32)

typedef uint32_t word_t;
typedef uint64_t dword_t; // necessary

// Calculating the accurate value of log2(n!) isn't that prohibitive on modern hardware
double log2fractorial(word_t n)
{
	return log2(2 * M_PI)/2 + log2(n) * (n + 0.5) - n / log(2);
}

void dump_frac(word_t *frac, word_t n)
{
	for(size_t i = 0; i < n; i++)
	{
		printf("_%08" PRIx32, frac[i]);
	}
	putchar('\n');
}

word_t to_digits_precision(size_t n, size_t word_size)
{
	const size_t digits = floor(log(2)/log(10) * n * word_size);
	return digits;
}

static inline word_t intpow10(word_t n)
{
	word_t r = 1;
	assert(n <= 19);
	for(word_t i = 0; i < n; i++)
		r *= 10;
	return r;
}

#define GROUP_SIZE (9)
const word_t pow10_9 = 1000000000ULL;

void print_fraction(word_t *frac, size_t n, size_t digits)
{
	// in groups of digits
	for(size_t i = 0; i < digits/GROUP_SIZE; i++)
	{
		dword_t tmp64b = 0;
		for(ssize_t j = n - 1; j >= 0; j--)
		{
			tmp64b = (dword_t)frac[j] * pow10_9 + tmp64b;
			frac[j] = tmp64b; // lower 64 bits
			tmp64b >>= WORD_SIZE; // carry
			//dump_frac(efrac, n);
		}
		printf("%09" PRIu32, (word_t)(tmp64b));
	}

	// the rest in one group
	size_t rest = digits % GROUP_SIZE;
	char fmtspec[16];

	dword_t tmp64b = 0;
	for(ssize_t j = n - 1; j >= 0; j--)
	{
		tmp64b = (dword_t)frac[j] * intpow10(rest) + tmp64b;
		frac[j] = tmp64b; // lower 64 bits
		tmp64b >>= WORD_SIZE;
		//dump_frac(efrac, n);
	}
	sprintf(fmtspec, "%%0%zu" PRIu32, rest);
	printf(fmtspec, (word_t)(tmp64b));
}

/* Algorithm by Steve Wozniak in 1980
 * https://archive.org/details/byte-magazine-1981-06/page/n393/mode/1up
 */

static inline void fraction_div_add1(word_t *frac, size_t n, word_t divisor)
{
	dword_t tmp_partial_dividend = 1;
	for(size_t i = 0; i < n; i++)
	{
		word_t tmp_quotient = 0;
		tmp_partial_dividend <<= WORD_SIZE;
		tmp_partial_dividend |= frac[i];
		tmp_quotient = tmp_partial_dividend / divisor;
		tmp_partial_dividend %= divisor;

		frac[i] = tmp_quotient;
	}
}

volatile word_t current_divisor = 0;
word_t terms = 5;

void display(union sigval sigval)
{
	clock_t ticks = clock();
	word_t current = terms - current_divisor;
	static clock_t last_ticks = 0;
	static word_t last = 0;
	if(last_ticks == 0)
	{
		last_ticks = ticks;
		return;
	}
	fprintf(stderr, ">%7.3f%% (%" PRIu32 "/%" PRIu32 ") @%lluT/s\n",
		(float)current / terms * 100,
		current,
		terms,
		(long long unsigned int)((current - last)*CLOCKS_PER_SEC/(ticks - last_ticks + 1))
	);
	last = current;
	last_ticks = ticks;
}

int main(int argc, char **argv)
{
	int hex_mode = false;
	if(argc == 2)
	{
		if(argv[1][0] == '-')
		{
			hex_mode = true;
			argv[1]++;
		}
		sscanf(argv[1], "%" SCNu32, &terms);
	}

	if(terms == 0)
	{
		fprintf(stderr, "Invalid term count\n");
		return 1;
	}

	double precision = log2fractorial(terms);
	fprintf(stderr, "estimated required precision: log2(%" PRIu32 "!) ~= %lfbits\n", terms, precision);
	
	size_t efrac_size = ceil(precision / WORD_SIZE);
	word_t *efrac = calloc(efrac_size, sizeof(word_t));
	fprintf(stderr, "allocated %zd %dbit words (%zu bit)\n", efrac_size, WORD_SIZE, efrac_size * WORD_SIZE);

	// how many decimal digits do we have?
	size_t digits = to_digits_precision(efrac_size, WORD_SIZE);
	fprintf(stderr, "will print %zu digits\n", digits);

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

	current_divisor = terms;
	timer_settime(timer, TIMER_ABSTIME, &period, NULL);

	// divisor = 1 is impossible as we don't really store the integer part
	for(word_t divisor = terms; divisor > 1; divisor--)
	{
		fraction_div_add1(efrac, efrac_size, divisor);
		current_divisor = divisor;
	}
	timer_delete(timer);

	putc('\n', stderr);

	// Print the result
	if(hex_mode)
	{
		for(size_t n = 0; n < efrac_size; n++)
			printf("%08" PRIx32 "_", efrac[n]);
		putchar('\n');
		return 0;
	}
	else
	{
		printf("e = 2.");
		print_fraction(efrac, efrac_size, digits);
		putchar('\n');
	}
	return 0;
}
