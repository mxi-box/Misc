CC	= cc
LIBS	= -fopenmp -lgmp -lmpfr -I/usr/local/include -L/usr/local/lib
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC -std=c99 -pedantic -lm $(LIBS)
EXE	= epoch hex_epoch neodate ackermann \
	  prime fastfib approx circle simpleturtle pi-omp pi-fast us ns notfib \
	  e e-normal
.PHONY: all

all: ${EXE}

countline:
	wc -l *.c
clean:
	-rm -f ${EXE} *.o
