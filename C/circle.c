#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef long long int num_t;

double distance(double a, double b)
{
	return sqrt(a*a + b*b);
}

char const dispchar[] = "%XH=- ";

#ifndef NO_DRAW
#define DRAW(x)	\
	putchar(dispchar[(x)])
#else
#define DRAW(x)
#endif

int main(int argc, char **argv)
{
	double x, y, d;
	int size;
	double area = 0.0;

	if(argc > 2)
		goto error;

	size = atoi(argv[1]);

	if(size < 0)
		goto error;
	/* plot size: size * 2 + 1 */

	for(y = size; y >= -size; y--)
	{
		for(x = -size; x <= size; x += 0.5)
		{
			d = distance(x, y);

			if(d <= size)
			{
				DRAW(0);
				area += 0.5;
			}
			else if(d - size <= 0.0625)
				DRAW(1);
			else if(d - size <= 0.125)
				DRAW(2);
			else if(d - size <= 0.25)
				DRAW(3);
			else if(d - size <= 0.50)
				DRAW(4);
			else
				DRAW(5);
		}

		putchar('\n');
	}

	fprintf(stderr, "\nPi = %lf\n", area / (size * size));
	exit(0);
error:
	exit(1);
}
