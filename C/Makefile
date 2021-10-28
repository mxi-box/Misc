CC	= cc
LIBS	= -lgmp	-I/usr/local/include -L/usr/local/lib
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC -std=c99 -pedantic -lm $(LIBS)
EXE	= epoch f-number hex_epoch neodate triangle diamond ackermann \
	  lcm prime gmpfib fastfib random as bswap pi approx circle
.PHONY: all

all: ${EXE}

countline:
	wc -l *.c
clean:
	-rm -f ${EXE} *.o
