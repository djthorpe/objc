#include <pico/sync.h>
#include <pico/time.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

static int64_t _sys_date = 0;     // Date offset in seconds
static int32_t _sys_tzoffset = 0; // Timezone offset in seconds
static mutex_t mtx;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Initializes the date module.
 */
void _sys_date_init() { mutex_init(&mtx); }

/**
 * @brief Exits the date module.
 */
void _sys_date_exit() {
  // Nothing to do here, but could be used for cleanup if needed
}

/**
 * @brief Gets the number of milliseconds since the process launched.
 */
uint64_t sys_date_get_timestamp() {
  absolute_time_t now = get_absolute_time();
  return to_us_since_boot(now) / 1000;
}

/**
 * @brief Gets the current system time.
 */
bool sys_date_get_now(sys_date_t *date) {
  if (date == NULL) {
    return false;
  }

  mutex_enter_blocking(&mtx);

  // Get the time in milliseconds since boot plus the offset
  absolute_time_t now = get_absolute_time();
  int64_t ms = (to_us_since_boot(now) / 1000) + (_sys_date * 1000);

  // Convert to sys_date_t
  date->seconds = (ms / 1000); // Convert milliseconds to seconds
  date->nanoseconds =
      (int32_t)(ms % 1000) * 1000000; // Convert milliseconds to nanoseconds
  date->tzoffset = _sys_tzoffset;     // Set timezone offset

  mutex_exit(&mtx);

  // Return success
  return true;
}

/**
 * @brief Sets the current system time.
 */
bool sys_date_set_now(const sys_date_t *date) {
  if (date == NULL) {
    return false;
  }

  mutex_enter_blocking(&mtx);

  // Set the system date and time based on the provided sys_date_t
  // Calculate the offset needed to make current time match the desired time
  absolute_time_t now = get_absolute_time();
  int64_t current_ms = to_us_since_boot(now) / 1000;
  int64_t desired_ms = (date->seconds * 1000) + (date->nanoseconds / 1000000);
  _sys_date = (desired_ms - current_ms) / 1000; // Store offset in seconds
  _sys_tzoffset = date->tzoffset;               // Set timezone offset

  mutex_exit(&mtx);

  // Return success
  return true;
}
