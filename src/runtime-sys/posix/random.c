#include <limits.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

static inline void _sys_random_init(void) {
  static bool init = false;
  if (!init) {
    // Initialize the random number generator if not already done
    srand((unsigned int)time(NULL));
    init = true;
  }
}

/**
 * @brief Returns a random number as a 32-bit unsigned integer.
 * @note This function is not thread-safe.
 */
uint32_t sys_random_uint32(void) {
  _sys_random_init();

  // Optimize for common case where int is 32-bit
  if (RAND_MAX == INT32_MAX) {
    return ((uint32_t)rand() & 0xFFFF) << 16 | ((uint32_t)rand() & 0xFFFF);
  }

  sys_panicf("sys_random_uint32: RAND_MAX is too small");
}

/**
 * @brief Returns a random number as a 64-bit unsigned integer.
 * @note This function is not thread-safe.
 */
uint64_t sys_random_uint64(void) {
  _sys_random_init();

  // Generate a 64-bit random number by combining multiple rand() calls
  // This approach works regardless of RAND_MAX size
  uint64_t result = 0;

  // Build the 64-bit number in 16-bit chunks to ensure we get full coverage
  // regardless of RAND_MAX value
  for (int i = 0; i < 4; i++) {
    result = (result << 16) | ((uint64_t)rand() & 0xFFFF);
  }

  return result;
}
