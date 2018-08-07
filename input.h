#ifndef _INPUT_H
#define _INPUT_H
#include "enums.h"
#include "player.h"
#include "point.h"
#include "sdl2.h"
#include "structs.h"

extern MouseCursor gCursor;

extern bool gShiftIsDown;
extern Point gSelectedTile;

void handle_cursor();
void update_input();

#endif