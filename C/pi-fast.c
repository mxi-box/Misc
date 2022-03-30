/* ==================================== *\
||          MPFR calculate Pi		||
\* ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <omp.h>
#include <mpfr.h>

int main(void)
{
	const mpfr_prec_t prec = 65536;

	MPFR_DECL_INIT(pi, prec);
	MPFR_DECL_INIT(neg1, prec);

	mpfr_set_si(neg1, -1, MPFR_RNDN);

	mpfr_acos(pi, neg1, MPFR_RNDN);

	mpfr_fprintf(stderr, "Pi = %.128Rf\n", pi);
	exit(0);
}
