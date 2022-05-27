/* Current UNIX epoch in 6-digit precision */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>

struct timespec now;

int main(void)
{
	clock_gettime(CLOCK_REALTIME, &now);

	long double nsec = (long double)now.tv_nsec / 1000000000;
	printf("%.6Lf\n", nsec + now.tv_sec);
}
