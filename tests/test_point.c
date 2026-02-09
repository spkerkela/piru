#include "minunit.h"
#include "../point.h"

/* cartesian_to_isometric tests */

static const char *test_cartesian_to_isometric_origin(void) {
    Point p = {0, 0};
    Point result = cartesian_to_isometric(p);
    mu_assert_int_eq(0, result.x);
    mu_assert_int_eq(0, result.y);
    return NULL;
}

static const char *test_cartesian_to_isometric_unit_x(void) {
    Point p = {1, 0};
    Point result = cartesian_to_isometric(p);
    /* (1 - 0) * TILE_WIDTH_HALF = 1 * 32 = 32 */
    mu_assert_int_eq(TILE_WIDTH_HALF, result.x);
    /* (1 + 0) * TILE_HEIGHT_HALF = 1 * 16 = 16 */
    mu_assert_int_eq(TILE_HEIGHT_HALF, result.y);
    return NULL;
}

static const char *test_cartesian_to_isometric_unit_y(void) {
    Point p = {0, 1};
    Point result = cartesian_to_isometric(p);
    /* (0 - 1) * 32 = -32 */
    mu_assert_int_eq(-TILE_WIDTH_HALF, result.x);
    /* (0 + 1) * 16 = 16 */
    mu_assert_int_eq(TILE_HEIGHT_HALF, result.y);
    return NULL;
}

static const char *test_cartesian_to_isometric_diagonal(void) {
    Point p = {1, 1};
    Point result = cartesian_to_isometric(p);
    /* (1 - 1) * 32 = 0 */
    mu_assert_int_eq(0, result.x);
    /* (1 + 1) * 16 = 32 */
    mu_assert_int_eq(2 * TILE_HEIGHT_HALF, result.y);
    return NULL;
}

static const char *test_cartesian_to_isometric_large_values(void) {
    Point p = {10, 5};
    Point result = cartesian_to_isometric(p);
    mu_assert_int_eq((10 - 5) * TILE_WIDTH_HALF, result.x);
    mu_assert_int_eq((10 + 5) * TILE_HEIGHT_HALF, result.y);
    return NULL;
}

/* isometric_to_cartesian tests */

static const char *test_isometric_to_cartesian_origin(void) {
    Point p = {0, 0};
    Point result = isometric_to_cartesian(p);
    mu_assert_int_eq(0, result.x);
    mu_assert_int_eq(0, result.y);
    return NULL;
}

static const char *test_isometric_to_cartesian_roundtrip(void) {
    Point original = {5, 3};
    Point iso = cartesian_to_isometric(original);
    Point back = isometric_to_cartesian(iso);
    mu_assert_int_eq(original.x, back.x);
    mu_assert_int_eq(original.y, back.y);
    return NULL;
}

static const char *test_isometric_to_cartesian_roundtrip_diagonal(void) {
    Point original = {7, 7};
    Point iso = cartesian_to_isometric(original);
    Point back = isometric_to_cartesian(iso);
    mu_assert_int_eq(original.x, back.x);
    mu_assert_int_eq(original.y, back.y);
    return NULL;
}

/* get_tile_coordinates tests */

static const char *test_get_tile_coordinates_origin(void) {
    Point p = {0, 0};
    Point result = get_tile_coordinates(p);
    mu_assert_int_eq(0, result.x);
    mu_assert_int_eq(0, result.y);
    return NULL;
}

static const char *test_get_tile_coordinates_within_tile(void) {
    Point p = {30, 15};
    Point result = get_tile_coordinates(p);
    /* 30/64 = 0, 15/32 = 0 */
    mu_assert_int_eq(0, result.x);
    mu_assert_int_eq(0, result.y);
    return NULL;
}

static const char *test_get_tile_coordinates_exact_boundary(void) {
    Point p = {TILE_WIDTH, TILE_HEIGHT};
    Point result = get_tile_coordinates(p);
    mu_assert_int_eq(1, result.x);
    mu_assert_int_eq(1, result.y);
    return NULL;
}

static const char *test_get_tile_coordinates_multiple_tiles(void) {
    Point p = {TILE_WIDTH * 3 + 10, TILE_HEIGHT * 2 + 5};
    Point result = get_tile_coordinates(p);
    mu_assert_int_eq(3, result.x);
    mu_assert_int_eq(2, result.y);
    return NULL;
}

/* get_distance tests */

static const char *test_get_distance_same_point(void) {
    Point a = {5, 5};
    Point b = {5, 5};
    mu_assert_double_eq(0.0, get_distance(a, b), 0.001);
    return NULL;
}

static const char *test_get_distance_horizontal(void) {
    Point a = {0, 0};
    Point b = {3, 0};
    mu_assert_double_eq(3.0, get_distance(a, b), 0.001);
    return NULL;
}

static const char *test_get_distance_vertical(void) {
    Point a = {0, 0};
    Point b = {0, 4};
    mu_assert_double_eq(4.0, get_distance(a, b), 0.001);
    return NULL;
}

static const char *test_get_distance_diagonal(void) {
    Point a = {0, 0};
    Point b = {3, 4};
    mu_assert_double_eq(5.0, get_distance(a, b), 0.001);
    return NULL;
}

static const char *test_get_distance_symmetric(void) {
    Point a = {2, 3};
    Point b = {7, 11};
    mu_assert_double_eq(get_distance(a, b), get_distance(b, a), 0.001);
    return NULL;
}

/* point_equal tests */

static const char *test_point_equal_same(void) {
    Point a = {5, 10};
    Point b = {5, 10};
    mu_assert("equal points should return true", point_equal(a, b));
    return NULL;
}

static const char *test_point_equal_different_x(void) {
    Point a = {5, 10};
    Point b = {6, 10};
    mu_assert("different x should return false", !point_equal(a, b));
    return NULL;
}

static const char *test_point_equal_different_y(void) {
    Point a = {5, 10};
    Point b = {5, 11};
    mu_assert("different y should return false", !point_equal(a, b));
    return NULL;
}

static const char *test_point_equal_both_different(void) {
    Point a = {1, 2};
    Point b = {3, 4};
    mu_assert("both different should return false", !point_equal(a, b));
    return NULL;
}

static const char *test_point_equal_origin(void) {
    Point a = {0, 0};
    Point b = {0, 0};
    mu_assert("origin points should be equal", point_equal(a, b));
    return NULL;
}

/* get_direction_from_path tests */

static const char *test_direction_from_path_up(void) {
    Point p = get_direction_from_path(UP);
    mu_assert_int_eq(0, p.x);
    mu_assert_int_eq(-1, p.y);
    return NULL;
}

static const char *test_direction_from_path_down(void) {
    Point p = get_direction_from_path(DOWN);
    mu_assert_int_eq(0, p.x);
    mu_assert_int_eq(1, p.y);
    return NULL;
}

static const char *test_direction_from_path_left(void) {
    Point p = get_direction_from_path(LEFT);
    mu_assert_int_eq(-1, p.x);
    mu_assert_int_eq(0, p.y);
    return NULL;
}

static const char *test_direction_from_path_right(void) {
    Point p = get_direction_from_path(RIGHT);
    mu_assert_int_eq(1, p.x);
    mu_assert_int_eq(0, p.y);
    return NULL;
}

static const char *test_direction_from_path_up_left(void) {
    Point p = get_direction_from_path(UP_LEFT);
    mu_assert_int_eq(-1, p.x);
    mu_assert_int_eq(-1, p.y);
    return NULL;
}

static const char *test_direction_from_path_up_right(void) {
    Point p = get_direction_from_path(UP_RIGHT);
    mu_assert_int_eq(1, p.x);
    mu_assert_int_eq(-1, p.y);
    return NULL;
}

static const char *test_direction_from_path_down_left(void) {
    Point p = get_direction_from_path(DOWN_LEFT);
    mu_assert_int_eq(-1, p.x);
    mu_assert_int_eq(1, p.y);
    return NULL;
}

static const char *test_direction_from_path_down_right(void) {
    Point p = get_direction_from_path(DOWN_RIGHT);
    mu_assert_int_eq(1, p.x);
    mu_assert_int_eq(1, p.y);
    return NULL;
}

static const char *test_direction_from_path_none(void) {
    Point p = get_direction_from_path(PATH_NONE);
    mu_assert_int_eq(0, p.x);
    mu_assert_int_eq(0, p.y);
    return NULL;
}

/* get_direction_from_player_direction tests */

static const char *test_direction_from_player_north(void) {
    Point p = get_direction_from_player_direction(PLAYER_NORTH);
    mu_assert_int_eq(-1, p.x);
    mu_assert_int_eq(-1, p.y);
    return NULL;
}

static const char *test_direction_from_player_south(void) {
    Point p = get_direction_from_player_direction(PLAYER_SOUTH);
    mu_assert_int_eq(1, p.x);
    mu_assert_int_eq(1, p.y);
    return NULL;
}

static const char *test_direction_from_player_east(void) {
    Point p = get_direction_from_player_direction(PLAYER_EAST);
    mu_assert_int_eq(1, p.x);
    mu_assert_int_eq(-1, p.y);
    return NULL;
}

static const char *test_direction_from_player_west(void) {
    Point p = get_direction_from_player_direction(PLAYER_WEST);
    mu_assert_int_eq(-1, p.x);
    mu_assert_int_eq(1, p.y);
    return NULL;
}

int main(void) {
    printf("\n=== point tests ===\n");

    /* cartesian_to_isometric */
    mu_run_test(test_cartesian_to_isometric_origin);
    mu_run_test(test_cartesian_to_isometric_unit_x);
    mu_run_test(test_cartesian_to_isometric_unit_y);
    mu_run_test(test_cartesian_to_isometric_diagonal);
    mu_run_test(test_cartesian_to_isometric_large_values);

    /* isometric_to_cartesian */
    mu_run_test(test_isometric_to_cartesian_origin);
    mu_run_test(test_isometric_to_cartesian_roundtrip);
    mu_run_test(test_isometric_to_cartesian_roundtrip_diagonal);

    /* get_tile_coordinates */
    mu_run_test(test_get_tile_coordinates_origin);
    mu_run_test(test_get_tile_coordinates_within_tile);
    mu_run_test(test_get_tile_coordinates_exact_boundary);
    mu_run_test(test_get_tile_coordinates_multiple_tiles);

    /* get_distance */
    mu_run_test(test_get_distance_same_point);
    mu_run_test(test_get_distance_horizontal);
    mu_run_test(test_get_distance_vertical);
    mu_run_test(test_get_distance_diagonal);
    mu_run_test(test_get_distance_symmetric);

    /* point_equal */
    mu_run_test(test_point_equal_same);
    mu_run_test(test_point_equal_different_x);
    mu_run_test(test_point_equal_different_y);
    mu_run_test(test_point_equal_both_different);
    mu_run_test(test_point_equal_origin);

    /* get_direction_from_path */
    mu_run_test(test_direction_from_path_up);
    mu_run_test(test_direction_from_path_down);
    mu_run_test(test_direction_from_path_left);
    mu_run_test(test_direction_from_path_right);
    mu_run_test(test_direction_from_path_up_left);
    mu_run_test(test_direction_from_path_up_right);
    mu_run_test(test_direction_from_path_down_left);
    mu_run_test(test_direction_from_path_down_right);
    mu_run_test(test_direction_from_path_none);

    /* get_direction_from_player_direction */
    mu_run_test(test_direction_from_player_north);
    mu_run_test(test_direction_from_player_south);
    mu_run_test(test_direction_from_player_east);
    mu_run_test(test_direction_from_player_west);

    mu_print_summary();
    return mu_tests_failed != 0;
}
