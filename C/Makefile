CC	= cc
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC
EXE	= epoch f-number hex_epoch neodate triangle diamond ackermann lcm prime gmpfib fastfib random as
LIBGMP	= -lgmp	-I/usr/local/include -L/usr/local/lib
.PHONY: all

all: ${EXE}

gmpfib:
	$(CC) $(CFLAGS) -o gmpfib gmpfib.c $(LIBGMP)

fastfib:
	$(CC) $(CFLAGS) -o fastfib fastfib.c $(LIBGMP)

as:
	$(CC) $(CFLAGS) -o as as.c $(LIBGMP)

countline:
	wc -l *.c
clean:
	-rm -f ${EXE}
