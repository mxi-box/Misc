/* ==================================== *\
 * Draw circle with high I/O throughput *
\* ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

typedef long long int num_t;

static inline double distance(double x, double y)
{
	return sqrt(x*x/4.0 + y*y);
}


int main(int argc, char **argv)
{
	int x, y;
	double d;
	int size;
	long int hsize;
	char *linebuf;
	long long unsigned int area = 0;
	int xlast = 0; /* last position of x */
	unsigned int areadelta = 0;

	if(argc > 2)
		goto error;

	size = atoi(argv[1]);

	if(size < 0 || size >= (INT_MAX / 2))
		goto error;
	/* plot size: size * 2 + 1 */

	hsize = size * 4 + 1; // Double horizontal resolution (0.5 per step)
	linebuf = malloc(hsize + 1); // include space for LF
	if(linebuf == NULL)
		goto error;
	linebuf[hsize] = '\n';

	for(y = size; y >= -size; y--)
	{
		memset(linebuf, ' ', hsize);

		/* what an interesting algorithm */
		for(x = xlast; x >= -size * 2; x += y >= 0 ? -1 : 1) /* change direction after y reaches 0 */
		{
			d = distance(x, y);

			/* Split because what value to record (before encounter / after encounter) is different */
			if(y >= 0 && d > size)
				break;
			xlast = x;
			if(y < 0 && d <= size)
				break;
		}
		memset(linebuf + xlast + size * 2, '#', areadelta = abs(xlast) * 2 + 1);
		area += areadelta;
		fwrite(linebuf, hsize + 1, sizeof(char), stdout);
	}

	fprintf(stderr, "\nPi = %lf\n", (double)area / 2.0 / ((long)size * size));
	free(linebuf);
	exit(0);
error:
	exit(1);
}
