CC = clang
CFLAGS = -Wall -lSDL2

all: run

build: main.c render.c
	$(CC) $(CFLAGS) main.c render.c -o main

run: build
	./main

clean:
	rm -f main
