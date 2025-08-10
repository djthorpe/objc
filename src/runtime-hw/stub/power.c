#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// Stub power singleton
static hw_power_t _hw_power_stub = {0};

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize power management.
 */
hw_power_t *hw_power_init(uint8_t gpio_vsys, uint8_t gpio_vbus,
                          hw_power_callback_t callback) {
  // Power management not implemented in stub
  (void)gpio_vsys; // Suppress unused parameter warning
  (void)gpio_vbus; // Suppress unused parameter warning
  (void)callback;  // Suppress unused parameter warning

  // Reset stub power handle
  _hw_power_stub = (hw_power_t){0};
  return &_hw_power_stub;
}

/**
 * @brief Determine if the power handle is initialized and usable.
 */
bool hw_power_valid(hw_power_t *power) {
  sys_assert(power);
  // Power management not implemented in stub
  return false; // Always return false since power management is not implemented
}

/**
 * @brief Finalize and release any resources.
 */
void hw_power_finalize(hw_power_t *power) {
  sys_assert(power);
  // Power management not implemented in stub
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Approximate battery state of charge.
 */
uint8_t hw_power_battery_percent(hw_power_t *power) {
  sys_assert(power);
  // Power management not implemented in stub
  return 100; // Return 100% as specified in the header for unsupported cases
}

/**
 * @brief Current detected power source(s).
 */
hw_power_flag_t hw_power_source(hw_power_t *power) {
  sys_assert(power);
  // Power management not implemented in stub
  return HW_POWER_UNKNOWN; // Return unknown as specified in the header
}

/**
 * @brief Reset the process or hardware.
 */
bool hw_power_reset(hw_power_t *power, uint32_t delay_ms) {
  sys_assert(power);
  (void)delay_ms; // Suppress unused parameter warning

  // Power management not implemented in stub
  return false; // Return false since reset is not implemented
}
