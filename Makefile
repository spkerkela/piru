UNAME := $(shell uname)
CC=gcc
C_FLAGS=-Wall -std=c99
ifeq ($(UNAME), Darwin)
SDL=SDL2
FRAMEWORKS=-framework SDL2 -framework SDL2_image
default: all
piru: piru.c
	$(CC) piru.c -o piru $(C_FLAGS) $(FRAMEWORKS)

all: piru

clean:
	rm piru
endif

ifeq ($(UNAME), MINGW32_NT-6.2)
COMPILER_FLAGS = -w -Wl,-subsystem,windows
INCLUDE_PATHS = -IC:/mingw_dev_lib/include/SDL2 
LIBRARY_PATHS = -LC:/mingw_dev_lib/lib
LINKER_FLAGS =-lmingw32 -lSDL2main -lSDL2 -lSDL2_image
piru: piru.c
	$(CC) -o piru piru.c $(COMPILER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) 
all: piru
endif