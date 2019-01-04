#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int data[4096];

int gcd(int a, int b)
{
	int t=1;
	while(t != 0)
	{
		t=a%b;
		a=b;
		b=t;
	}
	return a;
}

void divall(int *data, int size, int division)
{
	int i=0;
	for(i=0; i<size; i++)
	{
		printf("DIV data[%d], %d", i, division);
		data[i] /= division;
	}
}

int readin(int *data, FILE *fd)
{
	char *buffer=calloc(4096, sizeof(char));
	size_t ptr=0;
	int intptr=0;
	fgets(buffer, 4096, fd);
	while(ptr < 4096)
	{
		if(ptr == 0)
			sscanf(buffer + ptr, "%d", &data[intptr++]);
		else if(buffer[ptr] == ',')
			sscanf(buffer + ptr, ",%d", &data[intptr++]);
		ptr++;
	}
	return intptr;
}

int main(void)
{
	int i=0, numbers=0;
	/* Init */
	for(i=0; i<4096; i++)
		data[i]=0;
	fputs("?NUM=", stdout);
	numbers = readin(data, stdin);
	i=1;
	int temp=data[0];
	while(i < numbers)
	{
		temp = temp * data[i] / gcd(data[i], temp);
		i++;
	}
	printf("ANSWER=%d\n", temp);
}
