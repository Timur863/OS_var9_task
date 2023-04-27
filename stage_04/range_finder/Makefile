CC = gcc
CFLAGS = -Wall

all: range_finder

range_finder: range_finder_stub.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f range_finder
