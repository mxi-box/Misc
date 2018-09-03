#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* Coredump Generating Program
 * Neo_Chen <chenkolei@gmail.com>
 * Released to Public Domain
 */

int main(void)
{
	raise(SIGABRT);
	return 8;
}
