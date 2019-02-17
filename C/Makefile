CC	= cc
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC
EXE	= big_alloc big_coredump coredump divide-zero epoch f-number \
	  hex_epoch neodate triangle diamond ackermann lcm prime bignum

.PHONY: all

all: ${EXE}
countline:
	wc -l *.c
clean:
	-rm -f ${EXE}
