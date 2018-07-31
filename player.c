#include "player.h"

Player gPlayer;

player_state_fn stand, move, move_offset, try_attack, attack;

void try_attack(Player *player) {
  Point player_point = {player->world_x, player->world_y};
  Monster monster = monsters[player->target_monster_id];
  Point monster_point = {monster.world_x, monster.world_y};
  if (get_distance(player_point, monster_point) <= player->attack_radius) {
    player->direction = player_get_direction8(player->world_x, player->world_y,
                                              monster.world_x, monster.world_y);
    player->animation_frame = 0;
    player->next_state_fn = attack;
  } else if (find_path(player_point, monster_point, player->path,
                       &tile_is_blocked)) {
    player->next_state_fn = move_offset;
    player->target = monster_point;
    player->new_target = monster_point;
  } else {
    player->target_monster_id = -1;
    player->next_state_fn = stand;
  }
}

void stand(Player *player) {
  player->animation = ANIM_WARRIOR_IDLE;
  player->state = PLAYER_STANDING;
  if (player->target_monster_id >= 0) {
    player->next_state_fn = try_attack;
  } else if (!point_equal(player->target, player->new_target)) {
    Point player_point = {player->world_x, player->world_y};
    player->target = player->new_target;
    if (find_path(player_point, player->target, player->path,
                  &tile_is_blocked)) {
      player->point_in_path = 0;
      player->next_state_fn = move_offset;
    }
  }
}

void init_player() {
  gPlayer.next_state_fn = stand;
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
  switch_state(PLAYER_STANDING);
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
  gPlayer.moving_between_points = false;
  gPlayer.walk_interval = 1000;
  gPlayer.frames_since_walk = 0;
  gPlayer.frames_since_animation_frame = 0;
  gPlayer.animation_intervals[ANIM_WARRIOR_ATTACK] = 20;
  gPlayer.animation_intervals[ANIM_WARRIOR_WALK] = 80;
  gPlayer.animation_intervals[ANIM_WARRIOR_IDLE] = 100;
}

void player_do_destination_action() {
  switch (gPlayer.destination_action) {
  case PLAYER_DESTINATION_ATTACK:
    switch_state(PLAYER_ATTACKING);
    gPlayer.next_state = PLAYER_STANDING;
    break;
  case PLAYER_DESTINATION_STAND:
    switch_state(PLAYER_STANDING);
    break;
  case PLAYER_DESTINATION_INTERACT_OBJECT:
    break;
  case PLAYER_DESTINATION_PICK_ITEM:
    break;
  default:
    break;
  }
}

void switch_state(enum PLAYER_STATE new_state) {
  if (gPlayer.state == new_state) {
    return;
  }
  gPlayer.animation_frame = 0;
  gPlayer.frames_since_animation_frame = 0;
  switch (new_state) {
  case PLAYER_STANDING:
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
    gPlayer.animation = ANIM_WARRIOR_ATTACK;
    gPlayer.state = new_state;
    break;
  default:
    break;
  }
}

void player_do_walk(Player *player) {
  char raw_code = player->path[player->point_in_path];
  if (raw_code != -1) {
    enum PATH_CODE code = (enum PATH_CODE)raw_code;
    Point direction = get_direction_from_path(code);

    int new_x = player->world_x + direction.x;
    int new_y = player->world_y + direction.y;

    Point check = {new_x, new_y};
    if (tile_is_blocked(check)) {
      player->next_state_fn = stand;
    } else {
      player->world_x = new_x;
      player->world_y = new_y;
      player->point_in_path++;
    }
    if (player->world_x == player->target.x &&
        player->world_y == player->target.y) {
      player->next_state_fn = stand;
    }
  } else {
    player->next_state_fn = stand;
  }
}

void move_offset(Player *player) {
  player->animation = ANIM_WARRIOR_WALK;
  player->state = PLAYER_MOVING;
  gPlayer.frames_since_walk += gClock.delta;
  double percentage_walked =
      (double)gPlayer.frames_since_walk / (double)gPlayer.walk_interval;
  if (percentage_walked >= 1.0) {
    percentage_walked = 1.0;
  }
  char raw_code = gPlayer.path[gPlayer.point_in_path];
  if (raw_code == -1) {
    printf("FAILRE MODE ACTIVATED\n");
    printf("%d %d\n", gPlayer.target.x, gPlayer.target.y);
  }
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
  if (player->frames_since_walk >= player->walk_interval) {
    player->next_state_fn = move;
  }
}

void move(Player *player) {
  player->animation = ANIM_WARRIOR_WALK;
  player->state = PLAYER_MOVING;

  player->next_state_fn = move_offset;

  if (player->frames_since_walk >= player->walk_interval) {
    player->frames_since_walk = 0;
    player->pixel_x = 0;
    player->pixel_y = 0;
    player->next_x = -1;
    player->next_y = -1;

    if (player->target_monster_id >= 0) {
      player->next_state_fn = try_attack;
      return;
    }
    player_do_walk(player);
    if (!point_equal(player->target, player->new_target)) {
      Point player_position = {player->world_x, player->world_y};
      player->target = player->new_target;
      if (!find_path(player_position, player->target, player->path,
                     &tile_is_blocked)) {
        player->next_state_fn = stand;
      } else {
        player->point_in_path = 0;
      }
    }
  }
}

void attack(Player *player) {
  player->animation = ANIM_WARRIOR_ATTACK;
  player->state = PLAYER_ATTACKING;
  if (player->animation_frame == 8 && player->previous_animation_frame != 8) {
    int target_id = player->target_monster_id;
    if (target_id >= 0) {
      char *str = calloc(10, sizeof(char));
      sprintf(str, "%d", player->damage);
      Point monster_point = {monsters[target_id].world_x,
                             monsters[target_id].world_y};
      DamageText dt = {str, monster_point.x, monster_point.y};
      dt.r = 255;
      dt.g = 255;
      dt.b = 0;

      push_damage_text(dt);
      monsters[target_id].hp -= player->damage;
      if (monsters[target_id].hp <= 0) {
        monsters[target_id].state = MONSTER_DEAD;
        monsters[target_id].animation_frame = 0;
        gDungeonMonsterTable[monsters[target_id].world_y]
                            [monsters[target_id].world_x] = -1;
      }
    }
    player->target_monster_id = -1;
  }

  int animFrames = animations[player->animation][player->direction].columns;
  if (player->animation_frame >= animFrames - 1) {
    player->next_state_fn = stand;
  }
}

void update_player() {
  if (gPlayer.next_state_fn) {
    gPlayer.next_state_fn(&gPlayer);
  }
}
