CC = gcc
CFLAGS = -Wall

all: nav_data

nav_data: nav_data.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f nav_data
