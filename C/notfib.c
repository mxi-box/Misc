#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

typedef uint64_t num_t;

struct timespec duration =
{
	.tv_sec = 0,
	.tv_nsec = 200*1000*1000
};

int main(void)
{
	num_t a = 1, b = 0, c = 0, d = 0;
	while(1)
	{
		num_t tmp = c + d;
		printf("%21" PRId64 "\t%21" PRId64 "\t%21" PRId64 "\t%21" PRId64 "\n", a, b, c, d);
		d = c;
		c = b;
		b = a;
		a = tmp;
		nanosleep(&duration, NULL);
	}
	return 0;
}
