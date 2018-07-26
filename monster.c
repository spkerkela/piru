#include "monster.h"

extern Player gPlayer;
extern int created_monsters;
bool create_monster(const Point at)
{
  if (gDungeonBlockTable[at.y][at.x])
  {
    return false;
  }
  if (created_monsters >= MAX_MONSTERS)
  {
    return false;
  }
  Monster monster;
  monster.world_x = at.x;
  monster.world_y = at.y;
  monster.id = created_monsters;
  monster.level = 1;
  monster.state = MONSTER_STANDING;
  monster.next_state = MONSTER_NO_STATE;
  monster.direction = rand() % MONSTER_DIRECTION_COUNT;
  Point monster_target = {monster.world_x, monster.world_y};
  monster.attack_radius = 2.0;
  monster.aggro_radius = 10;
  monster.target = monster_target;
  memset(monster.path, -1, MAX_PATH_LENGTH);
  monster.max_hp = 20;
  monster.hp = monster.max_hp;
  monster.damage = 5;

  monster.animation_frame = 0;
  monster.walk_interval = 180;
  monster.frames_since_walk = 180;
  monster.animation = ANIM_SKELETON_IDLE;
  monster.frames_since_animation_frame = 0;
  monster.animation_intervals[ANIM_SKELETON_ATTACK] = 40;
  monster.animation_intervals[ANIM_SKELETON_IDLE] = 80;
  monster.animation_intervals[ANIM_SKELETON_WALK] = 70;
  gDungeonMonsterTable[at.y][at.x] = created_monsters;
  monsters[created_monsters++] = monster;
  return true;
}
void find_path_to_player(int id)
{
  memset(monsters[id].path, -1, MAX_PATH_LENGTH);
  monsters[id].point_in_path = 0;
  Point pos = {monsters[id].world_x, monsters[id].world_y};
  Point player_pos = {gPlayer.world_x, gPlayer.world_y};
  if (find_path(pos, player_pos, monsters[id].path))
  {
    monsters[id].state = MONSTER_MOVING;
    monsters[id].target = player_pos;
  }
}

double get_distance_to_player(int i)
{
  Point monster_point = {monsters[i].world_x, monsters[i].world_y};
  Point player_point = {gPlayer.world_x, gPlayer.world_y};
  return get_distance(monster_point, player_point);
}

void monster_do_walk(int i)
{
  if (get_distance_to_player(i) <= monsters[i].attack_radius)
  {
    monsters[i].direction = monster_get_direction8(monsters[i].world_x, monsters[i].world_y, gPlayer.world_x, gPlayer.world_y);
    monsters[i].state = MONSTER_ATTACKING;
    monsters[i].animation = ANIM_SKELETON_ATTACK;
    monsters[i].point_in_path = 0;
    monsters[i].animation_frame = 0;
    return;
  }
  char raw_code = monsters[i].path[monsters[i].point_in_path];
  if (raw_code != -1)
  {

    enum PATH_CODE code = (enum PATH_CODE)raw_code;
    Point direction = get_direction_from_path(code);
    Point check = {direction.x + monsters[i].world_x, direction.y + monsters[i].world_y};
    if (tile_is_blocked(check))
    {
      find_path_to_player(i);
      return;
    }

    monsters[i].direction = monster_get_direction_from_path_code(code);
    gDungeonMonsterTable[monsters[i].world_y][monsters[i].world_x] = -1;
    monsters[i].world_x += direction.x;
    monsters[i].world_y += direction.y;
    monsters[i].point_in_path++;
    gDungeonMonsterTable[monsters[i].world_y][monsters[i].world_x] = i;
    monsters[i].animation = ANIM_SKELETON_WALK;
  }
  else
  {
    monsters[i].state = MONSTER_STANDING;
    monsters[i].animation = ANIM_SKELETON_IDLE;
  }
}

void update_monster_movement(int i)
{
  if (gPlayer.world_x != monsters[i].target.x || gPlayer.world_y != monsters[i].target.y)
  {
    find_path_to_player(i);
  }
  if (monsters[i].frames_since_walk >= monsters[i].walk_interval)
  {
    monsters[i].frames_since_walk = 0;
    monster_do_walk(i);
  }
  else
  {
    monsters[i].frames_since_walk += gClock.delta;
  }
}

void update_monster(int id)
{
  switch (monsters[id].state)
  {
  case MONSTER_DEAD:
    monsters[id].animation = ANIM_SKELETON_DEAD;
    break;
  case MONSTER_MOVING:
    update_monster_movement(id);
    break;
  case MONSTER_STANDING:
    if (get_distance_to_player(id) < monsters[id].aggro_radius)
    {
      find_path_to_player(id);
    }
    else
    {
      monsters[id].animation = ANIM_SKELETON_IDLE;
    }
    break;
  case MONSTER_ATTACKING:
    monsters[id].next_state = MONSTER_STANDING;
    break;
  default:
    break;
  }
}