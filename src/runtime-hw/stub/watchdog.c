#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize the watchdog timer subsystem
 */
hw_watchdog_t hw_watchdog_init(uint32_t timeout_ms) {
  // Watchdog not implemented in stub
  (void)timeout_ms; // Suppress unused parameter warning
  hw_watchdog_t watchdog = {0};
  return watchdog;
}

/**
 * @brief Finalize the watchdog timer subsystem
 */
void hw_watchdog_finalize(hw_watchdog_t *watchdog) {
  // Watchdog not implemented in stub
  sys_assert(watchdog);
  (void)watchdog;
}

/**
 * @brief Return the maximum supported watchdog timeout
 */
uint32_t hw_watchdog_maxtimeout(void) {
  // Watchdog not implemented in stub
  return 0;
}

/**
 * @brief Check if the watchdog triggered a system reset
 */
bool hw_watchdog_did_reset(hw_watchdog_t *watchdog) {
  return false; // Watchdog not implemented in stub
}

/**
 * @brief Enable or disable the watchdog timer
 */
void hw_watchdog_enable(hw_watchdog_t *watchdog, bool enable) {
  // Watchdog not implemented in stub
  sys_assert(watchdog);
  (void)enable; // Suppress unused parameter warning
}

/**
 * @brief Ping the watchdog (reset the timeout counter)
 */
void hw_watchdog_ping(hw_watchdog_t *watchdog) {
  // Watchdog not implemented in stub
  sys_assert(watchdog);
}
