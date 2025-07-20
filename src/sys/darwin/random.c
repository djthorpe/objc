#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/sys.h>
#include <time.h>

/**
 * @brief Returns a random number as a 32-bit unsigned integer.
 * @note This function is not thread-safe.
 */
uint32_t sys_random_uint32(void) {
  static bool init = false;
  if (!init) {
    // Initialize the random number generator if not already done
    srand((unsigned int)time(NULL));
    init = true;
  }

  // Optimize for common case where int is 32-bit
  if (RAND_MAX == INT32_MAX) {
    return ((uint32_t)rand() & 0xFFFF) << 16 | ((uint32_t)rand() & 0xFFFF);
  }

  sys_panicf("sys_random_uint32: RAND_MAX is too small");
}
