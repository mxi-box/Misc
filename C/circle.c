#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef long long int num_t;

double distance(double a, double b)
{
	return sqrt(a*a + b*b);
}

int main(int argc, char **argv)
{
	double x, y;
	int size;

	if(argc > 2)
		goto error;

	size = atoi(argv[1]);
	/* plot size: size * 2 + 1 */

	for(y = size; y >= -size; y--)
	{
		for(x = -size; x <= size; x += 0.5)
		{
			if(distance(x, y) <= size)
				putchar('X');
			else
				putchar('_');
		}

		putchar('\n');
	}
	exit(0);
error:
	exit(1);
}
