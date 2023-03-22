SRC:=
include config.mk

INCLUDES?=
INCLUDES+=-I src

override CFLAGS?=-Wall -std=c99

include lib/.dep/config.mk

override CFLAGS+=$(INCLUDES)
override CFLAGS+=-D_DEFAULT_SOURCE

OBJ=$(SRC:.c=.o)

BIN=\
	benchmark \
	test

default: $(BIN)

$(BIN): $(OBJ) $(BIN:=.o)
	$(CC) $(CFLAGS) $(OBJ) $@.o -o $@

.PHONY: clean
clean:
	rm -f $(OBJ)
	rm -f $(BIN:=.o)
	rm -f test
