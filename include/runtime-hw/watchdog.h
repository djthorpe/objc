/**
 * @file watchdog.h
 * @brief Hardware Watchdog Timer Abstraction Layer
 * @defgroup Watchdog Watchdog
 * @ingroup Hardware
 *
 * Watchdog timer abstraction, which can be used to respond to dead processes
 *
 * The watchdog timer is a critical safety mechanism that can automatically
 * restart the system if software becomes unresponsive. Applications must
 * periodically "feed" the watchdog to prevent automatic system reset.
 *
 * @example examples/runtime/watchdog/main.c
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Watchdog adapter.
 * @ingroup Watchdog
 * @headerfile watchdog.h runtime-hw/hw.h
 */
typedef struct {
  uint32_t timeout_ms; ///< Watchdog timeout in milliseconds
} hw_watchdog_t;

/**
 * @brief Initialize the watchdog timer subsystem
 * @ingroup Watchdog
 * @param timeout_ms Watchdog timeout in milliseconds
 * @return true if initialization successful, false otherwise
 *
 * Performs platform-specific initialization of the watchdog hardware.
 */
hw_watchdog_t hw_watchdog_init(uint32_t timeout_ms);

/**
 * @brief Finalize the watchdog timer subsystem
 * @ingroup Watchdog
 * @param watchdog Pointer to the watchdog structure to finalize
 *
 * Disables the watchdog. After this call,
 * the watchdog cannot be used until re-initialized.
 */
void hw_watchdog_finalize(hw_watchdog_t *watchdog);

/**
 * @brief Return the maximum supported watchdog timeout
 * @ingroup Watchdog
 * @param timeout_ms Watchdog timeout in milliseconds
 * @return The maximum supported watchdog timeout in milliseconds, or 0 if
 * watchdogs are not supported on this platform.
 */
uint32_t hw_watchdog_maxtimeout(void);

/**
 * @brief Check if the watchdog triggered a system reset
 * @ingroup Watchdog
 * @param watchdog Pointer to the watchdog structure
 * @return true if the last reboot was triggered by the watchdog, false
 * otherwise
 *
 * This function checks if the last system reboot was caused by the watchdog
 * timer. It is useful for debugging and ensuring that the watchdog is
 * functioning correctly.
 */
bool hw_watchdog_did_reset(hw_watchdog_t *watchdog);

/**
 * @brief Enable or disable the watchdog timer
 * @ingroup Watchdog
 * @param watchdog Pointer to the watchdog structure
 * @param enable If true, enables the watchdog timer; if false, disables it.
 *
 * Starts or stops the watchdog countdown timer. Once enabled, the application
 * must call hw_watchdog_ping() periodically to prevent system reset, usually
 * by using a periodic timer.
 */
void hw_watchdog_enable(hw_watchdog_t *watchdog, bool enable);

/**
 * @brief Ping the watchdog (reset the timeout counter)
 * @ingroup Watchdog
 * @param watchdog Pointer to the watchdog structure
 *
 * Reloads the watchdog timer with the configured timeout value, preventing
 * an imminent reset. This function must be called regularly by the application
 * to demonstrate that the system is operating correctly.
 */
void hw_watchdog_ping(hw_watchdog_t *watchdog);

/**
 * @brief Check if a watchdog structure is valid
 * @ingroup Watchdog
 * @param watchdog Pointer to the watchdog structure to validate
 * @return true if the watchdog structure is valid and usable, false otherwise
 */
static inline bool hw_watchdog_valid(hw_watchdog_t *watchdog) {
  return watchdog && watchdog->timeout_ms > 0;
}

#ifdef __cplusplus
}
#endif
