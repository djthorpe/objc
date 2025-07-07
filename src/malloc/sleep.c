#include <errno.h>
#include <time.h>

/**
 * Sleeps the current thread for a specified number of milliseconds.
 */
void objc_sleep(unsigned int msec) {
  struct timespec ts;
  int res;

  if (msec == 0) {
    return;
  }

  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;
  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);
}
