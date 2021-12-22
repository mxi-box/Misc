/* ==================================== *\
 * Draw circle with high I/O throughput *
\* ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

typedef long long int num_t;

#define func(x, size)	(sqrt((size)*(size) - ((long)x)*((long)x)))

int main(int argc, char **argv)
{
	int x;
	int size;
	long int hsize;
	char *linebuf;
	double area = 0;
	double areadelta = 0;
	int startpos;

	if(argc != 2)
		goto error;

	size = atoi(argv[1]);

	if(size < 0 || size >= (INT_MAX / 2))
		goto error;
	/* plot size: size * 2 + 1 */

	hsize = size * 4 + 1; // Double horizontal resolution (0.5 per step)
	linebuf = calloc(hsize + 1, sizeof(char)); // include space for LF
	if(linebuf == NULL)
		goto error;
	linebuf[hsize] = '\n';

	for(x = -size; x <= size; x++)
	{
		/* intergrate! */
		areadelta = func(x, (long)size) * 2;
		startpos = round(areadelta);

		//memset(linebuf, ' ', hsize); /* Fill blank */
		memset(linebuf + size * 2 - startpos, '#', startpos * 2 + 1); /* Fill body */
		memset(linebuf, ' ', size * 2 - startpos); /* Fill left side empty space */
		memset(linebuf + size * 2 + startpos + 1, ' ', size * 2 - startpos); /* Fill right side empty space */

		fwrite(linebuf, hsize + 1, sizeof(char), stdout);
		area += areadelta;
	}

	fprintf(stderr, "\nPi = %lf\n", area / ((long)size * size));
	free(linebuf);
	exit(0);
error:
	exit(1);
}
