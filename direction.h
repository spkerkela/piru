#ifndef _DIRECTION_H
#define _DIRECTION_H
#include "enums.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

enum PLAYER_DIRECTION player_get_direction_from_path_code(enum PATH_CODE code);
enum MONSTER_DIRECTION
monster_get_direction_from_path_code(enum PATH_CODE code);

enum PLAYER_DIRECTION get_direction(const int x1, const int y1, const int x2,
                                    const int y2);
enum PLAYER_DIRECTION player_get_direction8(const int x1, const int y1,
                                            const int x2, const int y2);
enum MONSTER_DIRECTION monster_get_direction8(const int x1, const int y1,
                                              const int x2, const int y2);

#endif