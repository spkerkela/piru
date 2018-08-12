#ifndef _POINT_H
#define _POINT_H
#include "constants.h"
#include "enums.h"
#include "structs.h"
#include <math.h>

Point cartesian_to_isometric(const Point cartesian_point);

Point isometric_to_cartesian(const Point isometric_point);

Point get_tile_coordinates(const Point cartesian_point);

Point get_direction_from_path(enum PATH_CODE code);

Point get_direction_from_player_direction(enum PLAYER_DIRECTION direction);

Point get_direction_to_point(const Point from, const Point to);
void get_normalized(const Point point, double *x, double *y);

double get_distance(const Point a, const Point b);

bool point_equal(const Point a, const Point b);

#endif