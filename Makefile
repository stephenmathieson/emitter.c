
CC ?= gcc
CFLAGS = -std=c99 -Wall -Wextra -Ideps
SRC = $(wildcard src/*.c)
SRC += $(wildcard deps/*/*.c)
OBJS = $(SRC:.c=.o)

test.o: CFLAGS += -Isrc
test: test.o $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)
	./$@

example: example.o $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) test.o

.PHONY: test clean
