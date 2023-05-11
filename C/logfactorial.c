#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

int main(int argc, char **argv)
{
	long long int n = 1;
	if(argc == 2)
		n = atoll(argv[1]);

	if(n < 1)
		return 1;

	long double x = 0;
	for(long long int i = 1; i <= n; i++)
		x += log2l(i);

	printf("log2(%lld!) ~= %.*Lg\n", n, LDBL_DIG, x);
	return 0;
}
