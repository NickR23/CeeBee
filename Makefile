C=gcc
CFLAGS=-Wall -g
output:
	$(C) $(CFLAGS) -o gba main.c
