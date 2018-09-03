CC	= clang
CFLAGS	= -Ofast -gfull -gdwarf-5 -Wall -Wextra -pipe -static -march=native -fPIE -flto
EXE	= big_alloc big_coredump coredump divide-zero epoch f-number hex_epoch neodate
all: $(EXE)

$(EXE): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	-rm -v ${EXE}
