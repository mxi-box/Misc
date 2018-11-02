CC	= cc
CFLAGS	= -O2 -g -Wall -Wextra -pipe -fPIE -fPIC
EXE	= big_alloc big_coredump coredump divide-zero epoch f-number hex_epoch neodate triangle diamond ackermann

all: ${EXE}

big_alloc:
	$(CC) $(CFLAGS) -o $@ $@.c

big_coredump:
	$(CC) $(CFLAGS) -o $@ $@.c

coredump:
	$(CC) $(CFLAGS) -o $@ $@.c

divide-zero:
	$(CC) $(CFLAGS) -o $@ $@.c

epoch:
	$(CC) $(CFLAGS) -o $@ $@.c

f-number:
	$(CC) $(CFLAGS) -o $@ $@.c

hex_epoch:
	$(CC) $(CFLAGS) -o $@ $@.c

neodate:
	$(CC) $(CFLAGS) -o $@ $@.c

triangle:
	$(CC) $(CFLAGS) -o $@ $@.c

diamond:
	$(CC) $(CFLAGS) -o $@ $@.c

ackermann:
	$(CC) $(CFLAGS) -o $@ $@.c

clean:
	-rm -f ${EXE}
