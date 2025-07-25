#include "hardware/platform_defs.h"
#include <pico/stdlib.h>

/**
 * @brief Returns the number of CPU cores available on the host system.
 * @return The number of CPU cores available on the system. Returns 1 if the
 *         number of cores cannot be determined or if the system has only one
 *         core.
 *
 * This function queries the system to determine the total number of processing
 * cores available.
 */
uint8_t sys_thread_numcores() {
  // RP2040 has exactly NUM_CORES (2) cores
  return (uint8_t)NUM_CORES;
}

/**
 * @brief Pauses the execution of the current thread for a specified time.
 */
void sys_sleep(int32_t msec) { sleep_ms(msec); }
