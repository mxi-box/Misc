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

#define LENGTH 256

#define POS 0	/* Positive */
#define NEG -1	/* Negative */

#define NEG_CHAR	-1
#define INVAILD_CHAR	-2

struct _bignum
{
	unsigned int size;
	unsigned int ndigit; /* Number of digits */
	int8_t sign;
	int8_t *digit;	/* Signed because it will be easier to handle subtraction */
};

/* Digits is stored as little endian BCD (One digit per byte, so wasteful?) */

typedef struct _bignum bignum_t;

#define CARRY(bignum, ptr)	\
		if(bignum->digit[ptr] >= 10)					\
		{								\
			bignum->digit[ptr + 1] = bignum->digit[ptr] / 10;	\
			bignum->digit[ptr] %= 10;				\
		}

#define BORROW(bignum, ptr)	\
		if(bignum->digit[ptr] < 0)					\
		{								\
			bignum->digit[ptr + 1]--;				\
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

int8_t bignum_cmp(bignum_t *src1, bignum_t *src2)
{
	int ptr = 0;
	if(src1->ndigit != src2->ndigit)
	{
		if(src1->ndigit > src2->ndigit)
			return 1;
		else if(src1->ndigit < src2->ndigit)
			return -1;
		else
			return 0;
	}
	else
	{
		for(ptr = src1->ndigit - 1; ptr >= 0; ptr--)
		{
			if(src1->digit[ptr] > src2->digit[ptr])
				return 1;
			else if(src1->digit[ptr] < src2->digit[ptr])
				return -1;
		}
		/* They are completely equal */
		return 0;
	}
}

void bignum_setzero(bignum_t *bignum)
{
	unsigned int ptr = bignum->ndigit;
	while((ptr--) > 0)
		bignum->digit[ptr] = 0;
	bignum->ndigit=0;
}

void bignum_rawadd(bignum_t *op1, bignum_t *op2, bignum_t *dst)
{
	unsigned int ptr=0;
	bignum_t *src1, *src2;

	/* Keep src1 the longer one, src2 the shorter one */
	if(op2->ndigit > op1->ndigit)
	{
		src1 = op2;
		src2 = op1;
	}
	else
	{
		src1 = op1;
		src2 = op2;
	}

	/* Invaild Operation Checking */
	if(dst->size < (src1->ndigit + 1))
		goto end;

	while(ptr < src2->ndigit)
	{
		dst->digit[ptr] += src1->digit[ptr] + src2->digit[ptr];
		CARRY(dst, ptr);
		ptr++;
	}

	while(ptr < src1->ndigit)	/* Add rest of src1 it to dst */
	{
		dst->digit[ptr] += src1->digit[ptr];
		CARRY(dst, ptr);
		ptr++;
	}
	dst->ndigit = ptr;	/* Length of dst */
end:
	return;
}

void bignum_rawsub(bignum_t *op1, bignum_t *op2, bignum_t *dst)
{
	unsigned int ptr=0;
	bignum_t *src1, *src2;

	/* Keep src1 the larger one */
	if(bignum_cmp(op1, op2) == 1)
	{
		src1 = op1;
		src2 = op2;
	}
	else
	{
		src1 = op2;
		src2 = op1;
	}

	while(ptr < src2->ndigit)
	{
		/* Stub */
	}
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
	int ptr=0;
	int value=0;
	size_t endptr=strlen(str) - 1;
	for(ptr=endptr; ptr >= 0; ptr--)
	{
		value=chartoint(str[ptr]);
		if(value == NEG_CHAR)
			dst->sign = NEG;
		else if(value == INVAILD_CHAR)
			puts("?INVAILD_CHAR");
		else
			dst->digit[endptr - ptr] = (int8_t)value;
	}
	dst->ndigit = strlen(str) - dst->sign;
}

void bignum_prints(char *str, size_t size, bignum_t *bignum)
{
	/* Both will be used as index */
	size_t len = bignum->ndigit;
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
	while(numptr < len)
	{
		str[strptr] = inttochar(bignum->digit[numptr]);
		strptr--;
		numptr++;
	}
}

void bignum_dump(bignum_t *bignum)
{
	unsigned int ptr = bignum->size;
	fprintf(stderr, "SIZE=%u\n", bignum->size);
	fprintf(stderr, "SIGN=%c\n", "+-"[bignum->sign]);
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
	}
	return 0;
}
