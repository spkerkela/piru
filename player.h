#ifndef _PLAYER_H
#define _PLAYER_H
#include <string.h>
#include "structs.h"
#include "enums.h"
#include "point.h"
#include "direction.h"
#include "time.h"

extern Player gPlayer;
void init_player();
void update_player();

#endif