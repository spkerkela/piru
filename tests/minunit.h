/*
 * Minimal unit testing framework for C
 * Based on MinUnit (http://www.jera.com/techinfo/jtns/jtn002.html)
 * Extended with test suite support and colored output.
 */
#ifndef MINUNIT_H
#define MINUNIT_H

#include <stdio.h>
#include <string.h>

#define MU_COLOR_RED "\033[31m"
#define MU_COLOR_GREEN "\033[32m"
#define MU_COLOR_YELLOW "\033[33m"
#define MU_COLOR_RESET "\033[0m"

static int mu_tests_run = 0;
static int mu_tests_passed = 0;
static int mu_tests_failed = 0;
static int mu_asserts = 0;

#define mu_assert(message, test)                                               \
    do {                                                                        \
        mu_asserts++;                                                           \
        if (!(test)) {                                                          \
            printf("  " MU_COLOR_RED "FAIL" MU_COLOR_RESET ": %s\n"            \
                   "        %s:%d: %s\n",                                       \
                   __func__, __FILE__, __LINE__, message);                      \
            return message;                                                     \
        }                                                                       \
    } while (0)

#define mu_assert_int_eq(expected, actual)                                      \
    do {                                                                        \
        int _expected = (expected);                                             \
        int _actual = (actual);                                                 \
        mu_asserts++;                                                           \
        if (_expected != _actual) {                                             \
            printf("  " MU_COLOR_RED "FAIL" MU_COLOR_RESET ": %s\n"            \
                   "        %s:%d: expected %d, got %d\n",                      \
                   __func__, __FILE__, __LINE__, _expected, _actual);           \
            return "assertion failed";                                          \
        }                                                                       \
    } while (0)

#define mu_assert_double_eq(expected, actual, epsilon)                          \
    do {                                                                        \
        double _expected = (expected);                                          \
        double _actual = (actual);                                              \
        double _epsilon = (epsilon);                                            \
        mu_asserts++;                                                           \
        if ((_expected - _actual) > _epsilon ||                                 \
            (_actual - _expected) > _epsilon) {                                 \
            printf("  " MU_COLOR_RED "FAIL" MU_COLOR_RESET ": %s\n"            \
                   "        %s:%d: expected %f, got %f\n",                      \
                   __func__, __FILE__, __LINE__, _expected, _actual);           \
            return "assertion failed";                                          \
        }                                                                       \
    } while (0)

#define mu_run_test(test)                                                      \
    do {                                                                        \
        const char *message = test();                                           \
        mu_tests_run++;                                                         \
        if (message) {                                                          \
            mu_tests_failed++;                                                  \
        } else {                                                                \
            mu_tests_passed++;                                                  \
            printf("  " MU_COLOR_GREEN "PASS" MU_COLOR_RESET ": %s\n", #test); \
        }                                                                       \
    } while (0)

#define mu_print_summary()                                                     \
    do {                                                                        \
        printf("\n----------------------------------------\n");                \
        printf("Tests:   %d total, ", mu_tests_run);                           \
        if (mu_tests_passed > 0)                                               \
            printf(MU_COLOR_GREEN "%d passed" MU_COLOR_RESET, mu_tests_passed);\
        if (mu_tests_failed > 0) {                                             \
            if (mu_tests_passed > 0) printf(", ");                             \
            printf(MU_COLOR_RED "%d failed" MU_COLOR_RESET, mu_tests_failed);  \
        }                                                                       \
        printf("\n");                                                           \
        printf("Asserts: %d\n", mu_asserts);                                   \
        printf("----------------------------------------\n");                  \
    } while (0)

#endif /* MINUNIT_H */
