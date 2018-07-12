#ifndef _POINT_H
#define _POINT_H
#include "constants.h"
#include "enums.h"
#include "structs.h"

Point cartesian_to_isometric(const Point cartesian_point);

Point isometric_to_cartesian(const Point isometric_point);

Point get_tile_coordinates(const Point cartesian_point);

#endif