// Ensure POSIX time constants are available
#define _POSIX_C_SOURCE 199309L
#ifdef __APPLE__
// On macOS, timegm is available as a BSD extension
#define _DARWIN_C_SOURCE
#else
// On Linux, use GNU extensions for timegm
#define _GNU_SOURCE
#endif
#include <runtime-sys/sys.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

bool sys_time_get_utc(sys_time_t *time) {
  if (time == NULL) {
    return false;
  }

  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    return false; // Return false if clock_gettime fails
  }

  time->seconds = (int64_t)ts.tv_sec;
  time->nanoseconds = (int32_t)ts.tv_nsec;
  return true;
}
