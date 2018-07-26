#include "player.h"

Player gPlayer;

void player_do_destination_action()
{
  switch (gPlayer.destination_action)
  {
  case PLAYER_DESTINATION_ATTACK:
    gPlayer.state = PLAYER_ATTACKING;
    gPlayer.next_state = PLAYER_STANDING;
    break;
  case PLAYER_DESTINATION_STAND:
    gPlayer.state = PLAYER_STANDING;
    gPlayer.animation = ANIM_WARRIOR_IDLE;
    break;
  case PLAYER_DESTINATION_INTERACT_OBJECT:
    break;
  case PLAYER_DESTINATION_PICK_ITEM:
    break;
  default:
    break;
  }
}
void player_do_walk()
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
      gPlayer.point_in_path = 0;
      memset(gPlayer.path, -1, MAX_PATH_LENGTH);
      player_do_destination_action();
    }
  }
}

void update_player_movement()
{
  if (gPlayer.frames_since_walk >= gPlayer.walk_interval)
  {
    gPlayer.frames_since_walk = 0;
    player_do_walk();
  }
  else
  {
    gPlayer.frames_since_walk += gClock.delta;
  }
}

void update_player()
{
  switch (gPlayer.state)
  {
  case PLAYER_MOVING:
    update_player_movement();
    gPlayer.animation = ANIM_WARRIOR_WALK;
    break;
  case PLAYER_STANDING:
    gPlayer.animation = ANIM_WARRIOR_IDLE;
    break;
  case PLAYER_ATTACKING:
    gPlayer.animation = ANIM_WARRIOR_ATTACK;
    break;
  default:
    break;
  }
}