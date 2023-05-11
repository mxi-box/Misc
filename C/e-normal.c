#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

typedef long double ld_t;

int main(int argc, char **argv)
{
	size_t n = 5;
	if(argc == 2)
		n = atoll(argv[1]);
	
	ld_t e = 1;
	for(size_t i = n; i > 0; i--)
	{
		e = e / i + 1;
		printf("e -> %.*Lg\n", LDBL_DIG, e);
	}
}