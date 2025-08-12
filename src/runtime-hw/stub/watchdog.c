#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

// Opaque stub watchdog storage
static uint8_t _hw_watchdog_stub_storage[64];
static bool _hw_watchdog_stub_initialized = false;

/**
 * @brief Initialize the watchdog timer subsystem (stub)
 */
hw_watchdog_t *hw_watchdog_init() {
  _hw_watchdog_stub_initialized = true;
  return (hw_watchdog_t *)_hw_watchdog_stub_storage;
}

/**
 * @brief Finalize the watchdog timer subsystem
 */
void hw_watchdog_finalize(hw_watchdog_t *watchdog) {
  (void)watchdog;
  _hw_watchdog_stub_initialized = false;
}

/**
 * @brief Return the maximum supported watchdog timeout
 */
uint32_t hw_watchdog_maxtimeout(void) { return 0; }

bool hw_watchdog_valid(hw_watchdog_t *watchdog) {
  (void)watchdog;
  return _hw_watchdog_stub_initialized;
}

/**
 * @brief Check if the watchdog triggered a system reset
 */
bool hw_watchdog_did_reset(hw_watchdog_t *watchdog) {
  (void)watchdog;
  return false;
}

/**
 * @brief Enable or disable the watchdog timer
 */
void hw_watchdog_enable(hw_watchdog_t *watchdog, bool enable) {
  (void)watchdog;
  (void)enable;
}

/**
 * @brief Ping the watchdog (reset the timeout counter)
 */
void hw_watchdog_reset(hw_watchdog_t *watchdog, uint32_t delay_ms) {
  (void)watchdog;
  (void)delay_ms;
}

// (No watchdog polling in stub; ping/reset no-ops)
