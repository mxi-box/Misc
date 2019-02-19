/* ========================================================================== *\
||                   Neo_Chen's Bignum Library Routine Set                    ||
\* ========================================================================== */

/* ========================================================================== *\
||   This is free and unencumbered software released into the public domain.  ||
||									      ||
||   Anyone is free to copy, modify, publish, use, compile, sell, or	      ||
||   distribute this software, either in source code form or as a compiled    ||
||   binary, for any purpose, commercial or non-commercial, and by any	      ||
||   means.								      ||
||									      ||
||   In jurisdictions that recognize copyright laws, the author or authors    ||
||   of this software dedicate any and all copyright interest in the	      ||
||   software to the public domain. We make this dedication for the benefit   ||
||   of the public at large and to the detriment of our heirs and	      ||
||   successors. We intend this dedication to be an overt act of	      ||
||   relinquishment in perpetuity of all present and future rights to this    ||
||   software under copyright law.					      ||
||									      ||
||   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	      ||
||   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF       ||
||   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   ||
||   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR        ||
||   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,    ||
||   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR    ||
||   OTHER DEALINGS IN THE SOFTWARE.					      ||
||									      ||
||   For more information, please refer to <http://unlicense.org/>            ||
\* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#define LENGTH 512

#define POS 0	/* Positive */
#define NEG -1	/* Negative */

#define NEG_CHAR	-1
#define INVAILD_CHAR	-2

#define A	1
#define B	-1
#define SAME	0

struct _bignum
{
	unsigned int size;
	unsigned int ndigit; /* Number of digits */
	int8_t sign;
	int8_t *digit;	/* Signed because it would be easier to handle subtraction */
};

/* Digits is stored as little endian BCD (One digit per byte, so wasteful?) */

typedef struct _bignum bignum_t;

#define CARRY(bignum, ptr)	\
		if(bignum->digit[ptr] >= 10)					\
		{								\
			bignum->digit[ptr + 1] += bignum->digit[ptr] / 10;	\
			bignum->digit[ptr] %= -10;				\
		}

#define BORROW(bignum, ptr)	\
		if(bignum->digit[ptr] < 0)					\
		{								\
			bignum->digit[ptr + 1] -= bignum->digit[ptr] / 10 + 1;	\
			bignum->digit[ptr] += 10;				\
		}

bignum_t *bignum_init(unsigned int size)
{
	bignum_t *bignum = calloc(1, sizeof(bignum_t));
	bignum->size = size;
	bignum->digit = calloc(size, sizeof(int8_t));
	return bignum;
}

void bignum_destroy(bignum_t **bignum)
{
	free(*bignum);
	*bignum = NULL;
}

/* Compares absolute value, doesn't care sign */
int8_t bignum_digitcmp(bignum_t *src1, bignum_t *src2)
{
	int ptr = 0;
	if(src1->ndigit != src2->ndigit)
	{
		if(src1->ndigit > src2->ndigit)
			return A;
		else if(src1->ndigit < src2->ndigit)
			return B;
	}
	else
	{
		for(ptr = src1->ndigit - 1; ptr >= 0; ptr--)
		{
			if(src1->digit[ptr] > src2->digit[ptr])
				return A;
			else if(src1->digit[ptr] < src2->digit[ptr])
				return B;
		}
	}
	/* They are completely equal */
	return SAME;
}

/* This one does care about sign */
int8_t bignum_cmp(bignum_t *src1, bignum_t *src2)
{
	if(src1->sign == src2->sign)
		return src1->sign ?
			- bignum_digitcmp(src1, src2) :	/* Negative, reverse the result */
			bignum_digitcmp(src1, src2);
	else
	{
		if(src1->sign == POS)
			return A;
		else if(src2->sign == POS)
			return B;
	}
	return 0;
}


void bignum_setzero(bignum_t *bignum)
{
	unsigned int ptr = bignum->ndigit;
	while((ptr--) > 0)
		bignum->digit[ptr] = 0;
	bignum->ndigit=0;
}

unsigned int bignum_len(bignum_t *bignum)
{
	unsigned int ptr = bignum->size;
	while(ptr > 0)
	{
		if(bignum->digit[--ptr] != 0x00)
			return ptr + 1; /* Pointer position to size */
	}
	return 0;
}

void bignum_rawadd(bignum_t *big, bignum_t *small, bignum_t *dst)
{
	unsigned int ptr=0;

	bignum_setzero(dst);
	while(ptr < small->ndigit)
	{
		dst->digit[ptr] += big->digit[ptr] + small->digit[ptr];
		CARRY(dst, ptr);
		ptr++;
	}

	while(ptr < big->ndigit)	/* Add rest of big it to dst */
	{
		dst->digit[ptr] += big->digit[ptr];
		CARRY(dst, ptr);
		ptr++;
	}
}

void bignum_rawsub(bignum_t *big, bignum_t *small, bignum_t *dst)
{
	unsigned int ptr=0;

	bignum_setzero(dst);
	while(ptr < small->ndigit)
	{
		dst->digit[ptr] += big->digit[ptr] - small->digit[ptr];
		BORROW(dst, ptr);
		ptr++;
	}

	while(ptr < big->ndigit)
	{
		dst->digit[ptr] += big->digit[ptr];
		BORROW(dst, ptr);
		ptr++;
	}
}

void bignum_add(bignum_t *a, bignum_t *b, bignum_t *dst)
{
	/* Size checking */
	if(dst->size < (a->ndigit + b->ndigit))
		return;

	if(a->sign == b->sign)
	{
		bignum_rawadd(a, b, dst);
		dst->sign = a->sign;
	}
	else
	{
			int8_t cmp = bignum_digitcmp(a, b);
			if(a->sign == POS && cmp == A)		/* A > B, (A) + (B) */
			{
				dst->sign = POS;
				bignum_rawsub(a, b, dst);
			}
			else if(a->sign == NEG && cmp == B)		/* B > A, (B) + (-A) */
			{
				dst->sign = POS;
				bignum_rawsub(b, a, dst);
			}
			else if(a->sign == NEG && cmp == A)		/* A > B, (-A) + (B) */
			{
				dst->sign = NEG;
				bignum_rawsub(a, b, dst);
			}
			else if(a->sign == POS && cmp == B)		/* B > A, (A) + (-B) */
			{
				dst->sign = NEG;
				bignum_rawsub(b, a, dst);
			}
	}

	dst->ndigit = bignum_len(dst);
	return;
}

void bignum_sub(bignum_t *a, bignum_t *b, bignum_t *dst)
{
	/* Size checking */
	if(dst->size < (a->ndigit + b->ndigit))
		return;

	int8_t cmp = bignum_digitcmp(a, b);

	if(a->sign == b->sign)
	{
		if(cmp == A)				/* A > B, (+-A) - (+-B) */
		{
			bignum_rawsub(a, b, dst);
			dst->sign = POS;
		}
		else if(cmp == B)			/* B > A, (+-A) - (+-B) */
		{
			bignum_rawsub(b, a, dst);
			dst->sign = NEG;
		}
	}
	else
	{
		if(a->sign == POS && cmp == A)			/* A > B, (A) - (-B) */
		{
			dst->sign = POS;
			bignum_rawadd(a, b, dst);
		}
		else if(a->sign == POS && cmp == B)			/* A < B, (A) - (-B) */
		{
			dst->sign = NEG;
			bignum_rawsub(b, a, dst);
		}
		else if(a->sign == NEG && cmp == A)			/* A > B, (-A) - (B) */
		{
			dst->sign = NEG;
			bignum_rawadd(a, b, dst);
		}
		else if(a->sign == NEG && cmp == B)			/* A < B, (-A) + (B) */
		{
			dst->sign = NEG;
			bignum_rawadd(b, a, dst);
		}
	}

	dst->ndigit = bignum_len(dst);
	return;
}

int chartoint(char c)
{
	if(c >= '0' && c <= '9')
		return c - '0';
	else if(c == '-')
		return NEG_CHAR;
	else
		return INVAILD_CHAR;
}

char inttochar(int8_t c)
{
	if(c >= 0 && c <= 9)
		return c + '0';
	else
		return '*';
}

void bignum_strtonum(char *str, bignum_t *dst)
{
	int value=0;
	size_t end=strlen(str) - 1;
	unsigned int ptr=end;
	size_t strptr=0;
	while(strptr <= end)
	{
		value=chartoint(str[strptr++]);
		if(value == NEG_CHAR)
		{
			dst->sign = NEG;
			ptr--;
		}
		else if(value == INVAILD_CHAR)
		{
			puts("?INVAILD_CHAR");
			return;
		}
		else
		{
			dst->digit[ptr--] = (int8_t)value;
		}
	}
	dst->ndigit = bignum_len(dst);
}

void bignum_prints(char *str, size_t size, bignum_t *bignum)
{
	/* Both will be used as index */
	size_t len = bignum->ndigit;
	size_t end = len;
	unsigned int numptr=0;
	unsigned int strptr=len - 1;

	if(bignum->sign == NEG)
	{
		strptr++;
		len++;	/* Need one more character to store '-' */
		str[0] = '-';
	}
	if(len > size)
		return;		/* Not sufficient space */
	while(numptr < end)
		str[strptr--] = inttochar(bignum->digit[numptr++]);

	if(bignum->ndigit == 0)	/* If there's no digits */
	{
		str[++strptr]='0';
		str[++strptr] = 0x00;
	}
	return;
}

void bignum_dump(bignum_t *bignum)
{
	unsigned int ptr = bignum->size;
	fprintf(stderr, "SIZE=%u\n", bignum->size);
	fprintf(stderr, "SIGN=%s\n", bignum->sign ? "NEG" : "POS");	/* NEG == -1, POS == 0 */
	fprintf(stderr, "NDIGIT=%u\n", bignum->ndigit);
	while(ptr > 0)
	{
		--ptr;
		putc(inttochar(bignum->digit[ptr]), stderr);
		if((ptr & 0x3F) == 0)
			putc('\n', stderr);
	}
	putc('\n', stderr);
}

int main(void)
{
	char operator=0;
	char stra[LENGTH], strb[LENGTH], strc[LENGTH];
	bignum_t *a = bignum_init(LENGTH);
	bignum_t *b = bignum_init(LENGTH);
	bignum_t *c = bignum_init(LENGTH);

	while(scanf("%s %c %s\n", stra, &operator, strb) > 0)
	{
		bignum_strtonum(stra, a);
		bignum_strtonum(strb, b);
		bignum_dump(a);
		bignum_dump(b);
		switch(operator)
		{
			case '+':
				bignum_add(a, b, c);
				break;
			case '-':
				bignum_sub(a, b, c);
				break;
			case '*':
			case '/':
			default:
				return 0;
		}
		bignum_dump(c);
		bignum_prints(strc, LENGTH, c);
		puts(strc);
	}
	return 0;
}
