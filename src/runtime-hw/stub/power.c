#include <runtime-hw/power.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// Stub power singleton storage (opaque). We can't instantiate hw_power_t here
// because it's an incomplete type (opaque in the public header). Use a raw
// byte buffer to stand in for the handle; size chosen to match
// HW_POWER_CTX_SIZE so that taking the address yields a stable, unique pointer
// value.
static uint8_t _hw_power_stub_storage[HW_POWER_CTX_SIZE];
static bool _hw_power_stub_initialized = false;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize power management.
 */
hw_power_t *hw_power_init(uint8_t gpio_vsys, uint8_t gpio_vbus,
                          hw_power_callback_t callback, void *user_data) {
  // Power management not implemented in stub
  (void)gpio_vsys; // Suppress unused parameter warning
  (void)gpio_vbus; // Suppress unused parameter warning
  (void)callback;  // Suppress unused parameter warning
  (void)user_data; // Suppress unused parameter warning

  // Mark initialized and return opaque handle pointer
  _hw_power_stub_initialized = true;
  return (hw_power_t *)_hw_power_stub_storage; // Opaque, contents unused
}

/**
 * @brief Determine if the power handle is initialized and usable.
 */
bool hw_power_valid(hw_power_t *power) {
  (void)power;
  return _hw_power_stub_initialized;
}

/**
 * @brief Finalize and release any resources.
 */
void hw_power_finalize(hw_power_t *power) {
  (void)power;
  _hw_power_stub_initialized = false;
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Approximate battery state of charge.
 */
uint8_t hw_power_battery_percent(hw_power_t *power) {
  (void)power;
  return 100;
}

/**
 * @brief Current detected power source(s).
 */
hw_power_flag_t hw_power_source(hw_power_t *power) {
  (void)power;
  return HW_POWER_UNKNOWN;
}
