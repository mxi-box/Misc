/*=================================================================*\
|| approx.c: approximate fraction from given floating point number ||
\*=================================================================*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>

mpf_t	mid,	/* A number between them */
	orig;	/* Original value */

typedef struct fake_fraction
{
	unsigned long long int x, y;
} frac_t;

void panic(char *msg, int val)
{
	fprintf(stderr, "%s\n", msg);
	exit(val);
}

/* Used to calculate a and b */
frac_t fa =
{
	.x = 0,
	.y = 1
};

frac_t fb =
{
	.x = 1,
	.y = 1
};

frac_t fmid =
{
	.x = 1,
	.y = 2
};

void get_mid(mpf_t rop, frac_t *fmid, frac_t fa, frac_t fb)
{
/*
 *   ax + bx
 *   -------
 *   ay + by
 */
	fmid->x = fa.x + fb.x;
	fmid->y = fa.y + fb.y;

	mpf_set_ui(rop, fmid->x);
	mpf_div_ui(rop, rop, fmid->y);
}

int main(int argc, char **argv)
{
	uint64_t i, iter; /* Iterations to run */
	int cmp=0;

	if(argc != 3)
		panic("?ARG", 1);

	iter = strtoull(argv[2], NULL, 0);

	mpf_set_default_prec(4096);
	printf("Default Precison: %lu\n", mpf_get_default_prec());

	mpf_init_set_d(mid, 0.5);

	if(mpf_init_set_str(orig, argv[1], 10) == -1)	/* Should be a valid float */
		panic("?INVALID", 1);

	if(mpf_cmp_ui(orig, 1) > 0)	/* Should be less than 1 */
		panic("?ONE", 1);

	for(i=1; i <= iter; i++)
	{
		get_mid(mid, &fmid, fa, fb);

		gmp_printf("(%llu/%llu)\tmin: %llu/%llu, max: %llu/%llu, approx -> %llu/%llu (%F.64f)\n", i, iter,
				fa.x, fa.y,
				fb.x, fb.y,
				fmid.x, fmid.y, mid);

		if((cmp = mpf_cmp(mid, orig)) > 0)
			fb = fmid;
		else if(cmp < 0)
			fa = fmid;
		else
			break;
	}

	return 0;
}
