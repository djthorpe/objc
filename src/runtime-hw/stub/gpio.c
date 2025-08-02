#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Get the total number of available GPIO pins.
 */
uint8_t hw_gpio_count(void) {
  // GPIO not implemented in stub
  return 0;
}

/**
 * @brief Set the global GPIO interrupt callback handler.
 */
void hw_gpio_set_callback(hw_gpio_callback_t callback) {
  // GPIO not implemented in stub
  (void)callback; // Suppress unused parameter warning
}

/**
 * @brief Initialize a GPIO pin with the specified mode.
 */
hw_gpio_t hw_gpio_init(uint8_t pin, hw_gpio_mode_t mode) {
  // GPIO not implemented in stub
  (void)pin;
  (void)mode;
  hw_gpio_t gpio = {0};
  return gpio;
}

/**
 * @brief Finalize and release a GPIO pin.
 */
void hw_gpio_finalize(hw_gpio_t *gpio) {
  // GPIO not implemented in stub
  sys_assert(gpio);
  (void)gpio;
}

/**
 * @brief Get the current mode configuration of a GPIO pin.
 */
hw_gpio_mode_t hw_gpio_get_mode(hw_gpio_t *gpio) {
  // GPIO not implemented in stub
  sys_assert(gpio);
  return 0;
}

/**
 * @brief Set the current mode configuration of a GPIO pin.
 */
void hw_gpio_set_mode(hw_gpio_t *gpio, hw_gpio_mode_t mode) {
  // GPIO not implemented in stub
  sys_assert(gpio);
  (void)mode; // Suppress unused parameter warning
}

/**
 * @brief Read the current state of a GPIO pin.
 */
bool hw_gpio_get(hw_gpio_t *gpio) {
  // GPIO not implemented in stub
  sys_assert(gpio);
  return false;
}

/**
 * @brief Set the state of a GPIO pin.
 */
void hw_gpio_set(hw_gpio_t *gpio, bool value) {
  // GPIO not implemented in stub
  sys_assert(gpio);
  (void)value; // Suppress unused parameter warning
}