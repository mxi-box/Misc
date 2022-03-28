/* ==================================== *\
||	OpenMP calculate Pi		||
\* ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <omp.h>
#include <mpfr.h>

//#define func(x, size)	(sqrt((size)*(size) - (x)*(x)))

int main(int argc, char **argv)
{
	intmax_t size;
	const mpfr_prec_t prec = 65536;

	MPFR_DECL_INIT(area, prec);
	MPFR_DECL_INIT(size_pow2, prec);

	if(argc != 2)
		goto error;

	size = atoll(argv[1]);

	if(size < 0 || size >= (INT_MAX))
		goto error;

	mpfr_set_zero(area, 1);
	mpfr_set_si(size_pow2, size, MPFR_RNDN);
	mpfr_sqr(size_pow2, size_pow2, MPFR_RNDN); // size^2

	#pragma omp parallel
	{
		MPFR_DECL_INIT(area_slice_delta, prec);
		MPFR_DECL_INIT(area_slice, prec);
		MPFR_DECL_INIT(x_pow2, prec);

		mpfr_set_zero(area_slice_delta, 1);
		mpfr_set_zero(area_slice, 1);
		mpfr_set_zero(x_pow2, 1);

		#pragma omp for
		for(intmax_t i = -size; i <= size; i++)
		{
			/* intergrate! */
			mpfr_set_si(x_pow2, i, MPFR_RNDN);
			mpfr_sqr(x_pow2, x_pow2, MPFR_RNDN); // x^2

			mpfr_sub(area_slice_delta, size_pow2, x_pow2, MPFR_RNDN);
			//mpfr_fprintf(stderr, "[%d] x = %ld, x^2 = %.18Rf, size^2 = %.18Rf, Da = %.18Rf, area_slice = %.18Rf\n", omp_get_thread_num(), i, x_pow2, size_pow2, area_slice_delta, area_slice);
			//mpfr_div_si(area_slice_delta, area_slice_delta, size, MPFR_RNDN);
			mpfr_sqrt(area_slice_delta, area_slice_delta, MPFR_RNDN);
			mpfr_add(area_slice, area_slice, area_slice_delta, MPFR_RNDN);
		}
		#pragma omp critical
		{
			mpfr_add(area, area, area_slice, MPFR_RNDN);
		}
	}

	mpfr_div_si(area, area, size * size / 2, MPFR_RNDN);

	mpfr_fprintf(stderr, "Pi = %.128Rf\n", area);
	exit(0);
error:
	exit(1);
}
