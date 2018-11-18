/* ==================================== *
 *        ARC4 Implemention in C        *
 *               Neo_Chen               *
 * ==================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define KEYSIZE	256
FILE *infile;
FILE *outfile;
uint8_t sbox[256];
uint8_t key[KEYSIZE];
char *str;
size_t strlength=0;
size_t keylength=0;

int verbose=0;
uint8_t status=0;
#define ST_KEY	0x01
#define ST_IN	0x02
#define ST_INSTR	0x04
#define ST_OUT	0x08

void swap(uint8_t *a, uint8_t *b)
{
	uint8_t tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void ksa(uint8_t *sbox, uint8_t *key, size_t keylength)
{
	int i=0, j=0;
	for(i=0; i < (1<<8); ++i)
		sbox[i]=i;
	for(i=0; i < (1<<8); ++i)
	{
		j = (j + sbox[i] + key[i % keylength]) & 0xFF;
		swap(sbox + i, sbox +j);
	}
}

uint8_t prga(uint8_t *sbox, uint8_t input)
{
	static int i=0, j=0;
	uint8_t output=0;
	i = (i + 1) & 0xFF;
	j = (j + sbox[i]) & 0xFF;
	swap(sbox + i, sbox +j);
	output = input ^ sbox[(sbox[i] + sbox[j]) & 0xFF];
	if(verbose)
		printf("PRGA: S[i = %#x]=%#x,\tS[j = %#x]=%#x,\tIN=%#x,\tOUT=%#x\n", i, sbox[i], j, sbox[j], input, output);
	return output;
}

int main(int argc, char **argv)
{
	setvbuf(stderr, NULL, _IONBF, 0);

	int input=0;
	int ret=0;
	size_t ptr=0;
	int opt;
	while((opt = getopt(argc, argv, "hs:i:o:k:v")) != -1)
	{
		switch(opt)
		{
			case 'i': /* File */
				if(strcmp(optarg, "-"))
				{
					if((infile = fopen(optarg, "r")) == NULL)
					{
						perror(optarg);
						exit(8);
					}
				}
				else
					infile=stdin;
				status |= ST_IN;
				break;
			case 's':
				strlength = strlen(optarg);
				if(strlength == 0)
				{
					fputs("?STR\n", stderr);
					exit(2);
				}
				str = calloc(strlength, sizeof(char));
				strcpy(str, optarg);
				status |= ST_INSTR;
				break;
			case 'o':
				if(strcmp(optarg, "-"))
				{
					if((outfile = fopen(optarg, "w")) == NULL)
					{
						perror(optarg);
						exit(8);
					}
				}
				else
					outfile=stdout;
				status |= ST_OUT;
				break;
			case 'k':
				strncpy((char*)key, optarg, KEYSIZE);
				keylength = strnlen((char*)key, KEYSIZE);
				if(keylength == 0)
				{
					fputs("?KEY\n", stderr);
					exit(4);
				}
				status |= ST_KEY;
				break;
			case 'v':
				verbose++;
				break;
			case 'h': /* Help */
				printf("Usage: %s [-h] [-i infile] [-o outfile] [-k key]\n", argv[0]);
				exit(0);
			default:
				fputs("?INVARG\n", stderr);
				exit(1);

		}
	}

	if(! ((status & (ST_IN | ST_INSTR)) && (status & (ST_KEY | ST_OUT)) == (ST_KEY | ST_OUT)))
	{
		fputs("?ARG\n", stderr);
		exit(8);
	}

	ksa(sbox, key, keylength);

	int count=0;
	if(verbose)
	{
		for(count=0; count < 256; count++)
		{
			fprintf(stderr, "S[0x%02x]=0x%02x ", count, sbox[count]);
			if((count & 0x0F)  == 7)
				fputc('\n', stderr);
		}
	}

	if(status & ST_IN)
	{
		while((input = fgetc(infile)) != EOF && ret != EOF)
		{
			ret = fputc(prga(sbox, (char)input), outfile);
		}
		fclose(infile);
	}
	else if(status & ST_INSTR)
	{
		while(ptr < strlength && ret != EOF)
		{
			ret = fputc(prga(sbox, str[ptr]), outfile);
			ptr++;
		}
	}

	fclose(outfile);
}
