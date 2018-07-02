CC=cc
C_FLAGS=-Wall -std=c99
SDL=SDL2
FRAMEWORKS=-framework SDL2
default: all
piru: piru.c
	$(CC) piru.c -o piru $(C_FLAGS) $(FRAMEWORKS)

all: piru

clean:
	rm piru