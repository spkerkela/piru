#include "player.h"

Player gPlayer;

player_state_fn stand, move, move_offset, try_attack, attack;

void init_player() {
  gPlayer.next_state_fn = stand;
  gPlayer.world_x = gPlayerLevelSpawn.x;
  gPlayer.world_y = gPlayerLevelSpawn.y;

  gPlayer.previous_world_x = gPlayer.world_x;
  gPlayer.previous_world_y = gPlayer.world_y;
  gPlayer.next_x = -1;
  gPlayer.next_y = -1;
  gPlayer.pixel_x = 0;
  gPlayer.pixel_y = 0;
  gPlayer.attack_radius = 2.0;
  gPlayer.current_game_level = 0;
  gPlayer.direction = PLAYER_SOUTH;
  gPlayer.point_in_path = 0;
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
  gPlayer.walk_interval = 200;
  gPlayer.frames_since_walk = 0;
  gPlayer.frames_since_animation_frame = 0;
  gPlayer.animation_intervals[ANIM_WARRIOR_ATTACK] = 30;
  gPlayer.animation_intervals[ANIM_WARRIOR_WALK] = 80;
  gPlayer.animation_intervals[ANIM_WARRIOR_IDLE] = 100;
}

Point get_player_point(Player *player) {
  Point p = {player->world_x, player->world_y};
  return p;
}

void clear_player_path(Player *player) {
  player->point_in_path = 0;
  player->target = get_player_point(player);
  player->new_target = player->target;
}

void try_attack(Player *player) {
  Point player_point = get_player_point(player);
  Point monster_point = get_monster_point(player->target_monster_id);
  if (get_distance(player_point, monster_point) <= player->attack_radius) {
    player->direction = player_get_direction8(player->world_x, player->world_y,
                                              monster_point.x, monster_point.y);
    player->animation_frame = 0;
    player->next_state_fn = attack;
  } else if (find_path(player_point,
                       find_nearest_node_to_monster(player->target_monster_id,
                                                    player_point),
                       player->path, &tile_is_blocked)) {
    player->target = monster_point;
    player->point_in_path = 0;
    player->next_state_fn = move;
  } else {
    player->next_state_fn = stand;
    player->target_monster_id = -1;
  }
}

void stand(Player *player) {
  player->animation = ANIM_WARRIOR_IDLE;
  player->state = PLAYER_STANDING;
  if (player->target_monster_id >= 0) {
    player->next_state_fn = try_attack;
  } else if (!point_equal(player->target, player->new_target)) {
    Point player_point = get_player_point(player);
    player->target = player->new_target;
    if (find_path(player_point, player->target, player->path,
                  &tile_is_blocked)) {
      player->point_in_path = 0;
      player->next_state_fn = move;
    }
  }
}

void player_do_walk(Player *player) {
  char raw_code = player->path[gPlayer.point_in_path];
  enum PATH_CODE code = (enum PATH_CODE)raw_code;
  Point direction = get_direction_from_path(code);

  int new_x = player->world_x + direction.x;
  int new_y = player->world_y + direction.y;

  Point check = {new_x, new_y};
  if (tile_is_blocked(check)) {
    player->next_state_fn = stand;
    player->pixel_x = 0;
    player->pixel_y = 0;
  } else {
    player->world_x = new_x;
    player->world_y = new_y;
    player->direction = player_get_direction_from_path_code(code);
    Point direction = get_direction_from_path(code);
    Point isometric = cartesian_to_isometric(direction);
    player->pixel_x = isometric.x;
    player->pixel_y = isometric.y;
    player->point_in_path++;
    player->frames_since_walk = 0;
  }
}

void move(Player *player) {
  player->previous_world_x = player->world_x;
  player->previous_world_y = player->world_y;
  if (player->world_x == player->target.x &&
      player->world_y == player->target.y) {
    if (player->target_monster_id >= 0) {
      player->next_state_fn = try_attack;
    } else {
      player->next_state_fn = stand;
    }
    return;
  }
  if (!point_equal(player->target, player->new_target)) {
    Point player_position = get_player_point(player);
    player->target = player->new_target;
    if (!find_path(player_position, player->target, player->path,
                   &tile_is_blocked)) {
      player->next_state_fn = stand;
      return;
    } else {
      player->point_in_path = 0;
    }
  }
  char raw_code = player->path[gPlayer.point_in_path];
  if (raw_code == -1) {
    player->next_state_fn = stand;
    return;
  }
  player->animation = ANIM_WARRIOR_WALK;
  player->state = PLAYER_MOVING;
  player->next_state_fn = move_offset;
  player_do_walk(player);
}

void move_offset(Player *player) {

  player->frames_since_walk += gClock.delta;
  double percentage_walked =
      (double)player->frames_since_walk / (double)player->walk_interval;
  if (percentage_walked >= 1.0) {
    percentage_walked = 1.0;
  }
  Point direction = get_direction_from_player_direction(player->direction);
  Point isometric = cartesian_to_isometric(direction);
  int off_x, off_y;
  off_x = (int)(isometric.x * (1.0 - percentage_walked));
  off_y = (int)(isometric.y * (1.0 - percentage_walked));
  player->pixel_x = off_x;
  player->pixel_y = off_y;
  if (player->frames_since_walk >= player->walk_interval) {
    player->next_state_fn = move;
    player->pixel_x = 0;
    player->pixel_y = 0;
  }
}

void attack(Player *player) {
  player->animation = ANIM_WARRIOR_ATTACK;
  player->state = PLAYER_ATTACKING;
  if (player->animation_frame == 8 && player->previous_animation_frame != 8) {
    int target_id = player->target_monster_id;
    Point monster_point = get_monster_point(target_id);
    Point player_point = get_player_point(player);
    if (get_distance(player_point, monster_point) <= player->attack_radius) {
      char *str = calloc(10, sizeof(char));
      int damage = rand() % 100 < 20 ? player->damage * 2 : player->damage;
      sprintf(str, "%d", damage);
      Point monster_point = get_monster_point(target_id);
      DamageText dt = {str, monster_point.x, monster_point.y};
      dt.r = 255;
      dt.g = 255;
      dt.b = 0;

      push_damage_text(dt);
      monsters[target_id].hp -= damage;
    }
    player->target_monster_id = -1;
  }

  int animFrames = animations[player->animation][player->direction].columns;
  if (player->animation_frame >= animFrames - 1) {
    player->animation_frame = 0;
    player->next_state_fn = stand;
    clear_player_path(player);
  }
}

void update_player() {
  if (gPlayer.next_state_fn) {
    gPlayer.next_state_fn(&gPlayer);
  }
}
