#include "minunit.h"
#include "../piru_math.h"

static const char *test_piru_min_first_smaller(void) {
    mu_assert_int_eq(3, piru_min(3, 5));
    return NULL;
}

static const char *test_piru_min_second_smaller(void) {
    mu_assert_int_eq(2, piru_min(7, 2));
    return NULL;
}

static const char *test_piru_min_equal(void) {
    mu_assert_int_eq(4, piru_min(4, 4));
    return NULL;
}

static const char *test_piru_min_negative(void) {
    mu_assert_int_eq(-5, piru_min(-5, -3));
    return NULL;
}

static const char *test_piru_min_zero(void) {
    mu_assert_int_eq(0, piru_min(0, 5));
    return NULL;
}

static const char *test_piru_max_first_larger(void) {
    mu_assert_int_eq(5, piru_max(5, 3));
    return NULL;
}

static const char *test_piru_max_second_larger(void) {
    mu_assert_int_eq(7, piru_max(2, 7));
    return NULL;
}

static const char *test_piru_max_equal(void) {
    mu_assert_int_eq(4, piru_max(4, 4));
    return NULL;
}

static const char *test_piru_max_negative(void) {
    mu_assert_int_eq(-3, piru_max(-5, -3));
    return NULL;
}

static const char *test_piru_max_zero(void) {
    mu_assert_int_eq(5, piru_max(0, 5));
    return NULL;
}

static const char *test_random_at_most_range(void) {
    srand(42);
    int i;
    for (i = 0; i < 1000; i++) {
        long val = random_at_most(10);
        mu_assert("random_at_most should return value >= 0", val >= 0);
        mu_assert("random_at_most should return value <= max", val <= 10);
    }
    return NULL;
}

static const char *test_random_at_most_zero(void) {
    long val = random_at_most(0);
    mu_assert_int_eq(0, (int)val);
    return NULL;
}

static const char *test_random_between_range(void) {
    srand(42);
    int i;
    for (i = 0; i < 1000; i++) {
        long val = random_between(5, 15);
        mu_assert("random_between should return value >= min", val >= 5);
        mu_assert("random_between should return value <= max", val <= 15);
    }
    return NULL;
}

static const char *test_random_between_negative_range(void) {
    srand(42);
    int i;
    for (i = 0; i < 1000; i++) {
        long val = random_between(-10, -5);
        mu_assert("random_between should handle negative min", val >= -10);
        mu_assert("random_between should handle negative max", val <= -5);
    }
    return NULL;
}

static const char *test_random_between_same_values(void) {
    long val = random_between(7, 7);
    mu_assert_int_eq(7, (int)val);
    return NULL;
}

int main(void) {
    printf("\n=== piru_math tests ===\n");

    mu_run_test(test_piru_min_first_smaller);
    mu_run_test(test_piru_min_second_smaller);
    mu_run_test(test_piru_min_equal);
    mu_run_test(test_piru_min_negative);
    mu_run_test(test_piru_min_zero);
    mu_run_test(test_piru_max_first_larger);
    mu_run_test(test_piru_max_second_larger);
    mu_run_test(test_piru_max_equal);
    mu_run_test(test_piru_max_negative);
    mu_run_test(test_piru_max_zero);
    mu_run_test(test_random_at_most_range);
    mu_run_test(test_random_at_most_zero);
    mu_run_test(test_random_between_range);
    mu_run_test(test_random_between_negative_range);
    mu_run_test(test_random_between_same_values);

    mu_print_summary();
    return mu_tests_failed != 0;
}
