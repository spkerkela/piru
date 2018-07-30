#include "player.h"

Player gPlayer;

void init_player()
{
  gPlayer.world_x = 1;
  gPlayer.world_y = 1;
  gPlayer.next_x = -1;
  gPlayer.next_y = -1;
  gPlayer.pixel_x = 0;
  gPlayer.pixel_y = 0;
  gPlayer.attack_radius = 2.0;
  gPlayer.current_game_level = 0;
  gPlayer.direction = PLAYER_SOUTH;
  gPlayer.point_in_path = 0;
  gPlayer.state = PLAYER_STANDING;
  gPlayer.next_state = PLAYER_NO_STATE;
  gPlayer.destination_action = PLAYER_DESTINATION_NONE;
  gPlayer.max_hp = 1000;
  gPlayer.hp = gPlayer.max_hp;
  gPlayer.max_mana = 50;
  gPlayer.mana = 22;
  gPlayer.damage = 10;

  Point target = {-1, -1};
  gPlayer.target = target;
  Point new_target = {-1, -1};
  gPlayer.new_target = new_target;

  gPlayer.target_monster_id = -1;
  gPlayer.animation = ANIM_WARRIOR_IDLE;
  gPlayer.walk_interval = 300;
  gPlayer.frames_since_walk = 0;
  gPlayer.frames_since_animation_frame = 0;
  gPlayer.animation_intervals[ANIM_WARRIOR_ATTACK] = 20;
  gPlayer.animation_intervals[ANIM_WARRIOR_WALK] = 80;
  gPlayer.animation_intervals[ANIM_WARRIOR_IDLE] = 100;
}

void player_do_destination_action()
{
  switch (gPlayer.destination_action)
  {
  case PLAYER_DESTINATION_ATTACK:
    switch_state(PLAYER_ATTACKING);
    gPlayer.next_state = PLAYER_STANDING;
    break;
  case PLAYER_DESTINATION_STAND:
    switch_state(PLAYER_STANDING);
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

void switch_state(enum PLAYER_STATE new_state)
{
  switch (new_state)
  {
  case PLAYER_STANDING:
    gPlayer.animation_frame = 0;
    memset(gPlayer.path, -1, MAX_PATH_LENGTH);
    gPlayer.point_in_path = 0;
    gPlayer.animation = ANIM_WARRIOR_IDLE;
    gPlayer.state = new_state;
    break;
  case PLAYER_MOVING:
    gPlayer.point_in_path = 0;
    gPlayer.animation = ANIM_WARRIOR_WALK;
    gPlayer.state = new_state;
    break;
  case PLAYER_ATTACKING:
    gPlayer.animation_frame = 0;
    gPlayer.animation = ANIM_WARRIOR_ATTACK;
    gPlayer.state = new_state;
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

    int new_x = gPlayer.world_x + direction.x;
    int new_y = gPlayer.world_y + direction.y;

    Point check = {new_x, new_y};
    if (tile_is_blocked(check))
    {
      memset(gPlayer.path, -1, MAX_PATH_LENGTH);
      switch_state(PLAYER_STANDING);
    }
    else
    {
      gPlayer.world_x = new_x;
      gPlayer.world_y = new_y;
    }
    if (gPlayer.world_x == gPlayer.target.x && gPlayer.world_y == gPlayer.target.y)
    {
      player_do_destination_action();
    }
  }
}

void update_player_movement()
{
  if (gPlayer.frames_since_walk >= gPlayer.walk_interval)
  {
    gPlayer.frames_since_walk = 0;
    gPlayer.pixel_x = 0;
    gPlayer.pixel_y = 0;
    gPlayer.next_x = -1;
    gPlayer.next_y = -1;

    player_do_walk();
    if (!point_equal(gPlayer.target, gPlayer.new_target))
    {
      Point player_position = {gPlayer.world_x, gPlayer.world_y};
      if (!point_equal(player_position, gPlayer.new_target) && find_path(player_position, gPlayer.new_target, gPlayer.path, &tile_is_blocked))
      {
        switch_state(PLAYER_MOVING);
      }
      else if (gPlayer.state != PLAYER_ATTACKING && gPlayer.target_monster_id == -1)
      {
        switch_state(PLAYER_STANDING);
      }

      gPlayer.target = gPlayer.new_target;
    }
  }
  else
  {
    gPlayer.frames_since_walk += gClock.delta;
    double percentage_walked = (double)gPlayer.frames_since_walk / (double)gPlayer.walk_interval;
    if (percentage_walked >= 1.0)
    {
      percentage_walked = 1.0;
    }
    char raw_code = gPlayer.path[gPlayer.point_in_path];
    enum PATH_CODE code = (enum PATH_CODE)raw_code;
    gPlayer.direction = player_get_direction_from_path_code(code);
    Point direction = get_direction_from_path(code);
    Point isometric = cartesian_to_isometric(direction);
    int off_x, off_y;
    off_x = (int)(isometric.x * percentage_walked);
    off_y = (int)(isometric.y * percentage_walked);
    gPlayer.pixel_x = off_x;
    gPlayer.pixel_y = off_y;
    gPlayer.next_x = direction.x + gPlayer.world_x;
    gPlayer.next_y = direction.y + gPlayer.world_y;
  }
}

void update_player_attack()
{
  if (gPlayer.animation_frame == 8 && gPlayer.previous_animation_frame != 8)
  {
    int target_id = gPlayer.target_monster_id;
    if (target_id >= 0)
    {
      char *str = calloc(10, sizeof(char));
      sprintf(str, "%d", gPlayer.damage);
      Point monster_point = {monsters[target_id].world_x, monsters[target_id].world_y};
      DamageText dt = {
          str,
          monster_point.x,
          monster_point.y};
      dt.r = 255;
      dt.g = 255;
      dt.b = 0;

      push_damage_text(dt);
      monsters[target_id].hp -= gPlayer.damage;
      if (monsters[target_id].hp <= 0)
      {
        monsters[target_id].state = MONSTER_DEAD;
        monsters[target_id].animation_frame = 0;
        gDungeonMonsterTable[monsters[target_id].world_y][monsters[target_id].world_x] = -1;
      }
    }
    gPlayer.target_monster_id = -1;
  }
}

void update_player()
{
  switch (gPlayer.state)
  {
  case PLAYER_MOVING:
    update_player_movement();
    break;
  case PLAYER_STANDING:
    break;
  case PLAYER_ATTACKING:
    update_player_attack();
    break;
  default:
    break;
  }
}