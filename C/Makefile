CC	= clang
CFLAGS	= -Ofast -gfull -gdwarf-5 -Wall -Wextra -pipe -static -march=native -fPIE -flto

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

clean:
	-rm -v ${EXE}
