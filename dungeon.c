#include "dungeon.h"

char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
bool gDungeonVisibleTable[DUNGEON_SIZE][DUNGEON_SIZE];
int gDungeonMonsterTable[DUNGEON_SIZE][DUNGEON_SIZE];
char gDungeonWallTable[DUNGEON_SIZE][DUNGEON_SIZE];
Point gPlayerLevelSpawn;
BSP *bsps[MAX_DUNGEON_ROOMS];
int bsp_count = 0;
static int multipliers[4][8] = {{1, 0, 0, -1, -1, 0, 0, 1},
                                {0, 1, -1, 0, 0, -1, 1, 0},
                                {0, 1, 1, 0, 0, -1, -1, 0},
                                {1, 0, 0, 1, -1, 0, 0, -1}};

void clear_dungeon() {
  int x, y;
  for (y = 0; y < DUNGEON_SIZE; y++) {
    for (x = 0; x < DUNGEON_SIZE; x++) {
      gDungeon[y][x] = 0;
      gDungeonBlockTable[y][x] = false;
      gDungeonMonsterTable[y][x] = -1;
      gDungeonWallTable[y][x] = 0;
      gDungeonVisibleTable[y][x] = false;
    }
  }
}

bool tile_is_blocked(const Point p) {
  return gDungeonBlockTable[p.y][p.x] || gDungeonMonsterTable[p.y][p.x] >= 0;
}

bool tile_is_visible(const Point p) { return gDungeonVisibleTable[p.y][p.x]; }

bool tile_is_blocked_for_monster(const Point p) {
  return tile_is_blocked(p) || (gPlayer.next_x == p.x && gPlayer.next_y == p.y);
}

void init_monster_table() {
  int x, y;

  for (x = 0; x < DUNGEON_SIZE; x++) {
    for (y = 0; y < DUNGEON_SIZE; y++) {
      gDungeonMonsterTable[y][x] = -1;
    }
  }
}

void create_walls() {
  int x, y;
  for (y = 0; y < DUNGEON_SIZE; y++) {
    for (x = 0; x < DUNGEON_SIZE; x++) {
      if (gDungeonBlockTable[y][x]) {
        continue;
      }
      if (gDungeonBlockTable[y - 1][x]) {
        gDungeonWallTable[y][x] |= WALL_NORTH_EAST;
      }
      if (gDungeonBlockTable[y + 1][x]) {
        gDungeonWallTable[y][x] |= WALL_SOUTH_WEST;
      }
      if (gDungeonBlockTable[y][x - 1]) {
        gDungeonWallTable[y][x] |= WALL_NORTH_WEST;
      }
      if (gDungeonBlockTable[y][x + 1]) {
        gDungeonWallTable[y][x] |= WALL_SOUTH_EAST;
      }
    }
  }
}

void create_dungeon() {
  int x, y;
  for (x = 0; x < DUNGEON_SIZE; x++) {
    for (y = 0; y < DUNGEON_SIZE; y++) {
      gDungeonWallTable[y][x] = 0;

      if (x == 0 || x == DUNGEON_SIZE - 1 || y == 0 || y == DUNGEON_SIZE - 1) {
        gDungeon[y][x] = 'w';
        gDungeonBlockTable[y][x] = true;
      } else {
        gDungeon[y][x] = 'f';
        gDungeonBlockTable[y][x] = false;
      }

      if (x > 6 && y > 6) {
        int random = rand();
        bool put_wall = random % 5 == 0;
        if (put_wall) {
          gDungeon[y][x] = 'w';
          gDungeonBlockTable[y][x] = true;
        }
      }
    }
  }

  gDungeon[3][5] = 'w';
  gDungeonBlockTable[3][5] = true;
  gDungeon[2][5] = 'w';
  gDungeonBlockTable[2][5] = true;
  gDungeon[1][5] = 'w';
  gDungeonBlockTable[1][5] = true;
  gDungeon[5][2] = 'w';
  gDungeonBlockTable[5][2] = true;
  gDungeon[5][3] = 'w';
  gDungeonBlockTable[5][3] = true;
  gDungeon[5][4] = 'w';
  gDungeonBlockTable[5][4] = true;
  gDungeon[5][5] = 'w';
  gDungeonBlockTable[5][5] = true;

  init_monster_table();
  create_walls();
}

void cast_light(uint x, uint y, uint radius, uint row, float start_slope,
                float end_slope, uint xx, uint xy, uint yx, uint yy) {
  if (start_slope < end_slope) {
    return;
  }
  float next_start_slope = start_slope;
  for (uint i = row; i <= radius; i++) {
    bool blocked = false;
    for (int dx = -i, dy = -i; dx <= 0; dx++) {
      float l_slope = (dx - 0.5) / (dy + 0.5);
      float r_slope = (dx + 0.5) / (dy - 0.5);
      if (start_slope < r_slope) {
        continue;
      } else if (end_slope > l_slope) {
        break;
      }

      int sax = dx * xx + dy * xy;
      int say = dx * yx + dy * yy;
      if ((sax < 0 && (uint)abs(sax) > x) || (say < 0 && (uint)abs(say) > y)) {
        continue;
      }
      uint ax = x + sax;
      uint ay = y + say;
      if (ax >= DUNGEON_SIZE || ay >= DUNGEON_SIZE) {
        continue;
      }

      uint radius2 = radius * radius;
      if ((uint)(dx * dx + dy * dy) < radius2) {
        gDungeonVisibleTable[ay][ax] = true;
      }

      if (blocked) {
        if (gDungeonBlockTable[ay][ax]) {
          next_start_slope = r_slope;
          continue;
        } else {
          blocked = false;
          start_slope = next_start_slope;
        }
      } else if (gDungeonBlockTable[ay][ax]) {
        blocked = true;
        next_start_slope = r_slope;
        cast_light(x, y, radius, i + 1, start_slope, l_slope, xx, xy, yx, yy);
      }
    }
    if (blocked) {
      break;
    }
  }
}

void update_fov(Point p, int radius) {
  for (uint i = 0; i < 8; i++) {
    cast_light((uint)p.x, (uint)p.y, (uint)radius, 1, 1.0, 0.0,
               multipliers[0][i], multipliers[1][i], multipliers[2][i],
               multipliers[3][i]);
  }
}

Point center(const SDL_Rect *rect) {
  Point center_point;
  center_point.x = rect->x + (rect->w / 2);
  center_point.y = rect->y + (rect->h / 2);
  return center_point;
}

void connect_points(Point point1, Point point2) {
  bool cointoss = rand() % 2 == 0;
  if (cointoss) {
    create_horizontal_tunnel(point1.x, point2.x, point1.y);
    create_vertical_tunnel(point1.y, point2.y, point2.x);
  } else {
    create_vertical_tunnel(point1.y, point2.y, point2.x);
    create_horizontal_tunnel(point1.x, point2.x, point1.y);
  }
}

void create_vertical_tunnel(int y1, int y2, int x) {
  int y;
  for (y = min(y1, y2); y < max(y1, y2) + 1; y++) {
    gDungeon[y][x] = 'f';
    gDungeon[y][x - 1] = 'f';
    gDungeon[y][x + 1] = 'f';
    gDungeonBlockTable[y][x] = false;
    gDungeonBlockTable[y][x - 1] = false;
    gDungeonBlockTable[y][x + 1] = false;
  }
}

void create_horizontal_tunnel(int x1, int x2, int y) {
  int x;
  for (x = min(x1, x2); x < max(x1, x2) + 1; x++) {
    gDungeon[y][x] = 'f';
    gDungeon[y - 1][x] = 'f';
    gDungeon[y + 1][x] = 'f';
    gDungeonBlockTable[y][x] = false;
    gDungeonBlockTable[y - 1][x] = false;
    gDungeonBlockTable[y + 1][x] = false;
  }
}
void connect_rooms(SDL_Rect *room1, SDL_Rect *room2) {
  Point room1_center = center(room1);
  Point room2_center = center(room2);
  connect_points(room1_center, room2_center);
}

SDL_Rect *get_room(BSP *root) {
  if (root->room) {
    return root->room;
  }
  SDL_Rect *room1 = NULL;
  SDL_Rect *room2 = NULL;
  if (root->child1) {
    room1 = get_room(root->child1);
  }
  if (root->child2) {
    room2 = get_room(root->child2);
  }
  if (!room2) {
    return room1;
  } else if (!room1) {
    return room2;
  } else if (room1 && room2) {
    return rand() % 2 == 0 ? room1 : room2;
  }
  return NULL;
}

void carve_dungeon(BSP *bsp) {
  if (bsp->child1 || bsp->child2) {
    if (bsp->child1) {
      carve_dungeon(bsp->child1);
    }
    if (bsp->child2) {
      carve_dungeon(bsp->child2);
    }
    if (bsp->child1 && bsp->child2) {
      SDL_Rect *room1 = get_room(bsp->child1);
      SDL_Rect *room2 = get_room(bsp->child2);
      if (room1 && room2) {
        connect_rooms(room1, room2);
      }
    }
  } else {
    if (bsp->room) {
      return;
    }
    bsp->room = malloc(sizeof(SDL_Rect));
    if (bsp->room) {

      Point room_size = {random_between(3, bsp->width - 2),
                         random_between(3, bsp->height - 2)};
      Point room_pos = {
          random_between(1, bsp->width - room_size.x - 1),
          random_between(1, bsp->height - room_size.y - 1),
      };

      bsp->room->x = bsp->x + room_pos.x;
      bsp->room->y = bsp->y + room_pos.y;
      bsp->room->w = room_size.x;
      bsp->room->h = room_size.y;

      int x, y;
      for (y = bsp->room->y; y < bsp->room->y + bsp->room->h; y++) {
        for (x = bsp->room->x; x < bsp->room->x + bsp->room->w; x++) {
          gDungeon[y][x] = 'f';
          gDungeonBlockTable[y][x] = false;
        }
      }
    }
  }
}

bool split_bsp(BSP *root) {
  if (root->child1 || root->child2) {
    return false;
  }

  bool horizontal = (bool)(rand() % 2 == 0);
  if (root->width > root->height &&
      (double)root->width / (double)root->height >= 1.25) {
    horizontal = false;
  } else if (root->height > root->width &&
             (double)root->height / (double)root->width >= 1.25) {
    horizontal = true;
  }

  int max_size = (horizontal ? root->height : root->width) - MIN_ROOM_SIZE;
  if (max_size < MIN_ROOM_SIZE) {
    return false;
  }

  root->child1 = calloc(1, sizeof(BSP));
  root->child2 = calloc(1, sizeof(BSP));
  root->room = NULL;

  int split_at = random_between(MIN_ROOM_SIZE, max_size);
  if (horizontal) {
    root->child1->x = root->x;
    root->child1->y = root->y;
    root->child1->width = root->width;
    root->child1->height = split_at;
    root->child1->child1 = NULL;
    root->child1->child2 = NULL;

    root->child2->x = root->x;
    root->child2->y = root->y + split_at;
    root->child2->width = root->width;
    root->child2->height = root->height - split_at;
    root->child2->child1 = NULL;
    root->child2->child2 = NULL;

  } else {
    root->child1->x = root->x;
    root->child1->y = root->y;
    root->child1->width = split_at;
    root->child1->height = root->height;
    root->child1->child1 = NULL;
    root->child1->child2 = NULL;

    root->child2->x = root->x + split_at;
    root->child2->y = root->y;
    root->child2->width = root->width - split_at;
    root->child2->height = root->height;
    root->child2->child1 = NULL;
    root->child2->child2 = NULL;
  }
  return true;
}

void free_bsp() {
  int i;
  for (i = 0; i < MAX_DUNGEON_ROOMS; i++) {
    if (bsps[i]) {
      if (bsps[i]->room) {
        free(bsps[i]->room);
      }
      free(bsps[i]);
    }
    bsps[i] = NULL;
  }
  bsp_count = 0;
}

void create_bsp_dungeon(Point location, Point size) {
  int x, y, i;
  free_bsp();
  for (y = 0; y < DUNGEON_SIZE; y++) {
    for (x = 0; x < DUNGEON_SIZE; x++) {
      gDungeon[y][x] = 'w';
      gDungeonBlockTable[y][x] = true;
      gDungeonVisibleTable[y][x] = false;
    }
  }
  BSP *root = malloc(sizeof(BSP));
  if (!root) {
    return;
  }
  root->x = location.x;
  root->y = location.y;
  root->width = size.x;
  root->height = size.y;
  root->child1 = NULL;
  root->child2 = NULL;
  bsps[bsp_count++] = root;
  bool did_split = true;
  BSP *iter = NULL;
  while (did_split && bsp_count < MAX_DUNGEON_ROOMS) {
    did_split = false;
    for (i = 0; i < bsp_count; i++) {
      iter = bsps[i];
      if (!iter->child1 && !iter->child2) {
        if (iter->width > MAX_ROOM_SIZE || iter->height > MAX_ROOM_SIZE) {
          // Leaf node
          if (split_bsp(iter)) {
            bsps[bsp_count++] = iter->child1;
            bsps[bsp_count++] = iter->child2;
            did_split = true;
          }
        }
      }
    }
  }
  for (i = 0; i < bsp_count; i++) {
    carve_dungeon(bsps[i]);
  }
  init_monster_table();
  create_walls();
  for (i = bsp_count; i >= 0; i--) {
    if (bsps[i] && bsps[i]->room) {
      gPlayerLevelSpawn = center(bsps[i]->room);
    }
  }

  gDungeonVisibleTable[gPlayerLevelSpawn.y][gPlayerLevelSpawn.x] = true;
  free_bsp();
}
