#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Gets the current system time.
 */
bool sys_date_get_now(sys_date_t *date) {
  if (!date)
    return false;

  // Get current time with nanosecond precision
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    return false; // Return false if clock_gettime fails
  }

  // Get timezone offset using localtime_r
  struct tm local_tm;
  if (localtime_r(&ts.tv_sec, &local_tm) == NULL) {
    return false;
  }

  // Set values in sys_date_t structure
  date->seconds = (int64_t)ts.tv_sec;
  date->nanoseconds = (int32_t)ts.tv_nsec;
  date->tzoffset = (int32_t)local_tm.tm_gmtoff; // Timezone offset in seconds

  // Return true to indicate success
  return true;
}

/**
 * @brief Sets the current system time.
 */
bool sys_date_set_now(sys_date_t *date) {
  if (!date)
    return false;

  // Create timespec structure from sys_date_t
  struct timespec ts;
  ts.tv_sec =
      (time_t)(date->seconds - date->tzoffset); // Convert to UTC if needed
  ts.tv_nsec = (long)date->nanoseconds;

  // Set the system time (requires root privileges)
  if (clock_settime(CLOCK_REALTIME, &ts) != 0) {
    return false; // Failed to set system time
  }

  // Success
  return true;
}
