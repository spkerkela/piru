#include "piru_math.h"
int piru_min(int a, int b) { return a > b ? b : a; }
int piru_max(int a, int b) { return a > b ? a : b; }

// Assumes 0 <= max <= RAND_MAX
// Returns in the closed interval [0, max]
long random_at_most(long max) {
  unsigned long
      // max <= RAND_MAX < ULONG_MAX, so this is okay.
      num_bins = (unsigned long)max + 1,
      num_rand = (unsigned long)RAND_MAX + 1, bin_size = num_rand / num_bins,
      defect = num_rand % num_bins;

  long x;
  do {
    x = rand();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x / bin_size;
}

long random_between(long min, long max) {
  return random_at_most(max + 1 - min) + min;
}