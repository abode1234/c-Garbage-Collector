CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/gc.c cmd/main.c
OUT = main

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
