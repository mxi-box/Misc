#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>

mpz_t ans[4];
mpz_t fib[4];
mpz_t tmp[4];

unsigned long long int n=(1<<20);

void mul(mpz_t *a, mpz_t *b)
{
	short int i=0;
	mpz_mul(tmp[0], a[0], b[0]);
	mpz_addmul(tmp[0], a[1], b[2]);

	mpz_mul(tmp[1], a[0], b[1]);
	mpz_addmul(tmp[1], a[1], b[3]);

	mpz_mul(tmp[2], a[2], b[0]);
	mpz_addmul(tmp[2], a[3], b[2]);

	mpz_mul(tmp[3], a[2], b[1]);
	mpz_addmul(tmp[3], a[3], b[3]);

	for(i=0; i < 4; i++)
		mpz_set(a[i], tmp[i]);
}

void power(mpz_t *f, unsigned long long int n)
{
	if(n == 0 || n == 1)
		return;

	power(f, n >> 1);
	mul(f, f);
	if(n & 0x1)
		mul(f, fib);
}

void fibonacci(mpz_t *f, unsigned long long int n)
{
	if(n-- == 0)
	{
		mpz_set_ui(f[0], 0);
		return;
	}

	power(f, n);
}

int main(int argc, char **argv)
{
	int i=0;
	if(argc >= 2)
		n = strtoull(argv[1], NULL, 10);

	for(i=0; i < 4; i++)
	{
		mpz_init(ans[i]);
		mpz_init(fib[i]);
		mpz_init(tmp[i]);
	}

	mpz_set_ui(fib[0], 1);
	mpz_set_ui(fib[1], 1);
	mpz_set_ui(fib[2], 1);
	mpz_set_ui(fib[3], 0);

	mpz_set_ui(ans[0], 1);
	mpz_set_ui(ans[1], 1);
	mpz_set_ui(ans[2], 1);
	mpz_set_ui(ans[3], 0);

	fibonacci(ans, n);

	gmp_printf("%llu,\t%Zd\n", n, ans[0]);
}
