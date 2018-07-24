#include "direction.h"
enum DIRECTION get_direction_from_path_code(enum PATH_CODE code)
{
  switch (code)
  {
  case UP:
    return NORTH_EAST_2;
  case DOWN:
    return SOUTH_WEST_2;
  case LEFT:
    return NORTH_WEST_2;
  case RIGHT:
    return SOUTH_EAST_2;
  case UP_LEFT:
    return NORTH;
  case UP_RIGHT:
    return EAST;
  case DOWN_LEFT:
    return WEST;
  case DOWN_RIGHT:
    return SOUTH;
  default:
    return SOUTH;
  }
}

enum DIRECTION get_direction(const int x1, const int y1, const int x2, const int y2)
{
  static const double step = 360.0 / DIRECTION_COUNT;
  double angle = -atan2((double)y2 - (double)y1, (double)x2 - (double)x1) * 180 / M_PI;
  if (angle >= 0)
  {
    enum DIRECTION dir = EAST;
    double start = -step / 2;
    while (true)
    {

      if (angle < step + start)
      {
        return dir;
      }
      start += step;
      --dir;

      if (start > 1000.0)
      {
        break;
      }
    }
  }
  else
  {
    enum DIRECTION dir = EAST;
    double start = step / 2;
    while (true)
    {
      if (angle > (start - step))
      {
        return dir;
      }
      start -= step;
      dir++;
      if (dir >= DIRECTION_COUNT)
      {
        dir = SOUTH;
      }

      if (start < -1000.0)
      {
        break;
      }
    }
  }

  return SOUTH;
}