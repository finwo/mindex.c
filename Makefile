SRC:=
SRC+=$(wildcard src/*.c)
SRC+=test.c

INCLUDES?=
INCLUDES+=-I src

override CFLAGS?=-Wall -g -O2

include lib/.dep/config.mk

override CFLAGS+=$(INCLUDES)

OBJ=$(SRC:.c=.o)

test: $(OBJ)
	$(CC) $(CFLAGS) $(SRC) -o $@

.PHONY: clean
clean:
	rm -f $(OBJ)
	rm -f test
