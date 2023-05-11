/* Calculate e with some bignum magic */
/* NOTE: index 0 is the highest word */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <inttypes.h>

#define WORD_SIZE (64)

typedef uint64_t num_t;
#if __SIZEOF_INT128__ != 16
	#error "No 128-bit integer support"
#endif
typedef __uint128_t num128_t; // necessary
typedef uint8_t byte;

num_t divisor_block[WORD_SIZE][2];
#define LAST_DIVISOR_BLOCK (WORD_SIZE - 1)

double log2fractorial(num_t n)
{
	const double h = log2(2 * M_PI)/2;
	return h + log2(10 * n) * (n + 0.5) - n / log(2);
}

void dump_frac(num_t *frac, num_t n)
{
	for(size_t i = 0; i < n; i++)
	{
		printf("_%016" PRIx64, frac[i]);
	}
	putchar('\n');
}

num_t to_digits_precision(size_t n, size_t word_size)
{
	const size_t digits = floor(log(2)/log(10) * n * word_size);
	printf("digits -> %zu\n", digits);
	return digits;
}

static inline num_t intpow10(num_t n)
{
	num_t r = 1;
	for(num_t i = 0; i < n; i++)
		r *= 10;
	return r;
}

#define GROUP_SIZE (19)
const num_t pow10_19 = 10000000000000000000ULL;

void print_fraction(num_t *frac, size_t n, size_t digits)
{
	// in groups of digits
	for(size_t i = 0; i < digits/GROUP_SIZE; i++)
	{
		num128_t tmp128b = 0;
		for(ssize_t j = n - 1; j >= 0; j--)
		{
			tmp128b = (num128_t)frac[j] * pow10_19 + tmp128b;
			frac[j] = tmp128b; // lower 64 bits
			tmp128b >>= 64; // carry
			//dump_frac(efrac, n);
		}
		printf("%019" PRIu64, (num_t)(tmp128b));
	}

	// the rest in one group
	size_t rest = digits % GROUP_SIZE;
	char fmtspec[16];

	num128_t tmp128b = 0;
	for(ssize_t j = n - 1; j >= 0; j--)
	{
		tmp128b = (num128_t)frac[j] * intpow10(rest) + tmp128b;
		frac[j] = tmp128b; // lower 64 bits
		tmp128b >>= 64;
		//dump_frac(efrac, n);
	}
	sprintf(fmtspec, "%%0%zu" PRIu64, rest);
	printf(fmtspec, (num_t)(tmp128b));
}

/* Algorithm by Steve Wozniak in 1980
 * https://archive.org/details/byte-magazine-1981-06/page/n393/mode/1up
 */

// TODO: remove requirement for 128-bit integers here as it's not "that" necessary in the strict sense
// Or maybe not, depending on which implementation is faster
void fraction_div_add1(num_t *frac, size_t n, num_t divisor_block[][2])
{
	num_t remainder = 1;	// the implicit 1 at the beginning
	for(size_t i = 0; i < n; i++)
	{
		num_t tmp_quotient = 0;
		num128_t tmp_partial_dividend = (num128_t)remainder << 64 | frac[i];
		num128_t tmp_divosor;

		for(int j = LAST_DIVISOR_BLOCK; j >= 0; j--)
		{
			tmp_divosor = (num128_t)divisor_block[j][0] << 64 | divisor_block[j][1];
			if(tmp_partial_dividend >= tmp_divosor)
			{
				tmp_quotient |= 1ULL << j;
				tmp_partial_dividend -= tmp_divosor;
			}
		}
		frac[i] = tmp_quotient;
		remainder = tmp_partial_dividend; // lower 64bits
	}
}

int main(int argc, char **argv)
{
	num_t terms = 5;
	if(argc == 2)
		terms = atoll(argv[1]);

	double precision = log2fractorial(terms);
	printf("estimated required precision: log2(%" PRIu64 "!) ~= %lfbits\n", terms, precision);
	
	size_t efrac_size = ceil(precision / WORD_SIZE);
	num_t *efrac = calloc(efrac_size, sizeof(num_t));
	printf("allocated %" PRIu64 " 64bit words (%zu bit)\n", efrac_size, efrac_size * WORD_SIZE);

	// divisor = 1 is impossible as we don't really store the integer part
	// TODO: no need to split the divisor block into two parts if using 128-bit integers
	for(num_t divisor = terms; divisor > 1; divisor--)
	{
		//printf("======\ndivisor -> %" PRIu64 " (%#" PRIx64 ")\n", divisor, divisor);

		// Initialize the divisor block
		for(int i = 0; i < WORD_SIZE; i++)
		{
			divisor_block[i][1] = divisor << i;
			if(i > 0)
				divisor_block[i][0] = divisor >> (64 - i);
			else
				divisor_block[i][0] = 0;

			//printf("divisor_block[%d] -> %016" PRIx64 "_%016" PRIx64 "\n", i, divisor_block[i][0], divisor_block[i][1]);
		}

		fraction_div_add1(efrac, efrac_size, divisor_block);
		//dump_frac(efrac, efrac_size);
	}

	// how many digits do we have?

	/*
	// Fraction conversion test
	for(int i = 0; i < n; i++)
	{
		efrac[i] = 0x1234567890abcdef;
	}
	*/
	size_t digits = to_digits_precision(efrac_size, sizeof(num_t));

	// Print the result
	printf("e = 2.");
	print_fraction(efrac, efrac_size, digits);
	putchar('\n');
	return 0;
}