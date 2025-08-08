/**
 * @file power.h
 * @brief Power management interface
 * @defgroup Power Power
 * @ingroup Hardware
 *
 * Minimal cross‑platform power management abstraction providing:
 *  - Detection of active power source(s) (USB / battery)
 *  - Approximate battery state of charge reporting (0–100%) when supported
 *  - Optional asynchronous callback notification on source / status changes
 *
 * Design goals:
 *  - Header‑only public contract with opaque internal state (ctx buffer)
 *  - Tiny footprint: no dynamic allocation; caller owns the struct
 *  - Graceful degradation: platforms lacking measurement return defaults
 *
 * Unless otherwise stated, functions are NOT thread‑safe; external
 * serialization is required if accessed concurrently from multiple contexts.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

#define HW_POWER_CTX_SIZE 64 ///< Size (bytes) of the  power context

/**
 * @brief Handle for power management state.
 * @ingroup Power
 * @headerfile power.h runtime-hw/hw.h
 */
typedef struct {
  uint8_t ctx[HW_POWER_CTX_SIZE]; ///< Internal context buffer for power state
} hw_power_t;

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
} hw_power_cap_t;

/**
 * @brief Power status callback prototype.
 * @ingroup Power
 * @param power          Pointer to the associated power handle
 * @param source_change  True if the power source bitmask changed since the
 *                       last callback. If battery power source, the callback
 *                       may be invoked on battery level changes.
 */
typedef void (*hw_power_callback_t)(hw_power_t *power, bool source_change);

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
 * @return Initialized @ref hw_power_t handle (by value).
 *
 * GPIO arguments are platform dependent; pass 0xFF to use default signals, or
 * if unavailable. The returned handle is valid if hw_power_valid() subsequently
 * reports true. Supplying a callback enables event driven notification when
 * supported.
 */
hw_power_t hw_power_init(uint8_t gpio_vsys, uint8_t gpio_vbus,
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
 * @return Bitmask of @ref hw_power_cap_t flags; HW_POWER_UNKNOWN if unknown.
 */
hw_power_cap_t hw_power_source(hw_power_t *power);

/**
 * @brief Reset the process or hardware.
 * @ingroup Power
 * @param power Power handle
 * @return True if the reset operation was successful; false otherwise.
 *
 * If it's possible to reset the hardware, this function will attempt to do so
 * and return the result. If not, it will return false.
 */
bool hw_power_reset(hw_power_t *power);
