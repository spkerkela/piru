#ifndef _DIRECTION_H
#define _DIRECTION_H
#include <math.h>
#include <stdbool.h>
#include "enums.h"

enum PLAYER_DIRECTION get_direction_from_path_code(enum PATH_CODE code);

enum PLAYER_DIRECTION get_direction(const int x1, const int y1, const int x2, const int y2);

#endif