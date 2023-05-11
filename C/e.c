/* Calculate e with some bignum magic */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

typedef long long unsigned int num_t;
typedef __uint128_t num128_t; // useful when we need to multiply two 64-bit numbers
typedef uint8_t byte;

num_t divisor_block[64][2];

double log10fractorial(num_t n)
{
	const double h = log10(2 * M_PI)/2;
	return h + log10(10 * n) * (n + 0.5) - n / log(10);
}

int highest_bitpos(num_t n)
{
	int r = 0;
	while(n >>= 1)
		r++;
	return r + 1;
}

void dump_frac(num_t *frac, num_t n)
{
	for(int i = 0; i < n; i++)
	{
		printf("_%016llx", frac[i]);
	}
	putchar('\n');
}

num_t byte_to_digits(size_t n)
{
	const size_t digits = floor(log(2)/log(10) * n * 8);
	printf("digits -> %zu\n", digits);
	return digits;
}

int main(int argc, char **argv)
{
	num_t n = 5;
	//if(argc == 2)
	//	n = atoll(argv[1]);
	num_t *efrac = calloc(n, sizeof(num_t));
	printf("n -> %llu\n", n);
	
	/*
	// index 0 is highest word
	for(num_t divisor = n; divisor > 0; divisor--)
	{
		printf("======\ndivisor -> %llu (%#llx)\n", divisor, divisor);

		// Initialize the divisor block
		for(int i = 0; i < 64; i++)
		{
			divisor_block[i][1] = divisor << i;
			if(i > 0)
				divisor_block[i][0] = divisor >> (64 - i);
			else
				divisor_block[i][0] = 0;

			printf("divisor_block[%d] -> %016llx_%016llx\n", i, divisor_block[i][0], divisor_block[i][1]);
		}

		// Do the division, assume a hidden 1 in front of the fraction part
		// i.e. always assume the first dividand "block" is larger than divisor

	}
	*/

	// how many digits do we have?

	/*
	// Fraction conversion test
	for(int i = 0; i < n; i++)
	{
		efrac[i] = 0x1234567890abcdef;
	}
	*/
	size_t digits = byte_to_digits(n * sizeof(num_t));

	// Print the result
	printf("e = 2.");
	for(num_t i = 0; i < digits; i++)
	{
		for(int j = n - 1; j >= 0; j--)
		{
			num128_t tmp = (num128_t)efrac[j] * (num128_t)10;
			efrac[j] = tmp & 0xffffffffffffffff; // lower 64 bits
			if(j > 0)
			{
				efrac[j - 1] += tmp >> 64; // upper 64 bits
			}
			else
			{
				// print the overflowed digit
				printf("%01hhu", (byte)(tmp >> 64));
			}
			//dump_frac(efrac, n);
		}
	}
	putchar('\n');
	return 0;
}