#include "player.h"

Player gPlayer;

void update_player_movement()
{
  char raw_code = gPlayer.path[gPlayer.point_in_path];
  if (raw_code != -1)
  {
    enum PATH_CODE code = (enum PATH_CODE)raw_code;
    gPlayer.point_in_path++;
    Point direction = get_direction_from_path(code);

    gPlayer.direction = player_get_direction_from_path_code(code);
    gPlayer.world_x += direction.x;
    gPlayer.world_y += direction.y;
    if (gPlayer.world_x == gPlayer.target.x && gPlayer.world_y == gPlayer.target.y)
    {
      gPlayer.state = PLAYER_STANDING;
      gPlayer.point_in_path = 0;
    }
  }
}

void update_player()
{
  switch (gPlayer.state)
  {
  case PLAYER_MOVING:
    update_player_movement();
    break;
  default:
    break;
  }
}