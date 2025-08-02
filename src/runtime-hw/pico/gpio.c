#include <hardware/adc.h>
#include <hardware/gpio.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static void _hw_gpio_callback(uint pin, uint32_t events);

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

hw_gpio_callback_t _hw_gpio_callback_func =
    NULL;                       ///< Global GPIO callback handler
void *_hw_gpio_userdata = NULL; ///< User data for the GPIO callback

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a GPIO pin with the specified mode.
 */
hw_gpio_t hw_gpio_init(uint8_t pin, hw_gpio_mode_t mode) {
  sys_assert(pin < NUM_BANK0_GPIOS && pin < 64);
  gpio_init(pin);

  // Initialize the GPIO pin with the specified mode
  hw_gpio_t gpio = {0};
  gpio.mask = (1ULL << pin);
  gpio.pin = pin;

  // Set the GPIO mode
  if (mode > 0) {
    hw_gpio_set_mode(&gpio, mode);
  }

  // Return the initialized GPIO structure
  return gpio;
}

/**
 * @brief Finalize and release a GPIO pin.
 */
void hw_gpio_finalize(hw_gpio_t *gpio) {
  sys_assert(gpio && gpio->mask != 0);
  gpio_deinit(gpio->pin);
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Get the total number of available GPIO pins.
 */
uint8_t hw_gpio_count(void) { return NUM_BANK0_GPIOS; }

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Set the current mode configuration of a GPIO pin.
 */
void hw_gpio_set_mode(hw_gpio_t *gpio, hw_gpio_mode_t mode) {
  sys_assert(gpio && gpio->mask != 0);

  // Set GPIO pin to SIO
  gpio_init(gpio->pin);

  // Cancel the interrupt
  if (mode != HW_GPIO_INPUT && mode != HW_GPIO_PULLUP &&
      mode != HW_GPIO_PULLDOWN) {
    gpio_set_irq_enabled_with_callback(gpio->pin, 0xFF, false, NULL);
  }

  // Set the GPIO mode
  switch (mode) {
  case HW_GPIO_NONE:
    return; // No function set, do nothing
  case HW_GPIO_INPUT:
    gpio_set_dir(gpio->pin, GPIO_IN);
    gpio_set_pulls(gpio->pin, false, false);
    gpio_set_irq_enabled_with_callback(gpio->pin,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true, &_hw_gpio_callback);
    break;
  case HW_GPIO_PULLUP:
    gpio_set_pulls(gpio->pin, true, false);
    gpio_set_irq_enabled_with_callback(gpio->pin,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true, &_hw_gpio_callback);
    break;
  case HW_GPIO_PULLDOWN:
    gpio_set_dir(gpio->pin, GPIO_IN);
    gpio_set_pulls(gpio->pin, false, true);
    gpio_set_irq_enabled_with_callback(gpio->pin,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true, &_hw_gpio_callback);
    break;
  case HW_GPIO_OUTPUT:
    gpio_set_dir(gpio->pin, GPIO_OUT);
    break;
  case HW_GPIO_SPI:
    gpio_set_function(gpio->pin, GPIO_FUNC_SPI);
    break;
  case HW_GPIO_I2C:
    gpio_set_function(gpio->pin, GPIO_FUNC_I2C);
    break;
  case HW_GPIO_UART:
    gpio_set_function(gpio->pin, GPIO_FUNC_UART);
    break;
  case HW_GPIO_PWM:
    gpio_set_function(gpio->pin, GPIO_FUNC_PWM);
    break;
  case HW_GPIO_ADC:
    adc_gpio_init(gpio->pin);
    break;
  default:
    sys_panicf("Invalid GPIO mode");
    break;
  }
}

/**
 * @brief Get the current mode configuration of a GPIO pin.
 */
hw_gpio_mode_t hw_gpio_get_mode(hw_gpio_t *gpio) {
  sys_assert(gpio && gpio->mask != 0);

  gpio_function_t f = gpio_get_function(gpio->pin);
  switch (f) {
  case GPIO_FUNC_NULL:
    return HW_GPIO_ADC; // Default to ADC if no function set
  case GPIO_FUNC_SPI:
    return HW_GPIO_SPI;
  case GPIO_FUNC_I2C:
    return HW_GPIO_I2C;
  case GPIO_FUNC_UART:
    return HW_GPIO_UART;
  case GPIO_FUNC_PWM:
    return HW_GPIO_PWM;
  case GPIO_FUNC_SIO:
    if (gpio_get_dir(gpio->pin) == GPIO_IN) {
      if (gpio_is_pulled_up(gpio->pin)) {
        return HW_GPIO_PULLUP;
      } else if (gpio_is_pulled_down(gpio->pin)) {
        return HW_GPIO_PULLDOWN;
      } else {
        return HW_GPIO_INPUT;
      }
    } else {
      return HW_GPIO_OUTPUT;
    }
  default:
    // Unsupported or unknown function
    return HW_GPIO_UNKNOWN;
  }
}

/**
 * @brief Read the current state of a pin.
 */
bool hw_gpio_get(hw_gpio_t *gpio) {
  sys_assert(gpio && gpio->mask != 0);
  return gpio_get(gpio->pin);
}

/**
 * @brief Set the state of a pin.
 */
void hw_gpio_set(hw_gpio_t *gpio, bool value) {
  sys_assert(gpio && gpio->mask != 0);
  gpio_put(gpio->pin, value);
}

/**
 * @brief Set the global GPIO interrupt callback handler.
 */
void hw_gpio_set_callback(hw_gpio_callback_t callback, void *userdata) {
  _hw_gpio_callback_func = callback;
  _hw_gpio_userdata = userdata;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

static void _hw_gpio_callback(uint pin, uint32_t events) {
  hw_gpio_event_t event = 0;
  if (_hw_gpio_callback_func == NULL) {
    return; // No callback set, do nothing
  }
  if (events & GPIO_IRQ_EDGE_RISE) {
    event |= HW_GPIO_RISING;
  }
  if (events & GPIO_IRQ_EDGE_FALL) {
    event |= HW_GPIO_FALLING;
  }
  _hw_gpio_callback_func(pin, event, _hw_gpio_userdata);
}
