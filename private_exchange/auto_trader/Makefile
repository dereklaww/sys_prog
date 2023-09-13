TARGET = test

CC=gcc
CFLAGS=-Wall -Wuninitialized -Wmaybe-uninitialized -Werror -Wvla -O0 -std=c11 -g -fsanitize=address,leak
LDFLAGS=-lm
BINARIES=pe_trader

SRC        = test.c
OBJ        = $(SRC:.c=.o)

all: $(BINARIES)

test:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

.PHONY: clean
clean:
	rm -f $(BINARIES)

	



