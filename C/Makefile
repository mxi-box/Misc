CC	= cc
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC -std=c99 -pedantic
EXE	= epoch f-number hex_epoch neodate triangle diamond ackermann lcm prime gmpfib fastfib random as bswap
LIBGMP	= -lgmp	-I/usr/local/include -L/usr/local/lib
.PHONY: all

all: ${EXE}

gmpfib: gmpfib.o
	$(CC) $(CFLAGS) -o gmpfib gmpfib.c $(LIBGMP)

fastfib: fastfib.o
	$(CC) $(CFLAGS) -o fastfib fastfib.c $(LIBGMP)

as: as.o
	$(CC) $(CFLAGS) -o as as.c $(LIBGMP)

countline:
	wc -l *.c
clean:
	-rm -f ${EXE} *.o
