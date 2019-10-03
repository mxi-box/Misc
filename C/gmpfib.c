#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>

#define LAST		-1

mpz_t num[3];
int mode = 0;

unsigned long long int max=ULONG_MAX, i=0;

int main(int argc, char **argv)
{
	if(argc >= 2)
	{
		if(argv[1][0] == '-')
		{
			max = strtoull(argv[1] + 1, NULL, 10);
			mode = LAST;
		}
		else
			max = strtoull(argv[1], NULL, 10);
	}

	for(i=0; i < 3; i++)
		mpz_init(num[i]);

	mpz_set_ui(num[0], 1);
	mpz_set_ui(num[1], 0);
	mpz_set_ui(num[2], 0);

	i=0;
	if(mode)
	{
		while(i <= max)
		{
			switch(i % 3)
			{
				case 0:
					mpz_add(num[2], num[0], num[1]);
					break;
				case 1:
					mpz_add(num[0], num[1], num[2]);
					break;
				case 2:
					mpz_add(num[1], num[2], num[0]);
					break;
			}
			i++;
		}
		switch((i -= 1) % 3)
		{
			case 0:
				gmp_printf("%llu,\t%Zd\n", i, num[1]);
				break;
			case 1:
				gmp_printf("%llu,\t%Zd\n", i, num[2]);
				break;
			case 2:
				gmp_printf("%llu,\t%Zd\n", i, num[0]);
				break;
		}
	}
	else
	{
		while(i <= max)
		{
			switch(i % 3)
			{
				case 0:
					mpz_add(num[2], num[0], num[1]);
					break;
				case 1:
					mpz_add(num[0], num[1], num[2]);
					break;
				case 2:
					mpz_add(num[1], num[2], num[0]);
					break;
			}
					gmp_printf("%llu,\t%Zd\n", i, num[(i+1)%3]);
			i++;
		}
	}
}
