CC	= cc
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC
EXE	= epoch f-number hex_epoch neodate triangle diamond ackermann lcm prime gmpfib fastfib random as
LIBGMP	= -lgmp
.PHONY: all

all: ${EXE}

gmpfib: gmpfib.o
	$(CC) $(CFLAGS) -o gmpfib gmpfib.o $(LIBGMP)

fastfib: fastfib.o
	$(CC) $(CFLAGS) -o fastfib fastfib.o $(LIBGMP)

as:	as.o
	$(CC) $(CFLAGS) -o as as.o $(LIBGMP)

countline:
	wc -l *.c
clean:
	-rm -f ${EXE}
