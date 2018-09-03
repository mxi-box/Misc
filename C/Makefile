CC	= clang
CFLAGS	= -Ofast -gfull -gdwarf-5 -Wall -Wextra -pipe -static -march=native -fPIE -flto

big_alloc:
	$(CC) $(CFLAGS) -o $@ $<

big_coredump:
	$(CC) $(CFLAGS) -o $@ $<

coredump:
	$(CC) $(CFLAGS) -o $@ $<

divide-zero:
	$(CC) $(CFLAGS) -o $@ $<

epoch:
	$(CC) $(CFLAGS) -o $@ $<

f-number:
	$(CC) $(CFLAGS) -o $@ $<

hex_epoch:
	$(CC) $(CFLAGS) -o $@ $<

neodate:
	$(CC) $(CFLAGS) -o $@ $<

triangle:
	$(CC) $(CFLAGS) -o $@ $<

clean:
	-rm -v ${EXE}
