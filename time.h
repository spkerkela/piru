#ifndef _TIME_H
#define _TIME_H
#include "enums.h"
#include "sdl2.h"
#include "stdint.h"
#include "structs.h"

extern Clock gClock;
void init_clock();
void tick();

#endif