#include <runtime-sys/sys.h>
#include <stdint.h>

#define NANOSECONDS_PER_SECOND 1000000000LL

int64_t sys_time_compare_ns(const sys_time_t *start, const sys_time_t *end) {
  sys_time_t start_time = {0, 0}; // Default to epoch (0 seconds, 0 nanoseconds)
  sys_time_t end_time;

  // Handle NULL start parameter - treat as epoch
  if (start != NULL) {
    start_time = *start;
  }

  // Handle NULL end parameter - treat as current time
  if (end != NULL) {
    end_time = *end;
  } else {
    if (!sys_time_get_utc(&end_time)) {
      // If we can't get current time, return 0 (no difference)
      return 0;
    }
  }

  // Calculate difference in seconds, then convert to nanoseconds
  int64_t seconds_diff = end_time.seconds - start_time.seconds;
  int64_t nanoseconds_diff = end_time.nanoseconds - start_time.nanoseconds;

  // Convert seconds to nanoseconds and add the nanoseconds difference
  int64_t total_diff_ns =
      (seconds_diff * NANOSECONDS_PER_SECOND) + nanoseconds_diff;

  return total_diff_ns;
}
