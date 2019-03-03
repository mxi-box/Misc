CC	= cc
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC
EXE	= epoch f-number hex_epoch neodate triangle diamond ackermann lcm prime gmpfib fastfib

.PHONY: all

all: ${EXE}

gmpfib: gmpfib.o
	$(CC) $(CFLAGS) -o gmpfib gmpfib.o -lgmp

fastfib: fastfib.o
	$(CC) $(CFLAGS) -o fastfib fastfib.o -lgmp

countline:
	wc -l *.c
clean:
	-rm -f ${EXE}
