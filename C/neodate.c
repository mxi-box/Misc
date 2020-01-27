#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
int main()
{
	char c[4096];
	const time_t time_now=time(NULL);
	const struct tm *tmp=localtime( &time_now );
	strftime(c, sizeof(c), "%FT%T", tmp);
	puts(c);
	return 0;
}
