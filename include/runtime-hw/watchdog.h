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
 * Typically you would use a periodic timer to call hw_watchdog_ping()
 * and keep your fingers crossed that the timer fires on time.
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
typedef struct hw_watchdog_t hw_watchdog_t;

/**
 * @brief Initialize the watchdog timer subsystem
 * @ingroup Watchdog
 * @param timeout_ms Watchdog timeout in milliseconds
 * @return Pointer to a hw_watchdog_t handle, or NULL if watchdog is not
 * supported
 *
 * Performs platform-specific initialization of the watchdog hardware.
 * Returns NULL if watchdog is not supported or initialization failed.
 *
 * In order for the watchdog to function, hw_poll() must be called periodically.
 */
hw_watchdog_t *hw_watchdog_init();

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
 * @return The maximum supported watchdog timeout in milliseconds, or 0 if
 * watchdogs are not supported on this platform.
 */
uint32_t hw_watchdog_maxtimeout(void);

/**
 * @brief Check if the watchdog is in a valid state
 * @ingroup Watchdog
 * @param watchdog Pointer to the watchdog structure
 * @return true if the watchdog is valid, false otherwise
 */
bool hw_watchdog_valid(hw_watchdog_t *watchdog);

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
 * must call hw_poll() periodically in the event loop to prevent system reset,
 * no less than the value returned by hw_watchdog_maxtimeout()
 */
void hw_watchdog_enable(hw_watchdog_t *watchdog, bool enable);

/**
 * @brief Cause a reset after a delay
 * @ingroup Watchdog
 * @param watchdog Pointer to the watchdog structure
 * @param delay_ms Delay in milliseconds before the reset occurs
 *
 * This function configures the watchdog to trigger a system reset after the
 * specified delay. The delay is clamped to the maximum supported timeout.
 * The reset can be cancelled by calling hw_watchdog_enable() with any
 * value.
 */
void hw_watchdog_reset(hw_watchdog_t *watchdog, uint32_t delay_ms);

#ifdef __cplusplus
}
#endif
