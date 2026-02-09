#include "minunit.h"
#include "../pathfinding.h"

/* We need dungeon globals for pathfinding */
extern char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
extern bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern bool gDungeonVisibleTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern int gDungeonMonsterTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern int gDungeonGroundEffectTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern char gDungeonWallTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern Point gPlayerLevelSpawn;

/* Provide required globals that headers reference */
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

/* Simple obstacle check for testing */
static bool test_tile_blocked(const Point p) {
    if (p.x < 0 || p.y < 0 || p.x >= DUNGEON_SIZE || p.y >= DUNGEON_SIZE) {
        return true;
    }
    return gDungeonBlockTable[p.y][p.x];
}

/* Helper: set up a clear dungeon for pathfinding tests */
static void setup_clear_dungeon(void) {
    int x, y;
    for (y = 0; y < DUNGEON_SIZE; y++) {
        for (x = 0; x < DUNGEON_SIZE; x++) {
            gDungeonBlockTable[y][x] = false;
            gDungeonMonsterTable[y][x] = -1;
        }
    }
}

/* Helper: set up walls around the edges */
static void setup_walled_dungeon(void) {
    int x, y;
    for (y = 0; y < DUNGEON_SIZE; y++) {
        for (x = 0; x < DUNGEON_SIZE; x++) {
            if (x == 0 || x == DUNGEON_SIZE - 1 ||
                y == 0 || y == DUNGEON_SIZE - 1) {
                gDungeonBlockTable[y][x] = true;
            } else {
                gDungeonBlockTable[y][x] = false;
            }
            gDungeonMonsterTable[y][x] = -1;
        }
    }
}

/* get_heuristic_cost tests */

static const char *test_heuristic_same_point(void) {
    Point a = {5, 5};
    Point b = {5, 5};
    mu_assert_int_eq(0, get_heuristic_cost(a, b));
    return NULL;
}

static const char *test_heuristic_horizontal(void) {
    Point a = {0, 0};
    Point b = {3, 0};
    /* 2 * (3 + 0) = 6 */
    mu_assert_int_eq(6, get_heuristic_cost(a, b));
    return NULL;
}

static const char *test_heuristic_vertical(void) {
    Point a = {0, 0};
    Point b = {0, 4};
    /* 2 * (0 + 4) = 8 */
    mu_assert_int_eq(8, get_heuristic_cost(a, b));
    return NULL;
}

static const char *test_heuristic_diagonal(void) {
    Point a = {0, 0};
    Point b = {3, 4};
    /* 2 * (3 + 4) = 14 */
    mu_assert_int_eq(14, get_heuristic_cost(a, b));
    return NULL;
}

static const char *test_heuristic_symmetric(void) {
    Point a = {2, 3};
    Point b = {7, 1};
    mu_assert_int_eq(get_heuristic_cost(a, b), get_heuristic_cost(b, a));
    return NULL;
}

/* path_check_equal tests */

static const char *test_path_check_equal_same_x(void) {
    PathNode node = {0};
    node.x = 5;
    node.y = 3;
    Point dest = {5, 7};
    /* same x: returns 2 */
    mu_assert_int_eq(2, path_check_equal(&node, dest));
    return NULL;
}

static const char *test_path_check_equal_same_y(void) {
    PathNode node = {0};
    node.x = 5;
    node.y = 3;
    Point dest = {7, 3};
    /* same y: returns 2 */
    mu_assert_int_eq(2, path_check_equal(&node, dest));
    return NULL;
}

static const char *test_path_check_equal_diagonal(void) {
    PathNode node = {0};
    node.x = 5;
    node.y = 3;
    Point dest = {7, 7};
    /* different x and y: returns 3 */
    mu_assert_int_eq(3, path_check_equal(&node, dest));
    return NULL;
}

/* find_path tests */

static const char *test_find_path_adjacent(void) {
    setup_clear_dungeon();
    Point source = {5, 5};
    Point dest = {6, 5};
    char path[MAX_PATH_LENGTH];
    int length = find_path(source, dest, path, test_tile_blocked);
    mu_assert("should find path to adjacent tile", length > 0);
    mu_assert_int_eq(1, length);
    return NULL;
}

static const char *test_find_path_same_point(void) {
    setup_clear_dungeon();
    Point source = {5, 5};
    Point dest = {5, 5};
    char path[MAX_PATH_LENGTH];
    int length = find_path(source, dest, path, test_tile_blocked);
    mu_assert_int_eq(0, length);
    return NULL;
}

static const char *test_find_path_short_distance(void) {
    setup_clear_dungeon();
    Point source = {5, 5};
    Point dest = {8, 5};
    char path[MAX_PATH_LENGTH];
    int length = find_path(source, dest, path, test_tile_blocked);
    mu_assert("should find path 3 tiles away", length > 0);
    mu_assert("path length should be reasonable", length <= 5);
    return NULL;
}

static const char *test_find_path_diagonal(void) {
    setup_clear_dungeon();
    Point source = {5, 5};
    Point dest = {8, 8};
    char path[MAX_PATH_LENGTH];
    int length = find_path(source, dest, path, test_tile_blocked);
    mu_assert("should find diagonal path", length > 0);
    /* Diagonal 3 steps = 3 path codes */
    mu_assert_int_eq(3, length);
    return NULL;
}

static const char *test_find_path_around_obstacle(void) {
    setup_clear_dungeon();
    /* Place a wall blocking direct path */
    gDungeonBlockTable[5][7] = true;
    gDungeonBlockTable[5][8] = true;
    gDungeonBlockTable[5][9] = true;
    Point source = {5, 5};
    Point dest = {10, 5};
    char path[MAX_PATH_LENGTH];
    int length = find_path(source, dest, path, test_tile_blocked);
    mu_assert("should find path around obstacle", length > 0);

    /* Verify the path actually reaches the destination */
    int x = source.x, y = source.y;
    int i;
    for (i = 0; i < length; i++) {
        Point dir = get_direction_from_path((enum PATH_CODE)path[i]);
        x += dir.x;
        y += dir.y;
    }
    mu_assert_int_eq(dest.x, x);
    mu_assert_int_eq(dest.y, y);
    return NULL;
}

static const char *test_find_path_negative_dest(void) {
    setup_clear_dungeon();
    Point source = {5, 5};
    Point dest = {-1, 5};
    char path[MAX_PATH_LENGTH];
    int length = find_path(source, dest, path, test_tile_blocked);
    mu_assert_int_eq(0, length);
    return NULL;
}

static const char *test_find_path_in_walled_dungeon(void) {
    setup_walled_dungeon();
    Point source = {3, 3};
    Point dest = {10, 10};
    char path[MAX_PATH_LENGTH];
    int length = find_path(source, dest, path, test_tile_blocked);
    mu_assert("should find path inside walled dungeon", length > 0);
    return NULL;
}

static const char *test_find_path_produces_valid_moves(void) {
    setup_clear_dungeon();
    Point source = {5, 5};
    Point dest = {8, 7};
    char path[MAX_PATH_LENGTH];
    int length = find_path(source, dest, path, test_tile_blocked);
    mu_assert("should find path", length > 0);

    /* Verify we can follow the path to the destination */
    int x = source.x, y = source.y;
    int i;
    for (i = 0; i < length; i++) {
        Point dir = get_direction_from_path((enum PATH_CODE)path[i]);
        x += dir.x;
        y += dir.y;
    }
    mu_assert_int_eq(dest.x, x);
    mu_assert_int_eq(dest.y, y);
    return NULL;
}

/* Node management tests */

static const char *test_get_new_node_initial(void) {
    path_nodes_in_use = 0;
    PathNode *node = get_new_node();
    mu_assert("get_new_node should return non-NULL", node != NULL);
    mu_assert_int_eq(1, path_nodes_in_use);
    return NULL;
}

static const char *test_get_new_node_exhausted(void) {
    path_nodes_in_use = MAX_NODES;
    PathNode *node = get_new_node();
    mu_assert("get_new_node should return NULL when exhausted", node == NULL);
    return NULL;
}

int main(void) {
    printf("\n=== pathfinding tests ===\n");

    /* heuristic */
    mu_run_test(test_heuristic_same_point);
    mu_run_test(test_heuristic_horizontal);
    mu_run_test(test_heuristic_vertical);
    mu_run_test(test_heuristic_diagonal);
    mu_run_test(test_heuristic_symmetric);

    /* path_check_equal */
    mu_run_test(test_path_check_equal_same_x);
    mu_run_test(test_path_check_equal_same_y);
    mu_run_test(test_path_check_equal_diagonal);

    /* find_path */
    mu_run_test(test_find_path_adjacent);
    mu_run_test(test_find_path_same_point);
    mu_run_test(test_find_path_short_distance);
    mu_run_test(test_find_path_diagonal);
    mu_run_test(test_find_path_around_obstacle);
    mu_run_test(test_find_path_negative_dest);
    mu_run_test(test_find_path_in_walled_dungeon);
    mu_run_test(test_find_path_produces_valid_moves);

    /* node management */
    mu_run_test(test_get_new_node_initial);
    mu_run_test(test_get_new_node_exhausted);

    mu_print_summary();
    return mu_tests_failed != 0;
}
