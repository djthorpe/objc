#include "power.h"
#include <hardware/platform_defs.h>
#include <hardware/regs/watchdog.h>
#include <hardware/watchdog.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Handle for watchdog timer state.
 */
typedef struct hw_watchdog_t {
  uint64_t timeout_ms;
  uint64_t timestamp;
  bool disable;
} hw_watchdog_t;

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// Platform-specific constants from Pico SDK
#if PICO_RP2040
#define WATCHDOG_XFACTOR                                                       \
  2 // RP2040 decrements twice per tick (errata RP2040-E1)
#else
#define WATCHDOG_XFACTOR 1 // RP2350 decrements once per tick
#endif

// The watchdog singleton
static hw_watchdog_t _hw_watchdog = {0};

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize the watchdog timer subsystem
 */
hw_watchdog_t *hw_watchdog_init() {
  hw_watchdog_t *watchdog = &_hw_watchdog;

  // Finalize the existing watchdog
  hw_watchdog_finalize(watchdog);

  // Initialize the watchdog timer with the specified timeout,
  // which is half the maximum timeout
  uint64_t timeout_ms = hw_watchdog_maxtimeout() >> 1;
  if (timeout_ms == 0) {
    return NULL;
  } else {
    watchdog->timeout_ms = timeout_ms;
  }

  // Return the initialized watchdog structure
  // but don't enable the watchdog yet
  return watchdog;
}

/**
 * @brief Finalize the watchdog timer subsystem
 */
void hw_watchdog_finalize(hw_watchdog_t *watchdog) {
  if (hw_watchdog_valid(watchdog) == false) {
    return;
  }

  // Disable the watchdog timer
  watchdog_disable();

  // Reset the watchdog structure
  sys_memset(watchdog, 0, sizeof(hw_watchdog_t));
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

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
 * @brief Return the maximum supported watchdog timeout
 */
bool hw_watchdog_valid(hw_watchdog_t *watchdog) {
  if (watchdog == NULL) {
    return false;
  }
  if (watchdog->timeout_ms == 0) {
    return false;
  }
  return true;
}

/**
 * @brief Check if the watchdog triggered a system reset
 */
bool hw_watchdog_did_reset(hw_watchdog_t *watchdog) {
  sys_assert(hw_watchdog_valid(watchdog));

  // Check if the watchdog caused the last reset
  return watchdog_enable_caused_reboot();
}

/**
 * @brief Enable or disable the watchdog timer
 */
void hw_watchdog_enable(hw_watchdog_t *watchdog, bool enable) {
  sys_assert(hw_watchdog_valid(watchdog));

  if (enable) {
    // Enable the watchdog timer with the specified timeout and allow debug
    // pauses
    watchdog_enable(watchdog->timeout_ms, true);
    watchdog->disable = false;
  } else {
    // Disable the watchdog timer
    watchdog_disable();
    watchdog->disable = true;
  }
}

/**
 * @brief Cause a reset after a delay
 */
void hw_watchdog_reset(hw_watchdog_t *watchdog, uint32_t delay_ms) {
  sys_assert(hw_watchdog_valid(watchdog));
  sys_assert(delay_ms > 0);

  // Clamp the delay to the maximum supported timeout
  if (delay_ms > hw_watchdog_maxtimeout()) {
    delay_ms = hw_watchdog_maxtimeout();
  }

  // Enable the watchdog timer with the specified timeout and allow debug
  // pauses, and stop pinging the timer in hw_poll()
  watchdog_enable(delay_ms, true);
  watchdog->disable = true;

  // Notify the power management system of the reset
  _hw_power_notify_reset(delay_ms);
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Poll the watchdog (reset the timeout counter)
 */
void _hw_watchdog_poll() {
  hw_watchdog_t *watchdog = &_hw_watchdog;
  if (hw_watchdog_valid(watchdog) == false) {
    return;
  }
  if (watchdog->disable) {
    return;
  }

  // Determine if we should update
  uint64_t timestamp = sys_date_get_timestamp();
  if (watchdog->timestamp != 0 &&
      timestamp < watchdog->timestamp + watchdog->timeout_ms) {
    return;
  } else {
    watchdog->timestamp = timestamp;
  }

  // Update the watchdog timer
  watchdog_update();
}
