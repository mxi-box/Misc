#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

int main()
{
	char c[4096];
	const time_t time_now = time(NULL);
	const struct tm *tm = localtime(&time_now);
	strftime(c, sizeof(c), "%Y%m%dT%H%M%S%z", tm);
	puts(c);
	return 0;
}
