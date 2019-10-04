#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>

mpz_t start, stop, n, diff, temp, out_r, out_q;

int main(int argc, char **argv)
{
/* Init */
	mpz_init(start);
	mpz_init(stop);
	mpz_init(n);
	mpz_init(diff);
	mpz_init(temp);
	mpz_init(out_r);
	mpz_init(out_q);

	if(argc == 4)
	{
		gmp_sscanf(argv[1], "%Zd", start);
		gmp_sscanf(argv[2], "%Zd", diff);
		gmp_sscanf(argv[3], "%Zd", n);
	}
	else
		exit(4);

/*
 *     n(start + stop)
 *   ------------------- = out_q, out_r
 *            2
 */
	mpz_mul(stop, n, diff);
	mpz_add(temp, start, stop);
	mpz_mul(temp, n, temp);
	mpz_fdiv_qr_ui(out_q, out_r, temp, 2);
	gmp_printf("Q=%Zd, R=%Zd\n", out_q, out_r);
}
