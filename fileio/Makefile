CC = gcc
CFLAGS = -Wall -Wextra -Ilib

SRC = seek_io.c lib/error_functions.c lib/get_num.c

OUT = seek_io

all: $(OUT)


$(OUT): $(SRC)
	$(CC) $(CCFLAGS) -o $@ $(SRC)

clean:
	rm -f $(OUT)
