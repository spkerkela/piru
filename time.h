#ifndef _TIME_H
#define _TIME_H
#include "sdl2.h"
#include "enums.h"
#include "structs.h"
#include "stdint.h"

extern Clock gClock;
void init_clock();
void tick();

#endif