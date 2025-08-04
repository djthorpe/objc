#include <runtime-sys/sys.h>
#include <stdint.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Gets the number of milliseconds since the process launched.
 *
 * @note The function automatically initializes its reference point on the first
 * call. Subsequent calls return the elapsed time since that first call.
 *
 * @return The number of milliseconds elapsed since the process started,
 *         or 0 if the timestamp could not be retrieved.
 */
uint64_t sys_date_get_timestamp() {
  static uint64_t process_start_time_ms = 0;

  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
    // Use explicit 64-bit arithmetic to prevent overflow
    uint64_t current_time_ms =
        (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;

    // Initialize process start time on first call (when it's zero)
    if (process_start_time_ms == 0) {
      process_start_time_ms = current_time_ms;
    }

    // Return time elapsed since process start
    return current_time_ms - process_start_time_ms;
  }
  return 0;
}
