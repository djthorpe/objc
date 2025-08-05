#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static void _hw_pwm_callback_handler(void);

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

hw_pwm_callback_t _hw_pwm_callback_func = NULL; ///< Global PWM callback handler
void *_hw_pwm_userdata = NULL; ///< User data for the PWM callback

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a PWM unit.
 */
hw_pwm_t hw_pwm_init(uint8_t unit, hw_pwm_config_t *config) {
  sys_assert(unit < hw_pwm_count());

  // Set up invalid PWM structure
  hw_pwm_t pwm = {0};
  pwm.unit = 0xFF;

  pwm_config pico_config = pwm_get_default_config();
  if (config) {
    // Apply custom configuration
    pwm_config_set_wrap(&pico_config, config->wrap);
    pwm_config_set_clkdiv(&pico_config, config->divider);
    pwm_config_set_clkdiv_mode(&pico_config, PWM_DIV_FREE_RUNNING);
    pwm_config_set_phase_correct(&pico_config, false);
    pwm_config_set_output_polarity(&pico_config, false, false);
  }

  // Now initialize the unit - don't start it yet
  pwm_init(unit, &pico_config, false);
  pwm.unit = unit;
  pwm.wrap = config ? config->wrap : 0xFFFF;     // Default wrap value
  pwm.divider = config ? config->divider : 1.0f; // Default divider if not set
  pwm.enabled = false;

  return pwm;
}

/**
 * @brief Finalize and release a PWM unit.
 */
void hw_pwm_finalize(hw_pwm_t *pwm) {
  sys_assert(pwm);
  if (!hw_pwm_valid(pwm)) {
    return;
  }

  // Remove the callback if it was set
  if (_hw_pwm_callback_func) {
    hw_pwm_set_irq_enabled(pwm, false);
  }

  // Disable the PWM output on the GPIO pins
  uint8_t gpio;
  for (gpio = 0; gpio < hw_gpio_count(); gpio++) {
    if (hw_pwm_gpio_unit(gpio) == pwm->unit) {
      if (gpio_get_function(gpio) == GPIO_FUNC_PWM) {
        gpio_deinit(gpio);
      }
    }
  }

  // Disable the PWM unit
  hw_pwm_stop(pwm);

  // Mark as invalid
  sys_memset(pwm, 0, sizeof(hw_pwm_t));
  pwm->unit = 0xFF;
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

/**
 * @brief Get the number of PWM units.
 */
uint8_t hw_pwm_count() {
  return NUM_PWM_SLICES; // Number of PWM slices available
}

/**
 * @brief Get the PWM unit for a GPIO pin
 * @ingroup PWM
 * @param gpio The GPIO pin number.
 * @return The PWM unit number associated with the GPIO pin
 *
 * This function retrieves the PWM unit number associated with a GPIO pin.
 * The GPIO pin must be less than the total number of GPIO pins available.
 */
uint8_t hw_pwm_gpio_unit(uint8_t gpio) {
  sys_assert(gpio < hw_gpio_count());

  int unit = pwm_gpio_to_slice_num(gpio);
  sys_assert(unit >= 0 && unit < hw_pwm_count());
  return unit;
}

/**
 * @brief Get PWM configuration for a specific frequency.
 */
hw_pwm_config_t hw_pwm_get_config(float freq) {
  sys_assert(freq > 0.0f);

  // Get system clock frequency
  uint32_t sys_clk = clock_get_hz(clk_sys);
  sys_assert(sys_clk > 0);

  // If frequency is too high, clamp it to the maximum
  if (freq > (float)sys_clk / 2.0f) {
    freq = (float)sys_clk / 2.0f;
  }

  // Calculate target counts using floating point for better precision
  float target_counts_f = (float)sys_clk / freq;
  uint32_t target_counts =
      (uint32_t)(target_counts_f + 0.5); // Round to nearest
  if (target_counts == 0) {
    target_counts = 1;
  }

  // Find best divider (1.0 to 256.0) and wrap (0 to 65535)
  // Note: wrap value N means counter counts 0 to N (N+1 total counts)
  float best_divider = 1.0f;
  uint32_t best_wrap = target_counts - 1; // Convert from counts to wrap value

  // If target_counts > 65536, we need to use a divider
  if (target_counts > 65536) {
    best_divider =
        (float)target_counts / 65536.0f; // Calculate exact divider needed
    if (best_divider > 256.0f) {
      best_divider = 256.0f; // Clamp to max
    }
    uint32_t adjusted_counts =
        (uint32_t)((float)target_counts / best_divider + 0.5f);
    best_wrap =
        adjusted_counts > 0 ? adjusted_counts - 1 : 0; // Convert to wrap value
  }

  // Clamp wrap to maximum value (0-65535)
  if (best_wrap > 0xFFFF) {
    best_wrap = 0xFFFF;
  }

  // Return the best configuration
  hw_pwm_config_t config = {.wrap = best_wrap, .divider = best_divider};
  return config;
}

/**
 * @brief Returns the frequency configured for a PWM unit.
 */
float hw_pwm_get_freq(hw_pwm_t *pwm) {
  sys_assert(pwm && hw_pwm_valid(pwm));

  // Get the system clock frequency
  uint32_t sys_clk = clock_get_hz(clk_sys);
  sys_assert(sys_clk > 0);

  // Calculate frequency based on wrap and divider
  return (float)sys_clk / ((float)(pwm->wrap + 1) * pwm->divider);
}

/**
 * @brief Apply configuration to a PWM unit.
 */
void hw_pwm_set_config(hw_pwm_t *pwm, const hw_pwm_config_t *config) {
  sys_assert(pwm && hw_pwm_valid(pwm));
  sys_assert(config);

  // Validate configuration parameters
  sys_assert(config->wrap <= 0xFFFF); // 16-bit wrap value limit
  sys_assert(config->divider >= 1.0f &&
             config->divider <= 256.0f); // Valid divider range

  // Stop PWM if it was running
  if (pwm->enabled) {
    hw_pwm_stop(pwm);
  }

  // Apply custom configuration
  pwm_config pico_config = pwm_get_default_config();
  pwm_config_set_wrap(&pico_config, config->wrap);
  pwm_config_set_clkdiv(&pico_config, config->divider);
  pwm_config_set_clkdiv_mode(&pico_config, PWM_DIV_FREE_RUNNING);
  pwm_config_set_phase_correct(&pico_config, false);
  pwm_config_set_output_polarity(&pico_config, false, false);

  // Update the PWM structure
  pwm->divider = config->divider;
  pwm->wrap = config->wrap;

  // Apply the configuration, restarting the PWM if it was previously enabled
  pwm_init(pwm->unit, &pico_config, pwm->enabled);
}

///////////////////////////////////////////////////////////////////////////////
// CONTROL

/**
 * @brief Start PWM output on a specific GPIO pin.
 */
bool hw_pwm_start(hw_pwm_t *pwm, uint8_t gpio, float duty_percent) {
  sys_assert(pwm && hw_pwm_valid(pwm));
  sys_assert(gpio < hw_gpio_count());

  // Check GPIO is valid for this PWM unit
  if (pwm_gpio_to_slice_num(gpio) != pwm->unit) {
    return false;
  }

  // Calculate the level based on the wrap value using 32-bit arithmetic to
  // avoid overflow
  uint16_t level;
  if (duty_percent <= 0.0f) {
    level = 0; // Set to LOW
  } else if (duty_percent >= 100.0f) {
    level = pwm->wrap; // Set to HIGH
  } else {
    uint32_t level_calc = ((float)(pwm->wrap + 1) * duty_percent) / 100.0f;
    level = (uint16_t)level_calc;
    sys_assert(level <= pwm->wrap); // Ensure level is within bounds
  }

  // Set the level for the GPIO
  pwm_set_chan_level(pwm->unit, pwm_gpio_to_channel(gpio), level);

  // Set the GPIO function to PWM if not already set
  if (gpio_get_function(gpio) != GPIO_FUNC_PWM) {
    gpio_init(gpio);
    gpio_set_function(gpio, GPIO_FUNC_PWM);
  }

  // Enable the PWM output on the specified GPIO pin
  pwm_set_enabled(pwm->unit, true);
  pwm->enabled = true;

  // Return success
  return true;
}

/**
 * @brief Stop PWM output.
 */
void hw_pwm_stop(hw_pwm_t *pwm) {
  sys_assert(pwm && hw_pwm_valid(pwm));
  pwm_set_enabled(pwm->unit, false);
  pwm->enabled = false;
}

///////////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLING

/**
 * @brief Set the PWM interrupt callback handler.
 */
void hw_pwm_set_callback(hw_pwm_callback_t callback, void *userdata) {
  _hw_pwm_callback_func = callback;
  _hw_pwm_userdata = userdata;

  if (callback) {
    // Set up the interrupt handler
    irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), _hw_pwm_callback_handler);
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), true);
  } else {
    // Disable the interrupt
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), false);
  }
}

/**
 * @brief Enable or disable PWM wrap interrupt for a slice.
 */
void hw_pwm_set_irq_enabled(hw_pwm_t *pwm, bool enabled) {
  sys_assert(pwm && hw_pwm_valid(pwm));
  pwm_set_irq_enabled(pwm->unit, enabled);
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Internal PWM interrupt handler.
 */
static void _hw_pwm_callback_handler(void) {
  if (_hw_pwm_callback_func == NULL) {
    return;
  }
  uint32_t status = pwm_get_irq_status_mask();
  for (uint8_t slice = 0; slice < NUM_PWM_SLICES; slice++) {
    if (status & (1u << slice)) {
      pwm_clear_irq(slice);
      _hw_pwm_callback_func(slice, _hw_pwm_userdata);
    }
  }
}
