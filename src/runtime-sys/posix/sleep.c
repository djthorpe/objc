#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <time.h>
#include <stdint.h>

/**
 * Sleeps the current thread for a specified number of milliseconds.
 */
void sys_sleep(int32_t msec) {
  if (msec <= 0) {
    return;
  }

  struct timespec ts;
  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;

  int res;
  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);
}
