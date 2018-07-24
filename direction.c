#include "direction.h"
enum PLAYER_DIRECTION get_direction_from_path_code(enum PATH_CODE code)
{
  switch (code)
  {
  case UP:
    return PLAYER_NORTH_EAST_2;
  case DOWN:
    return PLAYER_SOUTH_WEST_2;
  case LEFT:
    return PLAYER_NORTH_WEST_2;
  case RIGHT:
    return PLAYER_SOUTH_EAST_2;
  case UP_LEFT:
    return PLAYER_NORTH;
  case UP_RIGHT:
    return PLAYER_EAST;
  case DOWN_LEFT:
    return PLAYER_WEST;
  case DOWN_RIGHT:
    return PLAYER_SOUTH;
  default:
    return PLAYER_SOUTH;
  }
}

enum PLAYER_DIRECTION get_direction(const int x1, const int y1, const int x2, const int y2)
{
  static const double step = 360.0 / PLAYER_DIRECTION_COUNT;
  double angle = -atan2((double)y2 - (double)y1, (double)x2 - (double)x1) * 180 / M_PI;
  if (angle >= 0)
  {
    enum PLAYER_DIRECTION dir = PLAYER_EAST;
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
    enum PLAYER_DIRECTION dir = PLAYER_EAST;
    double start = step / 2;
    while (true)
    {
      if (angle > (start - step))
      {
        return dir;
      }
      start -= step;
      dir++;
      if (dir >= PLAYER_DIRECTION_COUNT)
      {
        dir = PLAYER_SOUTH;
      }

      if (start < -1000.0)
      {
        break;
      }
    }
  }

  return PLAYER_SOUTH;
}