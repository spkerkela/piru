#include "dungeon.h"

char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
int gDungeonMonsterTable[DUNGEON_SIZE][DUNGEON_SIZE];
char gDungeonWallTable[DUNGEON_SIZE][DUNGEON_SIZE];

void clear_dungeon() {
  int x, y;
  for (y = 0; y < DUNGEON_SIZE; y++) {
    for (x = 0; x < DUNGEON_SIZE; x++) {
      gDungeon[y][x] = 0;
      gDungeonBlockTable[y][x] = false;
      gDungeonMonsterTable[y][x] = -1;
      gDungeonWallTable[y][x] = 0;
    }
  }
}

bool tile_is_blocked(const Point p) {
  return gDungeonBlockTable[p.y][p.x] || gDungeonMonsterTable[p.y][p.x] >= 0;
}

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

typedef struct BSP BSP;
struct BSP {
  int x, y, width, height, child_count;
  BSP *child1;
  BSP *child2;
  BSP *parent;
  SDL_Rect *room;
};

void carve_dungeon(BSP *bsp) {
  bool is_leaf = true;
  if (bsp->child1) {
    carve_dungeon(bsp->child1);
    is_leaf = false;
  }
  if (bsp->child2) {
    carve_dungeon(bsp->child2);
    is_leaf = false;
  }
  if (!is_leaf) {
    return;
  }
  int width, height;
  width = 0;
  height = 0;
  if (bsp->width < 10 || bsp->height < 10) {
    return;
  }
  do {
    width = bsp->width - 2;
    height = bsp->height - 2;
  } while (width <= 2 || width <= 2);
  int xx = bsp->x + 1;
  int yy = bsp->y + 1;

  bsp->room = malloc(sizeof(SDL_Rect));
  if (bsp->room) {
    bsp->room->x = xx;
    bsp->room->y = yy;
    bsp->room->w = width;
    bsp->room->h = height;
    int x, y;
    for (y = yy; y < height; y++) {
      for (x = xx; x < width; x++) {
        gDungeon[y][x] = 'f';
        gDungeonBlockTable[y][x] = false;
      }
    }
  }
}

Point center(const SDL_Rect *rect) {
  Point center_point;
  center_point.x = (rect->x + rect->w) / 2;
  center_point.y = (rect->y + rect->h) / 2;
  return center_point;
}

void create_vertical_tunnel(int y1, int y2, int x) {
  int y;
  for (y = min(y1, y2); y < max(y1, y2) + 1; y++) {
    gDungeon[y][x] = 'f';
    gDungeonBlockTable[y][x] = false;
  }
}

void create_horizontal_tunnel(int x1, int x2, int y) {
  int x;
  for (x = min(x1, x2); x < max(x1, x2) + 1; x++) {
    gDungeon[y][x] = 'f';
    gDungeonBlockTable[y][x] = false;
  }
}

SDL_Rect *get_random_room(BSP *bsp) {
  if (bsp->child1 && bsp->child2) {
    bool cointoss = rand() % 2 == 0;
    if (cointoss) {
      if (bsp->child1->room) {
        return bsp->child1->room;
      } else {
        return bsp->child2->room;
      }
    } else {
      if (bsp->child2->room) {
        return bsp->child2->room;
      } else {
        return bsp->child1->room;
      }
    }
  }
  if (bsp->child1) {
    return bsp->child1->room;
  } else if (bsp->child2) {
    return bsp->child2->room;
  }
  return NULL;
}

void connect_rooms(SDL_Rect *room1, SDL_Rect *room2) {
  Point room1_center = center(room1);
  Point room2_center = center(room2);
  bool cointoss = rand() % 2 == 0;
  if (cointoss) {
    create_horizontal_tunnel(room1_center.x, room2_center.x, room1_center.y);
    create_vertical_tunnel(room1_center.y, room2_center.y, room2_center.x);
  } else {
    create_vertical_tunnel(room1_center.y, room2_center.y, room2_center.x);
    create_horizontal_tunnel(room1_center.x, room2_center.x, room1_center.y);
  }
}

void connect_children(BSP *bsp) {
  if (bsp->child1 && bsp->child2) {
    if (bsp->child1->room && bsp->child2->room) {
      SDL_Rect *room1 = bsp->child1->room;
      SDL_Rect *room2 = bsp->child2->room;
      connect_rooms(room1, room2);
    }
    connect_children(bsp->child1);
    connect_children(bsp->child2);
  } else if (bsp->parent) {
    BSP *sibling;
    if (bsp->parent->child1 == bsp) {
      sibling = bsp->parent->child2;
    } else {
      sibling = bsp->parent->child1;
    }
    if (sibling) {
      SDL_Rect *room1 = get_random_room(sibling);
      SDL_Rect *room2 = get_random_room(bsp);
      if (room1 && room2) {

        connect_rooms(room1, room2);
      }
    }
  }
}

BSP *iterate_bsp(BSP *root, int iterations) {
  if (iterations <= 0) {
    return NULL;
  }
  if (root->width < 8 || root->height < 8) {
    return NULL;
  }

  root->child1 = calloc(1, sizeof(BSP));
  root->child2 = calloc(1, sizeof(BSP));
  root->room = NULL;
  bool horizontal = (bool)(rand() % 2 == 0);
  if (horizontal) {
    int random_x = random_at_most(root->width);
    root->child1->x = root->x;
    root->child1->y = root->y;
    root->child1->width = random_x;
    root->child1->height = root->height;
    root->child1->child_count = 2;
    root->child1->child1 = NULL;
    root->child1->child2 = NULL;
    root->child1->parent = root;

    root->child2->x = root->x + random_x;
    root->child2->y = root->y;
    root->child2->width = root->width - random_x;
    root->child2->height = root->height;
    root->child2->child_count = 2;
    root->child2->child1 = NULL;
    root->child2->child2 = NULL;
    root->child2->parent = root;

    root->child1 = iterate_bsp(root->child1, iterations - 1);
    root->child2 = iterate_bsp(root->child2, iterations - 1);

  } else {
    int random_y = random_at_most(root->height);
    root->child1->x = root->x;
    root->child1->y = root->y;
    root->child1->width = root->width;
    root->child1->height = random_y;
    root->child1->child_count = 2;
    root->child1->child1 = NULL;
    root->child1->child2 = NULL;
    root->child1->parent = root;

    root->child2->x = root->x;
    root->child2->y = root->y + random_y;
    root->child2->width = root->width;
    root->child2->height = root->height - random_y;
    root->child1->child_count = 2;
    root->child2->child1 = NULL;
    root->child2->child2 = NULL;
    root->child2->parent = root;

    root->child1 = iterate_bsp(root->child1, iterations - 1);
    root->child2 = iterate_bsp(root->child2, iterations - 1);
  }
  return root;
}

void create_bsp_dungeon() {
  int x, y;
  for (y = 0; y < DUNGEON_SIZE; y++) {
    for (x = 0; x < DUNGEON_SIZE; x++) {
      gDungeon[y][x] = 'w';
      gDungeonBlockTable[y][x] = true;
    }
  }
  BSP root = {0, 0, DUNGEON_SIZE, DUNGEON_SIZE, 2, NULL, NULL, NULL};
  iterate_bsp(&root, 8);
  carve_dungeon(&root);
  connect_children(&root);
  init_monster_table();
  create_walls();
}
