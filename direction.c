#include "direction.h"
enum PLAYER_DIRECTION player_get_direction_from_path_code(enum PATH_CODE code)
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

enum MONSTER_DIRECTION monster_get_direction_from_path_code(enum PATH_CODE code)
{
  switch (code)
  {
  case UP:
    return MONSTER_NORTH_EAST;
  case DOWN:
    return MONSTER_SOUTH_WEST;
  case LEFT:
    return MONSTER_NORTH_WEST;
  case RIGHT:
    return MONSTER_SOUTH_EAST;
  case UP_LEFT:
    return MONSTER_NORTH;
  case UP_RIGHT:
    return MONSTER_EAST;
  case DOWN_LEFT:
    return MONSTER_WEST;
  case DOWN_RIGHT:
    return MONSTER_SOUTH;
  default:
    return MONSTER_SOUTH;
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

enum PLAYER_DIRECTION player_get_direction8(const int x1, const int y1, const int x2, const int y2)
{

  int x, y;
  x = x2 - x1;
  y = y2 - y1;

  int s1 = x * 414 + y * 1000 > 0 ? 0 : 1;
  int s2 = y * 414 + x * 1000 > 0 ? 0 : 1;
  int s3 = y * 414 - x * 1000 < 0 ? 0 : 1;
  int s4 = x * 414 - y * 1000 > 0 ? 0 : 1;

  int segment = 4 * s4 + 2 * (s2 ^ s4) + (s1 ^ s2 ^ s3 ^ s4);
  printf("%d segment\n", segment);
  switch (segment)
  {
  case 0:
    return PLAYER_SOUTH_EAST_2;
  case 1:
    return PLAYER_EAST;
  case 2:
    return PLAYER_NORTH_EAST_2;
  case 3:
    return PLAYER_NORTH;
  case 4:
    return PLAYER_NORTH_WEST_2;
  case 5:
    return PLAYER_WEST;
  case 6:
    return PLAYER_SOUTH_WEST_2;
  case 7:
    return PLAYER_SOUTH;
  default:
    break;
  }
  return PLAYER_SOUTH;
}