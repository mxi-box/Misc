/*=================================================================*\
|| approx.c: approximate fraction from given floating point number ||
\*=================================================================*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

long double	mid,	/* A number between them */
		orig,	/* Original value */
		diff;	/* Difference */

typedef struct fake_fraction
{
	unsigned long long int x, y;
} frac_t;

void panic(char *msg, int val)
{
	fprintf(stderr, "%s\n", msg);
	exit(val);
}

/* Upper bound / Lower bound and mediant */
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

void get_mid(long double *rop, frac_t *fmid, frac_t fa, frac_t fb)
{
/*
 *   ax + bx
 *   -------
 *   ay + by
 */
	fmid->x = fa.x + fb.x;
	fmid->y = fa.y + fb.y;

	*rop = (long double)fmid->x / (long double)fmid->y;
}

int main(int argc, char **argv)
{
	unsigned long long int i, iter; /* Iterations to run */

	if(argc != 3)
		panic("?ARG", 1);

	iter = strtoull(argv[2], NULL, 0);

	if(sscanf(argv[1], "%Lf", &orig) != 1)	/* Should be a valid float */
		panic("?INVALID", 1);

	if(orig > 1)	/* Should be less than 1 */
		panic("?ONE", 1);

	printf("Input: %s\nIterations: %llu\n", argv[1], iter);

	for(i=0; i < 80; i++)
		fputc('=', stdout);
	fputc('\n', stdout);

	for(i=1; i <= iter; i++)
	{
		get_mid(&mid, &fmid, fa, fb);
		diff = orig - mid;

		printf("(%llu/%llu)\t[min: %llu/%llu, max: %llu/%llu],\tapprox => %llu/%llu (%1.10Lf) : diff => %1.10Lf\n", i, iter,
				fa.x, fa.y,
				fb.x, fb.y,
				fmid.x, fmid.y, mid, diff);

		if(diff == 0.0)
			break;
		else if(orig < mid)
			fb = fmid;
		else if(orig > mid)
			fa = fmid;
		else
			break;
	}

	return 0;
}
