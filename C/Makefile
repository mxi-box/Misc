CC	= cc
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC -std=c99 -pedantic
EXE	= epoch f-number hex_epoch neodate triangle diamond ackermann lcm prime gmpfib fastfib random as bswap pi ip approx
LIBGMP	= -lgmp	-I/usr/local/include -L/usr/local/lib
.PHONY: all

all: ${EXE}

gmpfib: gmpfib.o
	$(CC) $(CFLAGS) -o gmpfib gmpfib.o $(LIBGMP)

fastfib: fastfib.o
	$(CC) $(CFLAGS) -o fastfib fastfib.o $(LIBGMP)

as: as.o
	$(CC) $(CFLAGS) -o as as.o $(LIBGMP)

approx: approx.o
	$(CC) $(CFLAGS) -o approx approx.o $(LIBGMP)

countline:
	wc -l *.c
clean:
	-rm -f ${EXE} *.o
