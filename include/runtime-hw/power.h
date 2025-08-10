/**
 * @file power.h
 * @brief Power management interface
 * @defgroup Power Power
 * @ingroup Hardware
 *
 * Power management detection of active power
 * source, and approximate battery state.
 *
 * The power management interface provides battery state of charge reporting
 * (0–100%) when supported, the ability to reset the device and optional
 * asynchronous callback notification on power changes.
 *
 * In order to be notified of power changes, the user must provide a callback
 * function when initializing the power management interface, and in the
 * main event loop, call hw_poll().
 */
#pragma once
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

#define HW_POWER_CTX_SIZE 64 ///< Size (bytes) of the  power context

// Opaque power management handle
typedef struct hw_power_t hw_power_t;

/**
 * @brief Power source capability / status flags.
 * @ingroup Power
 *
 * Bitmask describing which power sources are currently present / can be
 * detected. Implementations may return a combination (e.g. USB + BATTERY)
 * if simultaneous presence is detectable; otherwise a single dominant source
 * may be reported. When no information is available, HW_POWER_UNKNOWN is
 * returned.
 */
typedef enum {
  HW_POWER_UNKNOWN = 0,        ///< Unknown power source
  HW_POWER_USB = (1 << 0),     ///< USB (VBUS) power
  HW_POWER_BATTERY = (1 << 1), ///< Battery power
  HW_POWER_RESET = (1 << 2),   ///< Hardware will reset
} hw_power_flag_t;

/**
 * @brief Power status callback prototype.
 * @ingroup Power
 * @param power          Pointer to the associated power handle
 * @param flags          Bitmask of power source flags that changed since the
 *                       last callback. If HW_POWER_BATTERY flag is set,
 *                       the callback may be invoked on battery level changes.
 * @param battery_percent  Current battery percentage (0-100), valid if
 * HW_POWER_BATTERY is set and <battery_percent> is non-zero.
 */
typedef void (*hw_power_callback_t)(hw_power_t *power, hw_power_flag_t flags,
                                    uint8_t battery_percent);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize power management.
 * @ingroup Power
 * @param gpio_vsys  GPIO (or 0xFF for default) used for VSYS / battery sense
 * (platform specific)
 * @param gpio_vbus  GPIO (or 0xFF for default) used for VBUS / USB present
 * detect
 * @param callback   Optional callback for asynchronous updates (may be NULL)
 * @return Pointer to a hw_power_t handle, or NULL
 *
 * GPIO arguments are platform dependent; pass 0xFF to use default signals, or
 * if unavailable. The returned handle is valid if hw_power_valid() subsequently
 * reports true. Supplying a callback enables event driven notification when
 * supported.
 */
hw_power_t *hw_power_init(uint8_t gpio_vsys, uint8_t gpio_vbus,
                          hw_power_callback_t callback);

/**
 * @brief Determine if the power handle is initialized and usable.
 * @ingroup Power
 * @param power Power handle
 * @return True if initialized and not finalized; false otherwise.
 */
bool hw_power_valid(hw_power_t *power);

/**
 * @brief Finalize and release any resources.
 * @ingroup Power
 * @param power Power handle
 *
 * Safe to call on an already finalized or never‑initialized handle; in that
 * case it is a no‑op. After finalization, hw_power_valid() returns false.
 */
void hw_power_finalize(hw_power_t *power);

/**
 * @brief Approximate battery state of charge.
 * @ingroup Power
 * @param power Power handle
 * @return Integer percentage 0–100 inclusive.
 *
 * If unsupported or indeterminable, returns 100
 */
uint8_t hw_power_battery_percent(hw_power_t *power);

/**
 * @brief Current detected power source(s).
 * @ingroup Power
 * @param power Power handle
 * @return Bitmask of @ref hw_power_flag_t flags; HW_POWER_UNKNOWN if unknown.
 */
hw_power_flag_t hw_power_source(hw_power_t *power);

/**
 * @brief Reset the process or hardware.
 * @ingroup Power
 * @param power Power handle
 * @param delay_ms Delay in milliseconds before the reset
 * @return True if the reset operation was successful; false otherwise.
 *
 * If it's possible to reset the hardware, this function will attempt to do so
 * and return the result. If not, it will return false. The callback will be
 * called with the HW_POWER_RESET flag set, giving the application a chance
 * to react to the reset event.
 */
bool hw_power_reset(hw_power_t *power, uint32_t delay_ms);
