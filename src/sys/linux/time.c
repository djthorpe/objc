// Ensure POSIX time constants are available
#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <sys/sys.h>

typedef struct {
    struct timespec ts;
} systime_t;

systime_t sys_time_now(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        // If clock_gettime fails, return a zero-initialized timespec
        ts.tv_sec = 0;
        ts.tv_nsec = 0;
    }
    return (systime_t){ .ts = ts };
}
