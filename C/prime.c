#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TRUE	1
#define FALSE	0

int is_prime(int a)
{
	int i=0;
	for(i=2; i< (a/2); i++)
	{
		if(a%i == 0)
			return FALSE;
	}
	return TRUE;
}

int main(void)
{
	int num=0, i=0;
	fputs("?NUM=", stdout);
	scanf("%d", &num);

	for(i=2; i<=num; i++)
	{
		if(is_prime(i))
			printf("%d\n", i);
	}
}
