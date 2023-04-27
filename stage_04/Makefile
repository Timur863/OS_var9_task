CC = gcc
CFLAGS = -Wall

all: nav_data

nav_data: nav_data_stub.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f nav_data
