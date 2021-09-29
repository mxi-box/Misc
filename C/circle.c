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
				putchar('%');
				area += 0.5;
			}
			else if(d - size <= 0.0625)
			{
				putchar('X');
				area += 0.125;
			}
			else if(d - size <= 0.125)
			{
				putchar('H');
				area += 0.0625;
			}
			else if(d - size <= 0.25)
			{
				putchar('=');
				area += 0.03125;
			}
			else if(d - size <= 0.50)
			{
				putchar('-');
				area += 0.015625;
			}
			else
				putchar(' ');
		}

		putchar('\n');
	}

	printf("\nPi = %lf\n", area / (size * size));
	exit(0);
error:
	exit(1);
}
