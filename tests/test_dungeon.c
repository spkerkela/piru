#include "minunit.h"
#include "../dungeon.h"

/* Provide required globals */
Player gPlayer;
Monster monsters[MAX_MONSTERS];
int created_monsters = 0;
Clock gClock;
MouseCursor gCursor;
Point gSelectedTile;
bool gShiftIsDown = false;
Animation animations[256][256];
ImageAsset gImageAssets[MAX_SPRITES];
GroundEffect ground_effects[MAX_GROUND_EFFECTS];
int ground_effect_count = 0;
DamageText damage_text[MAX_DAMAGE_TEXT];
int damage_text_count = 0;
Spell gSpells[SPELL_COUNT];
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
TTF_Font *gFont = NULL;

/* clear_dungeon tests */

static const char *test_clear_dungeon_resets_tiles(void) {
    /* Dirty the dungeon first */
    gDungeon[5][5] = 'w';
    gDungeonBlockTable[5][5] = true;
    gDungeonMonsterTable[5][5] = 3;
    gDungeonWallTable[5][5] = 1;
    gDungeonVisibleTable[5][5] = true;

    clear_dungeon();

    mu_assert_int_eq(0, gDungeon[5][5]);
    mu_assert("block table should be false", !gDungeonBlockTable[5][5]);
    mu_assert_int_eq(-1, gDungeonMonsterTable[5][5]);
    mu_assert_int_eq(0, gDungeonWallTable[5][5]);
    mu_assert("visible table should be false", !gDungeonVisibleTable[5][5]);
    return NULL;
}

static const char *test_clear_dungeon_all_monsters_reset(void) {
    gDungeonMonsterTable[10][10] = 5;
    gDungeonMonsterTable[20][30] = 12;

    clear_dungeon();

    mu_assert_int_eq(-1, gDungeonMonsterTable[10][10]);
    mu_assert_int_eq(-1, gDungeonMonsterTable[20][30]);
    return NULL;
}

static const char *test_clear_dungeon_edges(void) {
    clear_dungeon();

    mu_assert_int_eq(0, gDungeon[0][0]);
    mu_assert_int_eq(0, gDungeon[DUNGEON_SIZE - 1][DUNGEON_SIZE - 1]);
    mu_assert_int_eq(-1, gDungeonMonsterTable[0][0]);
    mu_assert_int_eq(-1,
                     gDungeonMonsterTable[DUNGEON_SIZE - 1][DUNGEON_SIZE - 1]);
    return NULL;
}

/* tile_is_blocked tests */

static const char *test_tile_is_blocked_wall(void) {
    clear_dungeon();
    gDungeonBlockTable[5][5] = true;
    Point p = {5, 5};
    mu_assert("wall tile should be blocked", tile_is_blocked(p));
    return NULL;
}

static const char *test_tile_is_blocked_monster(void) {
    clear_dungeon();
    gDungeonMonsterTable[5][5] = 0;
    Point p = {5, 5};
    mu_assert("tile with monster should be blocked", tile_is_blocked(p));
    return NULL;
}

static const char *test_tile_is_blocked_free(void) {
    clear_dungeon();
    Point p = {5, 5};
    mu_assert("free tile should not be blocked", !tile_is_blocked(p));
    return NULL;
}

static const char *test_tile_is_blocked_wall_and_monster(void) {
    clear_dungeon();
    gDungeonBlockTable[5][5] = true;
    gDungeonMonsterTable[5][5] = 1;
    Point p = {5, 5};
    mu_assert("wall+monster tile should be blocked", tile_is_blocked(p));
    return NULL;
}

/* tile_is_visible tests */

static const char *test_tile_is_visible_true(void) {
    clear_dungeon();
    gDungeonVisibleTable[5][5] = true;
    Point p = {5, 5};
    mu_assert("visible tile should be visible", tile_is_visible(p));
    return NULL;
}

static const char *test_tile_is_visible_false(void) {
    clear_dungeon();
    Point p = {5, 5};
    mu_assert("non-visible tile should not be visible", !tile_is_visible(p));
    return NULL;
}

/* center tests */

static const char *test_center_simple(void) {
    SDL_Rect rect = {0, 0, 10, 10};
    Point c = center(&rect);
    mu_assert_int_eq(5, c.x);
    mu_assert_int_eq(5, c.y);
    return NULL;
}

static const char *test_center_offset(void) {
    SDL_Rect rect = {10, 20, 6, 8};
    Point c = center(&rect);
    mu_assert_int_eq(13, c.x);
    mu_assert_int_eq(24, c.y);
    return NULL;
}

static const char *test_center_unit(void) {
    SDL_Rect rect = {0, 0, 1, 1};
    Point c = center(&rect);
    mu_assert_int_eq(0, c.x);
    mu_assert_int_eq(0, c.y);
    return NULL;
}

static const char *test_center_large(void) {
    SDL_Rect rect = {0, 0, 100, 200};
    Point c = center(&rect);
    mu_assert_int_eq(50, c.x);
    mu_assert_int_eq(100, c.y);
    return NULL;
}

/* create_dungeon tests */

static const char *test_create_dungeon_border_walls(void) {
    srand(42);
    create_dungeon();

    /* Check that borders are walls */
    int i;
    for (i = 0; i < DUNGEON_SIZE; i++) {
        mu_assert("top border should be wall",
                  gDungeon[0][i] == 'w');
        mu_assert("bottom border should be wall",
                  gDungeon[DUNGEON_SIZE - 1][i] == 'w');
        mu_assert("left border should be wall",
                  gDungeon[i][0] == 'w');
        mu_assert("right border should be wall",
                  gDungeon[i][DUNGEON_SIZE - 1] == 'w');
    }
    return NULL;
}

static const char *test_create_dungeon_border_blocked(void) {
    srand(42);
    create_dungeon();

    int i;
    for (i = 0; i < DUNGEON_SIZE; i++) {
        mu_assert("top border should be blocked",
                  gDungeonBlockTable[0][i]);
        mu_assert("bottom border should be blocked",
                  gDungeonBlockTable[DUNGEON_SIZE - 1][i]);
    }
    return NULL;
}

static const char *test_create_dungeon_has_floor(void) {
    srand(42);
    create_dungeon();

    /* Interior should have at least some floor tiles */
    int floor_count = 0;
    int x, y;
    for (y = 1; y < DUNGEON_SIZE - 1; y++) {
        for (x = 1; x < DUNGEON_SIZE - 1; x++) {
            if (gDungeon[y][x] == 'f') {
                floor_count++;
            }
        }
    }
    mu_assert("dungeon should have floor tiles", floor_count > 0);
    return NULL;
}

static const char *test_create_dungeon_monster_table_clear(void) {
    srand(42);
    create_dungeon();

    int x, y;
    for (y = 0; y < DUNGEON_SIZE; y++) {
        for (x = 0; x < DUNGEON_SIZE; x++) {
            mu_assert("monster table should be -1 after create",
                      gDungeonMonsterTable[y][x] == -1);
        }
    }
    return NULL;
}

/* split_bsp tests */

static const char *test_split_bsp_too_small(void) {
    BSP root = {0, 0, MIN_ROOM_SIZE, MIN_ROOM_SIZE, NULL, NULL, NULL};
    bool result = split_bsp(&root);
    mu_assert("BSP too small to split should return false", !result);
    return NULL;
}

static const char *test_split_bsp_large_enough(void) {
    srand(42);
    BSP root = {0, 0, MAX_ROOM_SIZE + 10, MAX_ROOM_SIZE + 10, NULL, NULL, NULL};
    bool result = split_bsp(&root);
    mu_assert("large BSP should split", result);
    mu_assert("child1 should be created", root.child1 != NULL);
    mu_assert("child2 should be created", root.child2 != NULL);

    /* Children should cover the original area */
    if (root.child1 && root.child2) {
        /* Either horizontal or vertical split */
        if (root.child1->width == root.width) {
            /* Horizontal split */
            mu_assert_int_eq(root.width, root.child1->width);
            mu_assert_int_eq(root.width, root.child2->width);
            mu_assert("heights should sum to original",
                      root.child1->height + root.child2->height == root.height);
        } else {
            /* Vertical split */
            mu_assert_int_eq(root.height, root.child1->height);
            mu_assert_int_eq(root.height, root.child2->height);
            mu_assert("widths should sum to original",
                      root.child1->width + root.child2->width == root.width);
        }
        free(root.child1);
        free(root.child2);
    }
    return NULL;
}

static const char *test_split_bsp_already_split(void) {
    BSP child1 = {0};
    BSP child2 = {0};
    BSP root = {0, 0, 50, 50, &child1, &child2, NULL};
    bool result = split_bsp(&root);
    mu_assert("already-split BSP should return false", !result);
    return NULL;
}

/* update_fov tests */

static const char *test_update_fov_marks_tiles(void) {
    clear_dungeon();
    Point p = {50, 50};
    update_fov(p, 5);

    /* The tiles around the point should be visible */
    mu_assert("tile near center should be visible after fov update",
              gDungeonVisibleTable[50][51] ||
              gDungeonVisibleTable[49][50] ||
              gDungeonVisibleTable[50][50]);
    return NULL;
}

static const char *test_update_fov_wall_blocks(void) {
    clear_dungeon();
    /* Create a wall line blocking view */
    int x;
    for (x = 45; x <= 55; x++) {
        gDungeonBlockTable[48][x] = true;
    }
    Point p = {50, 50};
    update_fov(p, 5);

    /* Tiles behind the wall should not be visible */
    mu_assert("tile behind wall should not be visible",
              !gDungeonVisibleTable[44][50]);
    return NULL;
}

/* create_horizontal_tunnel tests */

static const char *test_create_horizontal_tunnel(void) {
    clear_dungeon();
    /* Fill with walls first */
    int x, y;
    for (y = 0; y < DUNGEON_SIZE; y++) {
        for (x = 0; x < DUNGEON_SIZE; x++) {
            gDungeon[y][x] = 'w';
            gDungeonBlockTable[y][x] = true;
        }
    }

    create_horizontal_tunnel(10, 20, 50);

    /* Check the tunnel is carved */
    for (x = 10; x <= 20; x++) {
        mu_assert("tunnel center should be floor",
                  gDungeon[50][x] == 'f');
        mu_assert("tunnel center should not be blocked",
                  !gDungeonBlockTable[50][x]);
    }
    return NULL;
}

/* create_vertical_tunnel tests */

static const char *test_create_vertical_tunnel(void) {
    clear_dungeon();
    int x, y;
    for (y = 0; y < DUNGEON_SIZE; y++) {
        for (x = 0; x < DUNGEON_SIZE; x++) {
            gDungeon[y][x] = 'w';
            gDungeonBlockTable[y][x] = true;
        }
    }

    create_vertical_tunnel(10, 20, 50);

    for (y = 10; y <= 20; y++) {
        mu_assert("tunnel center should be floor",
                  gDungeon[y][50] == 'f');
        mu_assert("tunnel center should not be blocked",
                  !gDungeonBlockTable[y][50]);
    }
    return NULL;
}

/* create_bsp_dungeon tests */

static const char *test_create_bsp_dungeon_has_spawn(void) {
    srand(42);
    Point location = {1, 1};
    Point size = {DUNGEON_SIZE - 2, DUNGEON_SIZE - 2};
    create_bsp_dungeon(location, size);

    mu_assert("spawn x should be valid",
              gPlayerLevelSpawn.x > 0 && gPlayerLevelSpawn.x < DUNGEON_SIZE);
    mu_assert("spawn y should be valid",
              gPlayerLevelSpawn.y > 0 && gPlayerLevelSpawn.y < DUNGEON_SIZE);
    return NULL;
}

static const char *test_create_bsp_dungeon_spawn_visible(void) {
    srand(42);
    Point location = {1, 1};
    Point size = {DUNGEON_SIZE - 2, DUNGEON_SIZE - 2};
    create_bsp_dungeon(location, size);

    mu_assert("spawn tile should be visible",
              gDungeonVisibleTable[gPlayerLevelSpawn.y][gPlayerLevelSpawn.x]);
    return NULL;
}

static const char *test_create_bsp_dungeon_has_rooms(void) {
    srand(42);
    Point location = {1, 1};
    Point size = {DUNGEON_SIZE - 2, DUNGEON_SIZE - 2};
    create_bsp_dungeon(location, size);

    /* Should have some floor tiles indicating rooms were carved */
    int floor_count = 0;
    int x, y;
    for (y = 2; y < DUNGEON_SIZE - 2; y++) {
        for (x = 2; x < DUNGEON_SIZE - 2; x++) {
            if (gDungeon[y][x] == 'f') {
                floor_count++;
            }
        }
    }
    mu_assert("BSP dungeon should have floor tiles", floor_count > 100);
    return NULL;
}

int main(void) {
    printf("\n=== dungeon tests ===\n");

    /* clear_dungeon */
    mu_run_test(test_clear_dungeon_resets_tiles);
    mu_run_test(test_clear_dungeon_all_monsters_reset);
    mu_run_test(test_clear_dungeon_edges);

    /* tile_is_blocked */
    mu_run_test(test_tile_is_blocked_wall);
    mu_run_test(test_tile_is_blocked_monster);
    mu_run_test(test_tile_is_blocked_free);
    mu_run_test(test_tile_is_blocked_wall_and_monster);

    /* tile_is_visible */
    mu_run_test(test_tile_is_visible_true);
    mu_run_test(test_tile_is_visible_false);

    /* center */
    mu_run_test(test_center_simple);
    mu_run_test(test_center_offset);
    mu_run_test(test_center_unit);
    mu_run_test(test_center_large);

    /* create_dungeon */
    mu_run_test(test_create_dungeon_border_walls);
    mu_run_test(test_create_dungeon_border_blocked);
    mu_run_test(test_create_dungeon_has_floor);
    mu_run_test(test_create_dungeon_monster_table_clear);

    /* split_bsp */
    mu_run_test(test_split_bsp_too_small);
    mu_run_test(test_split_bsp_large_enough);
    mu_run_test(test_split_bsp_already_split);

    /* update_fov */
    mu_run_test(test_update_fov_marks_tiles);
    mu_run_test(test_update_fov_wall_blocks);

    /* tunnels */
    mu_run_test(test_create_horizontal_tunnel);
    mu_run_test(test_create_vertical_tunnel);

    /* BSP dungeon generation */
    mu_run_test(test_create_bsp_dungeon_has_spawn);
    mu_run_test(test_create_bsp_dungeon_spawn_visible);
    mu_run_test(test_create_bsp_dungeon_has_rooms);

    mu_print_summary();
    return mu_tests_failed != 0;
}
