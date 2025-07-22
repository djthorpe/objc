#include <sys/sys.h>
#include <unistd.h>

/**
 * @brief Returns the number of CPU cores available on the host system.
 */
uint8_t sys_thread_numcores(void) {
  int num_cores = 0;

  // Method 1: Get number of currently online processors
  long cores = sysconf(_SC_NPROCESSORS_ONLN);
  if (cores > 0) {
    num_cores = (int)cores;
  }

  // Method 2: Fallback to configured processors if online count failed
  if (num_cores <= 0) {
    cores = sysconf(_SC_NPROCESSORS_CONF);
    if (cores > 0) {
      num_cores = (int)cores;
    }
  }

  // Ensure we return a reasonable value
  if (num_cores < 1) {
    return 1; // Ensure at least one core is returned
  } else if (num_cores > UINT8_MAX) {
    return UINT8_MAX; // Cap at maximum uint8_t value
  }

  return (uint8_t)num_cores;
}
