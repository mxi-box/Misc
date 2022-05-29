/* Current UNIX epoch in 6-digit precision */

#include <stdio.h>
#include <time.h>

struct timespec now;

int main(void)
{
	clock_gettime(CLOCK_REALTIME, &now);

	printf("%ld.%09ld\n", now.tv_sec, now.tv_nsec);
}
