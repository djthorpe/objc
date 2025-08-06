#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a LED unit.
 */
hw_led_t hw_led_init(uint8_t gpio, hw_pwm_t *pwm) {
  // LED not implemented in stub
  (void)gpio; // Suppress unused parameter warning
  (void)pwm;  // Suppress unused parameter warning
  hw_led_t led = {0};
  return led;
}

/**
 * @brief Return the GPIO pin number for the on-board status LED.
 */
uint8_t hw_led_status_gpio() {
  // LED not implemented in stub
  return 0xFF; // Return invalid pin
}

/**
 * @brief Finalize and release an LED.
 */
void hw_led_finalize(hw_led_t *led) {
  // LED not implemented in stub
  sys_assert(led);
  (void)led; // Suppress unused parameter warning
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Check if an LED is valid.
 */
bool hw_led_valid(hw_led_t *led) {
  // LED not implemented in stub
  sys_assert(led);
  (void)led; // Suppress unused parameter warning
  return false;
}

/**
 * @brief Set the LED state.
 */
bool hw_led_set_state(hw_led_t *led, bool on) {
  // LED not implemented in stub
  sys_assert(led);
  (void)led; // Suppress unused parameter warning
  (void)on;  // Suppress unused parameter warning
  return false;
}

/**
 * @brief Set the LED brightness.
 */
bool hw_led_set_brightness(hw_led_t *led, uint8_t brightness) {
  // LED not implemented in stub
  sys_assert(led);
  (void)led;        // Suppress unused parameter warning
  (void)brightness; // Suppress unused parameter warning
  return false;
}

/**
 * @brief Blink the LED continuously.
 */
bool hw_led_blink(hw_led_t *led, uint8_t period_ms, bool repeats) {
  // LED not implemented in stub
  sys_assert(led);
  (void)led;       // Suppress unused parameter warning
  (void)period_ms; // Suppress unused parameter warning
  (void)repeats;   // Suppress unused parameter warning
  return false;
}

/**
 * @brief Fade the LED continuously.
 */
bool hw_led_fade(hw_led_t *led, uint8_t period_ms, bool repeats) {
  // LED not implemented in stub
  sys_assert(led);
  (void)led;       // Suppress unused parameter warning
  (void)period_ms; // Suppress unused parameter warning
  (void)repeats;   // Suppress unused parameter warning
  return false;
}
