#include "minunit.h"
#include "../direction.h"

/* player_get_direction_from_path_code tests */

static const char *test_player_dir_from_path_up(void) {
    mu_assert_int_eq(PLAYER_NORTH_EAST_2,
                     player_get_direction_from_path_code(UP));
    return NULL;
}

static const char *test_player_dir_from_path_down(void) {
    mu_assert_int_eq(PLAYER_SOUTH_WEST_2,
                     player_get_direction_from_path_code(DOWN));
    return NULL;
}

static const char *test_player_dir_from_path_left(void) {
    mu_assert_int_eq(PLAYER_NORTH_WEST_2,
                     player_get_direction_from_path_code(LEFT));
    return NULL;
}

static const char *test_player_dir_from_path_right(void) {
    mu_assert_int_eq(PLAYER_SOUTH_EAST_2,
                     player_get_direction_from_path_code(RIGHT));
    return NULL;
}

static const char *test_player_dir_from_path_up_left(void) {
    mu_assert_int_eq(PLAYER_NORTH,
                     player_get_direction_from_path_code(UP_LEFT));
    return NULL;
}

static const char *test_player_dir_from_path_up_right(void) {
    mu_assert_int_eq(PLAYER_EAST,
                     player_get_direction_from_path_code(UP_RIGHT));
    return NULL;
}

static const char *test_player_dir_from_path_down_left(void) {
    mu_assert_int_eq(PLAYER_WEST,
                     player_get_direction_from_path_code(DOWN_LEFT));
    return NULL;
}

static const char *test_player_dir_from_path_down_right(void) {
    mu_assert_int_eq(PLAYER_SOUTH,
                     player_get_direction_from_path_code(DOWN_RIGHT));
    return NULL;
}

static const char *test_player_dir_from_path_default(void) {
    mu_assert_int_eq(PLAYER_SOUTH,
                     player_get_direction_from_path_code(PATH_NONE));
    return NULL;
}

/* monster_get_direction_from_path_code tests */

static const char *test_monster_dir_from_path_up(void) {
    mu_assert_int_eq(MONSTER_NORTH_EAST,
                     monster_get_direction_from_path_code(UP));
    return NULL;
}

static const char *test_monster_dir_from_path_down(void) {
    mu_assert_int_eq(MONSTER_SOUTH_WEST,
                     monster_get_direction_from_path_code(DOWN));
    return NULL;
}

static const char *test_monster_dir_from_path_left(void) {
    mu_assert_int_eq(MONSTER_NORTH_WEST,
                     monster_get_direction_from_path_code(LEFT));
    return NULL;
}

static const char *test_monster_dir_from_path_right(void) {
    mu_assert_int_eq(MONSTER_SOUTH_EAST,
                     monster_get_direction_from_path_code(RIGHT));
    return NULL;
}

static const char *test_monster_dir_from_path_up_left(void) {
    mu_assert_int_eq(MONSTER_NORTH,
                     monster_get_direction_from_path_code(UP_LEFT));
    return NULL;
}

static const char *test_monster_dir_from_path_down_right(void) {
    mu_assert_int_eq(MONSTER_SOUTH,
                     monster_get_direction_from_path_code(DOWN_RIGHT));
    return NULL;
}

/* player_get_direction8 tests */

static const char *test_player_dir8_east(void) {
    /* Target is directly to the right (+x) */
    mu_assert_int_eq(PLAYER_SOUTH_EAST_2,
                     player_get_direction8(0, 0, 10, 0));
    return NULL;
}

static const char *test_player_dir8_west(void) {
    /* Target is directly to the left (-x) */
    mu_assert_int_eq(PLAYER_NORTH_WEST_2,
                     player_get_direction8(0, 0, -10, 0));
    return NULL;
}

static const char *test_player_dir8_south(void) {
    /* Target is directly down (+y) */
    mu_assert_int_eq(PLAYER_SOUTH_WEST_2,
                     player_get_direction8(0, 0, 0, 10));
    return NULL;
}

static const char *test_player_dir8_north(void) {
    /* Target is directly up (-y) */
    mu_assert_int_eq(PLAYER_NORTH_EAST_2,
                     player_get_direction8(0, 0, 0, -10));
    return NULL;
}

static const char *test_player_dir8_south_east(void) {
    /* Target is to the bottom-right (+x, +y diagonal) */
    mu_assert_int_eq(PLAYER_SOUTH,
                     player_get_direction8(0, 0, 10, 10));
    return NULL;
}

static const char *test_player_dir8_north_west(void) {
    /* Target is to the top-left (-x, -y diagonal) */
    mu_assert_int_eq(PLAYER_NORTH,
                     player_get_direction8(0, 0, -10, -10));
    return NULL;
}

/* monster_get_direction8 tests */

static const char *test_monster_dir8_east(void) {
    mu_assert_int_eq(MONSTER_SOUTH_EAST,
                     monster_get_direction8(0, 0, 10, 0));
    return NULL;
}

static const char *test_monster_dir8_west(void) {
    mu_assert_int_eq(MONSTER_NORTH_WEST,
                     monster_get_direction8(0, 0, -10, 0));
    return NULL;
}

static const char *test_monster_dir8_south(void) {
    mu_assert_int_eq(MONSTER_SOUTH_WEST,
                     monster_get_direction8(0, 0, 0, 10));
    return NULL;
}

static const char *test_monster_dir8_north(void) {
    mu_assert_int_eq(MONSTER_NORTH_EAST,
                     monster_get_direction8(0, 0, 0, -10));
    return NULL;
}

static const char *test_monster_dir8_diagonal(void) {
    mu_assert_int_eq(MONSTER_SOUTH,
                     monster_get_direction8(0, 0, 10, 10));
    return NULL;
}

/* get_direction (angle-based) tests */

static const char *test_get_direction_east(void) {
    /* From origin, target is directly right */
    enum PLAYER_DIRECTION dir = get_direction(0, 0, 10, 0);
    mu_assert_int_eq(PLAYER_EAST, dir);
    return NULL;
}

static const char *test_get_direction_west(void) {
    /* From origin, target is directly left in the 16-direction system */
    enum PLAYER_DIRECTION dir = get_direction(0, 0, -10, 0);
    mu_assert_int_eq(PLAYER_WEST, dir);
    return NULL;
}

static const char *test_get_direction_same_point(void) {
    /* Same point - atan2(0,0) = 0, should return PLAYER_EAST */
    enum PLAYER_DIRECTION dir = get_direction(5, 5, 5, 5);
    mu_assert_int_eq(PLAYER_EAST, dir);
    return NULL;
}

int main(void) {
    printf("\n=== direction tests ===\n");

    /* player_get_direction_from_path_code */
    mu_run_test(test_player_dir_from_path_up);
    mu_run_test(test_player_dir_from_path_down);
    mu_run_test(test_player_dir_from_path_left);
    mu_run_test(test_player_dir_from_path_right);
    mu_run_test(test_player_dir_from_path_up_left);
    mu_run_test(test_player_dir_from_path_up_right);
    mu_run_test(test_player_dir_from_path_down_left);
    mu_run_test(test_player_dir_from_path_down_right);
    mu_run_test(test_player_dir_from_path_default);

    /* monster_get_direction_from_path_code */
    mu_run_test(test_monster_dir_from_path_up);
    mu_run_test(test_monster_dir_from_path_down);
    mu_run_test(test_monster_dir_from_path_left);
    mu_run_test(test_monster_dir_from_path_right);
    mu_run_test(test_monster_dir_from_path_up_left);
    mu_run_test(test_monster_dir_from_path_down_right);

    /* player_get_direction8 */
    mu_run_test(test_player_dir8_east);
    mu_run_test(test_player_dir8_west);
    mu_run_test(test_player_dir8_south);
    mu_run_test(test_player_dir8_north);
    mu_run_test(test_player_dir8_south_east);
    mu_run_test(test_player_dir8_north_west);

    /* monster_get_direction8 */
    mu_run_test(test_monster_dir8_east);
    mu_run_test(test_monster_dir8_west);
    mu_run_test(test_monster_dir8_south);
    mu_run_test(test_monster_dir8_north);
    mu_run_test(test_monster_dir8_diagonal);

    /* get_direction */
    mu_run_test(test_get_direction_east);
    mu_run_test(test_get_direction_west);
    mu_run_test(test_get_direction_same_point);

    mu_print_summary();
    return mu_tests_failed != 0;
}
