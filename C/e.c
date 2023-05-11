/* Calculate e with some bignum magic */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <mpfr.h>

typedef long long unsigned int num_t;
typedef uint8_t byte;

double log2factorial(num_t n)
{
	const double h = log2(2 * M_PI)/2;
	return h + log2(10 * n) * (n + 0.5) - n / log(2);
}

num_t byte_to_digits(size_t n)
{
	const size_t digits = floor(log(2)/log(10) * n * 8);
	printf("digits -> %zu\n", digits);
	return digits;
}

int main(int argc, char **argv)
{
	num_t n = 1024;
	int fpif_export = 0;
	if(argc == 2)
	{
		if(argv[1][0] == '-')
		{
			fpif_export = 1;
			argv[1]++;
		}
		n = atoll(argv[1]);
	}
	mpfr_prec_t prec = ceil(log2factorial(n));

	if(prec > MPFR_PREC_MAX || prec < MPFR_PREC_MIN)
	{
		mpfr_fprintf(stderr, "precision %Pu is out of range (%Pu to %Pu)\n", prec, MPFR_PREC_MIN, MPFR_PREC_MAX);
		return 1;
	}

	mpfr_t e;
	mpfr_init2(e, prec);
	fprintf(stderr, "estimated precision -> %lu bits (%lf digits)\n", prec, prec * log10(2));
	mpfr_set_ui(e, 1, MPFR_RNDN);

	for(num_t i = n; i > 0; i--)
	{
		mpfr_div_ui(e, e, i, MPFR_RNDN);
		mpfr_add_ui(e, e, 1, MPFR_RNDN);
	}

	if(fpif_export)
	{
		mpfr_fpif_export(stdout, e);
	}
	else
	{
		puts("e = ");
		mpfr_out_str(stdout, 10, 0, e, MPFR_RNDN);
		puts("\n");
	}
	return 0;
}