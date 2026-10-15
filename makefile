CFLAGS = -std=c17 -Wall -Wextra

all:
	gcc $(CFLAGS) chip8.c -o chip8 