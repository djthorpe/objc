#include <hardware/platform_defs.h>
#include <hardware/regs/watchdog.h>
#include <hardware/watchdog.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

// Platform-specific constants from Pico SDK
#if PICO_RP2040
#define WATCHDOG_XFACTOR                                                       \
  2 // RP2040 decrements twice per tick (errata RP2040-E1)
#else
#define WATCHDOG_XFACTOR 1 // RP2350 decrements once per tick
#endif

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize the watchdog timer subsystem
 */
hw_watchdog_t hw_watchdog_init(uint32_t timeout_ms) {
  sys_assert(timeout_ms > 0);

  // Initialize the watchdog timer with the specified timeout
  hw_watchdog_t watchdog = {0};
  if (timeout_ms > hw_watchdog_maxtimeout()) {
    return watchdog; // Invalid timeout
  }

  // Set the timeout in milliseconds
  watchdog.timeout_ms = timeout_ms;

  // Return the initialized watchdog structure
  // but don't enable the watchdog yet
  return watchdog;
}

/**
 * @brief Finalize the watchdog timer subsystem
 */
void hw_watchdog_finalize(hw_watchdog_t *watchdog) {
  sys_assert(watchdog && hw_watchdog_valid(watchdog));

  // Disable the watchdog timer
  watchdog_disable();

  // Reset the watchdog structure
  sys_memset(watchdog, 0, sizeof(hw_watchdog_t));
}

/**
 * @brief Return the maximum supported watchdog timeout
 */
uint32_t hw_watchdog_maxtimeout(void) {
  // Use the actual SDK calculation: WATCHDOG_LOAD_BITS / (1000 *
  // WATCHDOG_XFACTOR) This gives us the true maximum based on hardware register
  // width and platform
  return WATCHDOG_LOAD_BITS / (1000 * WATCHDOG_XFACTOR);
}

/**
 * @brief Check if the watchdog triggered a system reset
 */
bool hw_watchdog_did_reset(hw_watchdog_t *watchdog) {
  sys_assert(watchdog && hw_watchdog_valid(watchdog));

  // Check if the watchdog caused the last reset
  return watchdog_enable_caused_reboot();
}

/**
 * @brief Enable or disable the watchdog timer
 */
void hw_watchdog_enable(hw_watchdog_t *watchdog, bool enable) {
  sys_assert(watchdog && hw_watchdog_valid(watchdog));

  if (enable) {
    // Enable the watchdog timer with the specified timeout and allow debug
    // pauses
    watchdog_enable(watchdog->timeout_ms, true);
  } else {
    // Disable the watchdog timer
    watchdog_disable();
  }
}

/**
 * @brief Ping the watchdog (reset the timeout counter)
 */
void hw_watchdog_ping(hw_watchdog_t *watchdog) {
  sys_assert(watchdog && hw_watchdog_valid(watchdog));

  // Reload the watchdog timer with the configured timeout value
  watchdog_update();
}
