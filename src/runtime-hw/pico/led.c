#include <hardware/gpio.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#ifdef PICO_CYW43_SUPPORTED
#include <pico/cyw43_arch.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// TYPES

struct hw_led_ctx {
  sys_timer_t timer; ///< Timer for fade/blink operations
};

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static inline bool hw_led_status_is_wifi() {
#ifdef CYW43_WL_GPIO_LED_PIN
  return true;
#else
  return false;
#endif
}

static inline bool hw_led_status_is_ws2812() {
#ifdef PICO_DEFAULT_WS2812_PIN
  return true;
#else
  return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a LED unit.
 */
hw_led_t hw_led_init(uint8_t gpio, hw_pwm_t *pwm) {
  sys_assert(gpio < hw_gpio_count() || gpio == 0xFF);
  sys_assert(pwm == NULL || hw_pwm_valid(pwm));
  sys_assert(HW_LED_CTX_SIZE >= sizeof(struct hw_led_ctx));

  // If it's WS2812 we don't support it yet
  hw_led_t led = {0};
  if (gpio == 0xFF && hw_led_status_is_ws2812()) {
    sys_printf("WS2812 LED control not implemented yet\n");
    return led;
  }

  // If gpio is 0xFF, assume it's the on-board status LED
  if (gpio == 0xFF) {
    gpio = hw_led_status_gpio();
  }

  // Check GPIO pin validity against the PWM
  if (gpio != 0xFF && pwm) {
    uint8_t pwm_unit = hw_pwm_gpio_unit(gpio);
    if (pwm->unit != pwm_unit) {
      return led; // Return invalid LED
    }
  }

  // Set the GPIO to output mode if not PWM
  if (gpio != 0xFF && pwm == NULL) {
    sys_assert(gpio < hw_gpio_count());
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
  }

  // Initialize the struct
  led.gpio = gpio;
  led.pwm = pwm;

  // Set state to off - we initialize the PWM in here
  hw_led_set_state(&led, false);

  // Return valid LED structure
  return led;
}

/**
 * @brief Return the GPIO pin number for the on-board status LED.
 */
uint8_t hw_led_status_gpio() {
#ifdef PICO_DEFAULT_LED_PIN
  return PICO_DEFAULT_LED_PIN; // Return the default LED pin
#elif defined(PICO_DEFAULT_WS2812_PIN)
  return PICO_DEFAULT_WS2812_PIN; // Return the default WS2812 pin
#else
  return 0xFF; // Return 0xFF if the GPIO is controlled through
               // the Wi-Fi subsystem, or not available
#endif
}

/**
 * @brief Check if an LED is valid.
 */
bool hw_led_valid(hw_led_t *led) {
  if (led == NULL) {
    return false;
  }
  if (led->gpio == 0xFF) {
    // If it's WiFi, then OK
    return hw_led_status_is_wifi();
  }
  if (led->gpio >= hw_gpio_count()) {
    return false; // Invalid GPIO pin
  }
  if (led->pwm && !hw_pwm_valid(led->pwm)) {
    return false; // Invalid PWM structure
  }
  return true; // Valid LED structure
}

/**
 * @brief Finalize and release an LED.
 */
void hw_led_finalize(hw_led_t *led) {
  if (led == NULL || !hw_led_valid(led)) {
    return; // Nothing to finalize
  }

  // TODO: Finalize any timers

  // Set state to off
  hw_led_set_state(led, false);

  // If PWM is used, stop it
  if (led->pwm) {
    hw_pwm_stop(led->pwm);
  }

  // Deinitialize the GPIO pin
  if (led->gpio != 0xFF) {
    sys_assert(led->gpio < hw_gpio_count());
    gpio_deinit(led->gpio); // Deinitialize the GPIO pin
  }

  // Clear the LED structure
  sys_memset(led, 0, sizeof(hw_led_t));
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Set the LED state.
 */
bool hw_led_set_state_wifi(bool on) {
#ifdef PICO_CYW43_SUPPORTED
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on ? 1 : 0);
  return true;
#else
  (void)on;     // Suppress unused parameter warning
  return false; // Not supported if Wi-Fi is not available
#endif
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Set the LED state.
 */
bool hw_led_set_state(hw_led_t *led, bool on) {
  sys_assert(led && hw_led_valid(led));

  // TODO: Cancel any ongoing fade or blink operations

  // Binary control
  if (led->pwm == NULL) {
    if (led->gpio == 0xFF && hw_led_status_is_wifi()) {
      return hw_led_set_state_wifi(on);
    }
    sys_assert(led->gpio < hw_gpio_count());
    gpio_put(led->gpio, on ? 1 : 0);
    return true;
  }

  // Linear control using PWM
  if (led->pwm) {
    return hw_pwm_start(led->pwm, led->gpio,
                        on ? 100.0f : 0.0f); // Set to full brightness or off
  }

  // Don't know what to do here
  return false;
}

/**
 * @brief Set the LED brightness.
 */
bool hw_led_set_brightness(hw_led_t *led, uint8_t brightness) {
  sys_assert(led && hw_led_valid(led));

  // TODO: Cancel any ongoing fade or blink operations

  // Binary control
  if (led->pwm == NULL) {
    if (brightness == 0x00) {
      hw_led_set_state(led, false); // Set to off
      return true;
    } else if (brightness == 0xFF) {
      hw_led_set_state(led, true); // Set to full brightness
      return true;
    } else {
      return false; // Can't set intermediate brightness without PWM
    }
  }

  // Linear control using PWM
  if (led->pwm) {
    hw_pwm_start(led->pwm, led->gpio, (float)brightness * 100.0f / 255.0f);
    return true;
  }

  // Don't know what to do here
  return false;
}

/**
 * @brief Blink the LED continuously.
 */
bool hw_led_blink(hw_led_t *led, uint8_t period_ms, bool repeats) {
  // Blink functionality not yet implemented for Pico
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
  // Fade functionality not yet implemented for Pico
  sys_assert(led);
  (void)led;       // Suppress unused parameter warning
  (void)period_ms; // Suppress unused parameter warning
  (void)repeats;   // Suppress unused parameter warning
  return false;
}
