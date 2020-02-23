#include <stdio.h>
#include <math.h>

int main(void)
{
	long double pi;

	pi = 4 * atanl(1);

	printf("PI=%#Lf\n", pi);

	return 0;
}
