#include "monster.h"

extern Player gPlayer;
void update_monster_movement(int i)
{
  char raw_code = monsters[i].path[monsters[i].point_in_path];
  if (raw_code != -1)
  {
    enum PATH_CODE code = (enum PATH_CODE)raw_code;
    monsters[i].point_in_path++;
    Point direction = get_direction_from_path(code);

    monsters[i].direction = monster_get_direction_from_path_code(code);
    monsters[i].world_x += direction.x;
    monsters[i].world_y += direction.y;
    if (monsters[i].world_x == monsters[i].target.x && monsters[i].world_y == monsters[i].target.y)
    {
      monsters[i].state = MONSTER_STANDING;
      monsters[i].point_in_path = 0;
    }
  }
}

void find_path_to_player(int id)
{
  memset(monsters[id].path, -1, MAX_PATH_LENGTH);
  Point pos = {monsters[id].world_x, monsters[id].world_y};
  Point player_pos = {gPlayer.world_x, gPlayer.world_y};
  if (find_path(pos, player_pos, monsters[id].path))
  {
    monsters[id].state = MONSTER_MOVING;
    monsters[id].point_in_path = 0;
    monsters[id].target = player_pos;
  }
}

void update_monster(int id)
{
  switch (monsters[id].state)
  {
  case MONSTER_MOVING:
    update_monster_movement(id);
    break;
  case MONSTER_STANDING:
    find_path_to_player(id);
    break;
  default:
    break;
  }
}