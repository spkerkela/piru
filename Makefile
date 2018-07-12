default: piru
all: piru
UNAME := $(shell uname)
src=$(wildcard *.c)
obj=$(src:.c=.o)
dep = $(obj:.o=.d)
-include $(dep)   # include all dep files in the makefile
CC=gcc
C_FLAGS=-Wall -std=c99 -g
ifeq ($(UNAME), Darwin)
SDL=SDL2
FRAMEWORKS=-framework SDL2 -framework SDL2_image -framework SDL2_ttf

piru: $(obj)
	$(CC) -o $@ $^ $(C_FLAGS) $(FRAMEWORKS)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@	

.PHONY: all

.PHONY: clean
clean:
	rm -rf $(obj) $(dep) piru piru.dSYM
endif

ifeq ($(UNAME), MINGW32_NT-6.2)
COMPILER_FLAGS = -w -Wl,-subsystem,windows
INCLUDE_PATHS = -IC:/mingw_dev_lib/include/SDL2 
LIBRARY_PATHS = -LC:/mingw_dev_lib/lib
LINKER_FLAGS =-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
piru: piru.c
	$(CC) -o piru piru.c $(COMPILER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) 
all: piru
endif
