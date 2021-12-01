CC	= cc
LIBS	= -lgmp	-I/usr/local/include -L/usr/local/lib
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC -std=c99 -pedantic -lm $(LIBS)
EXE	= epoch hex_epoch neodate ackermann \
	  prime fastfib approx circle simpleturtle
.PHONY: all

all: ${EXE}

countline:
	wc -l *.c
clean:
	-rm -f ${EXE} *.o
