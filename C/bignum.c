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

#define _BASE	10
#define _LENGTH	512

#define _POS 0	/* Positive */
#define _NEG -1	/* Negative */

#define _NEG_CHAR	-1
#define _INVAILD_CHAR	-2

#define _A	1
#define _B	-1
#define _SAME	0

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
		if(bignum->digit[ptr] >= _BASE)					\
		{								\
			bignum->digit[ptr + 1] += bignum->digit[ptr] / _BASE;	\
			bignum->digit[ptr] %= _BASE;				\
		}

#define BORROW(bignum, ptr)	\
		if(bignum->digit[ptr] < 0)					\
		{								\
			bignum->digit[ptr + 1] -= bignum->digit[ptr] / _BASE + 1;\
			bignum->digit[ptr] += _BASE;				\
		}

/* Panic Codes */
#define P_SIZE	0
#define P_INVAILD_CHAR	1

void panic(int type)
{
	switch(type)
	{
		case P_SIZE:
			fputs("?SIZE\n", stderr);
			break;
		case P_INVAILD_CHAR:
			fputs("?INVAILD_CHAR\n", stderr);
			break;
		default:
			exit(8);
	}
	exit(type);
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
	free((*bignum)->digit);
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
			return _A;
		else if(src1->ndigit < src2->ndigit)
			return _B;
	}
	else
	{
		for(ptr = src1->ndigit - 1; ptr >= 0; ptr--)
		{
			if(src1->digit[ptr] > src2->digit[ptr])
				return _A;
			else if(src1->digit[ptr] < src2->digit[ptr])
				return _B;
		}
	}
	/* They are completely equal */
	return _SAME;
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
		if(src1->sign == _POS)
			return _A;
		else if(src2->sign == _POS)
			return _B;
	}
	return 0;
}


void bignum_setzero(bignum_t *bignum)
{
	unsigned int ptr = bignum->ndigit;
	bignum->sign = _POS;
	while((ptr--) > 0)
		bignum->digit[ptr] = 0;
	bignum->ndigit=0;
}

void bignum_copy(bignum_t *src, bignum_t *dst)
{
	free(dst->digit);
	memcpy(dst, src, sizeof(bignum_t));
	dst->digit = calloc(src->size, sizeof(int8_t));
	memcpy(dst->digit, src->digit, src->size);
	return;
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

void bignum_rawdec(bignum_t *dst, bignum_t *dec)
{
	unsigned int ptr=0;

	while(ptr < dec->ndigit)
	{
		dst->digit[ptr] -= dec->digit[ptr];
		BORROW(dst, ptr);
		ptr++;
	}

	dst->ndigit = bignum_len(dst);
}

void bignum_add(bignum_t *a, bignum_t *b, bignum_t *dst)
{
	/* Size checking */
	if(dst->size < (a->ndigit + b->ndigit))
		panic(P_SIZE);

	if(a->sign == b->sign)
	{
		bignum_rawadd(a, b, dst);
		dst->sign = a->sign;
	}
	else
	{
			int8_t cmp = bignum_digitcmp(a, b);
			if(a->sign == _POS && cmp == _A)		/* A > B, (A) + (B) */
			{
				dst->sign = _POS;
				bignum_rawsub(a, b, dst);
			}
			else if(a->sign == _NEG && cmp == _B)		/* B > A, (B) + (-A) */
			{
				dst->sign = _POS;
				bignum_rawsub(b, a, dst);
			}
			else if(a->sign == _NEG && cmp == _A)		/* A > B, (-A) + (B) */
			{
				dst->sign = _NEG;
				bignum_rawsub(a, b, dst);
			}
			else if(a->sign == _POS && cmp == _B)		/* B > A, (A) + (-B) */
			{
				dst->sign = _NEG;
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
		panic(P_SIZE);

	int8_t cmp = bignum_digitcmp(a, b);

	if(a->sign == b->sign)
	{
		if(cmp == _A)				/* A > B, (+-A) - (+-B) */
		{
			bignum_rawsub(a, b, dst);
			dst->sign = _POS;
		}
		else if(cmp == _B)			/* B > A, (+-A) - (+-B) */
		{
			bignum_rawsub(b, a, dst);
			dst->sign = _NEG;
		}
	}
	else
	{
		if(a->sign == _POS && cmp == _A)			/* A > B, (A) - (-B) */
		{
			dst->sign = _POS;
			bignum_rawadd(a, b, dst);
		}
		else if(a->sign == _POS && cmp == _B)			/* A < B, (A) - (-B) */
		{
			dst->sign = _NEG;
			bignum_rawsub(b, a, dst);
		}
		else if(a->sign == _NEG && cmp == _A)			/* A > B, (-A) - (B) */
		{
			dst->sign = _NEG;
			bignum_rawadd(a, b, dst);
		}
		else if(a->sign == _NEG && cmp == _B)			/* A < B, (-A) + (B) */
		{
			dst->sign = _NEG;
			bignum_rawadd(b, a, dst);
		}
	}

	dst->ndigit = bignum_len(dst);
	return;
}

void bignum_rawmul(bignum_t *large, bignum_t *small, bignum_t *dst)
{
	unsigned int i=0, j=0;

	bignum_setzero(dst);
	for(i=0; i < small->ndigit; i++)
	{
		for(j=0; j < large->ndigit; j++)
		{
			dst->digit[j+i] +=  small->digit[i] * large->digit[j];
			CARRY(dst, j+i);
		}
	}

	dst->ndigit = bignum_len(dst);
	return;
}

void bignum_mul(bignum_t *a, bignum_t *b, bignum_t *dst)
{
	if((a->ndigit + a->ndigit) > dst->size)
		panic(P_SIZE);

	bignum_rawmul(a, b, dst);
	dst->sign = a->sign ^ b->sign;
	return;
}

/* Left Shift */
void bignum_lshift(bignum_t *bignum, unsigned int ndigit)
{
	unsigned int size = bignum->size;
	unsigned int ptr = size;

	while(ptr != 0)
	{
		--ptr;
		if((ptr + ndigit) < size)
			bignum->digit[ptr + ndigit] = bignum->digit[ptr];
	}

	/* Clear moved out digits */
	for(ptr = 0; ptr < ndigit; ptr++)
		bignum->digit[ptr] = 0;

	bignum->ndigit = bignum_len(bignum);
	return;
}

/* Right Shift */
void bignum_rshift(bignum_t *bignum, unsigned int ndigit)
{
	unsigned int size = bignum->size;
	unsigned int ptr = 0;

	while(ptr != size)
	{
		if(ptr <= size - ndigit)
			bignum->digit[ptr] = bignum->digit[ptr + ndigit];
		ptr++;
	}

	/* Clear moved out digits */
	for(ptr = size - 1; ptr >= size - ndigit; ptr--)
		bignum->digit[ptr] = 0;

	bignum->ndigit = bignum_len(bignum);
	return;
}

unsigned long long int intpow(unsigned long long int x, unsigned long long int y)
{
	unsigned long long int count=y - 1, temp=1;

	while(count > 0)
	{
		temp *= x;
		count++;
	}

	return temp;
}

/* Division */
void bignum_rawdiv(bignum_t *src, bignum_t *div, bignum_t *dst)
{
	unsigned int shift=0;

	bignum_setzero(dst);
	bignum_t *temp = bignum_init(_LENGTH);
	bignum_t *subtract = bignum_init(_LENGTH);
	bignum_copy(src, temp);
	bignum_copy(div, subtract);

	while(bignum_digitcmp(temp, subtract) != _B)
	{
		bignum_lshift(subtract, 1);
		shift++;
	}

	while(shift > 0)
	{
		bignum_rshift(subtract, 1);
		shift--;
		while(bignum_digitcmp(temp, subtract) != _B)
		{
			bignum_rawdec(temp, subtract);
			dst->digit[shift]++;
		}
	}

	dst->ndigit = bignum_len(dst);
	bignum_destroy(&temp);
	bignum_destroy(&subtract);
}

void bignum_rawmod(bignum_t *src, bignum_t *div, bignum_t *dst)
{
	unsigned int shift=0;

	bignum_t *subtract = bignum_init(_LENGTH);
	bignum_copy(src, dst);
	bignum_copy(div, subtract);

	while(bignum_digitcmp(dst, subtract) != _B)
	{
		bignum_lshift(subtract, 1);
		shift++;
	}

	while(shift > 0)
	{
		bignum_rshift(subtract, 1);
		shift--;
		while(bignum_digitcmp(dst, subtract) != _B)
			bignum_rawdec(dst, subtract);
	}

	dst->ndigit = bignum_len(dst);
	bignum_destroy(&subtract);
}

void bignum_div(bignum_t *a, bignum_t *b, bignum_t *dst)
{
	if(a->ndigit > dst->size)
		panic(P_SIZE);

	bignum_rawdiv(a, b, dst);
	dst->sign = a->sign ^ b->sign;
	return;
}

void bignum_mod(bignum_t *a, bignum_t *b, bignum_t *dst)
{
	if(b->ndigit > dst->size)
		panic(P_SIZE);

	bignum_rawmod(a, b, dst);
	dst->sign = a->sign ^ b->sign;
	return;
}

int chartoint(char c)
{
	if(c >= '0' && c <= '9')
		return c - '0';
	else if(c == '-')
		return _NEG_CHAR;
	else
		return _INVAILD_CHAR;
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

	bignum_setzero(dst);
	while(strptr <= end)
	{
		value=chartoint(str[strptr++]);
		if(value == _NEG_CHAR)
		{
			dst->sign = _NEG;
			ptr--;
		}
		else if(value == _INVAILD_CHAR)
		{
			panic(P_INVAILD_CHAR);
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

	if(bignum->sign == _NEG)
	{
		strptr++;
		len++;	/* Need one more character to store '-' */
		str[0] = '-';
	}
	if(len > size)
		return;		/* Not sufficient space */

	str[len] = '\0'; /* Add a NUL after these digits */
	while(numptr < end)
		str[strptr--] = inttochar(bignum->digit[numptr++]);

	if(bignum->ndigit == 0)	/* If there's no digits */
	{
		str[0]='0';
		str[1] = 0x00;
	}
	return;
}

void bignum_dump(bignum_t *bignum, char *name)
{
	unsigned int ptr = bignum->size;
	fprintf(stderr, "=== DUMP OF \"%s\" ===\n", name);
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
	fputs("=== END ===\n\n", stderr);
}

int main(void)
{
	int i=0;
	char operator=0;
	char stra[_LENGTH], strb[_LENGTH], strc[_LENGTH];

	/* Because of the way cache works, this thing's performance might be somewhat poor */
	for(i=0; i < _LENGTH; i++)
	{
		stra[i] = 0;
		strb[i] = 0;
		strc[i] = 0;
	}

	bignum_t *a = bignum_init(_LENGTH);
	bignum_t *b = bignum_init(_LENGTH);
	bignum_t *c = bignum_init(_LENGTH);

	while(scanf("%s %c %s", stra, &operator, strb) > 0)
	{
		bignum_strtonum(stra, a);
		bignum_strtonum(strb, b);
		bignum_dump(a, "A");
		bignum_dump(b, "B");
		switch(operator)
		{
			case '+':
				bignum_add(a, b, c);
				break;
			case '-':
				bignum_sub(a, b, c);
				break;
			case '*':
				bignum_mul(a, b, c);
				break;
			case '/':
				bignum_div(a, b, c);
				break;
			case '%':
				bignum_mod(a, b, c);
				break;
			default:
				return 0;
		}
		bignum_dump(c, "C");
		bignum_prints(strc, _LENGTH, c);
		puts(strc);
	}
	return 0;
}
