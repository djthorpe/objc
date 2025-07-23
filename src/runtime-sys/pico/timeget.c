#include <pico/aon_timer.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/*
 ** @brief Gets the current time based on the system's absolute time.
 */
bool sys_time_get_utc(sys_time_t *time) {
  if (time == NULL) {
    return false;
  }

  // Get the number of microseconds since boot
  struct timespec ts;
  if (!aon_timer_get_time(&ts)) {
    return false;
  }

  // Convert timespec to sys_time_t
  time->seconds = ts.tv_sec;
  time->nanoseconds = ts.tv_nsec;

  // Return success
  return true;
}
