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
 * @brief Initialize a PWM slice from a GPIO pin.
 */
hw_pwm_t hw_pwm_init(uint8_t gpio_x, uint8_t gpio_y, hw_pwm_config_t *config) {
  sys_assert(gpio_x < hw_gpio_count());

  // Set up invalid PWM structure
  hw_pwm_t pwm = {0};
  pwm.slice = 0xFF;

  uint8_t slice = pwm_gpio_to_slice_num(gpio_x);
  if (gpio_y != 0 && gpio_y != gpio_x) {
    sys_assert(gpio_y < hw_gpio_count());
    if (pwm_gpio_to_slice_num(gpio_y) != slice) {
      return pwm; // Invalid GPIO pins for PWM
    }
  }

  // Set GPIO-X
  switch (pwm_gpio_to_channel(gpio_x)) {
  case PWM_CHAN_A:
    pwm.gpio_a = hw_gpio_init(gpio_x, HW_GPIO_PWM);
    if (!hw_gpio_valid(&pwm.gpio_a)) {
      return pwm; // Failed to initialize GPIO
    }
    break;
  case PWM_CHAN_B:
    pwm.gpio_b = hw_gpio_init(gpio_x, HW_GPIO_PWM);
    if (!hw_gpio_valid(&pwm.gpio_b)) {
      return pwm; // Failed to initialize GPIO
    }
    break;
  default:
    return pwm; // Invalid channel for PWM
  }

  // Set GPIO-Y if provided and different from gpio_x
  if (gpio_y != 0 && gpio_y != gpio_x) {
    uint8_t gpio_y_channel = pwm_gpio_to_channel(gpio_y);
    uint8_t gpio_x_channel = pwm_gpio_to_channel(gpio_x);

    // Prevent overwriting existing channel assignment
    if ((gpio_y_channel == PWM_CHAN_A && gpio_x_channel == PWM_CHAN_A) ||
        (gpio_y_channel == PWM_CHAN_B && gpio_x_channel == PWM_CHAN_B)) {
      // GPIO pins conflict - both map to same channel
      hw_gpio_finalize(&pwm.gpio_a);
      hw_gpio_finalize(&pwm.gpio_b);
      return pwm;
    }

    switch (gpio_y_channel) {
    case PWM_CHAN_A:
      pwm.gpio_a = hw_gpio_init(gpio_y, HW_GPIO_PWM);
      if (!hw_gpio_valid(&pwm.gpio_a)) {
        // Cleanup already initialized GPIO
        if (gpio_x_channel == PWM_CHAN_B) {
          hw_gpio_finalize(&pwm.gpio_b);
        }
        return pwm; // Failed to initialize GPIO
      }
      break;
    case PWM_CHAN_B:
      pwm.gpio_b = hw_gpio_init(gpio_y, HW_GPIO_PWM);
      if (!hw_gpio_valid(&pwm.gpio_b)) {
        // Cleanup already initialized GPIO
        if (gpio_x_channel == PWM_CHAN_A) {
          hw_gpio_finalize(&pwm.gpio_a);
        }
        return pwm; // Failed to initialize GPIO
      }
      break;
    default:
      // Cleanup already initialized GPIO
      if (gpio_x_channel == PWM_CHAN_A) {
        hw_gpio_finalize(&pwm.gpio_a);
      } else {
        hw_gpio_finalize(&pwm.gpio_b);
      }
      return pwm; // Invalid channel for PWM
    }
  }

  pwm_config pico_config = pwm_get_default_config();
  if (config) {
    // Apply custom configuration
    pwm_config_set_wrap(&pico_config, config->wrap);
    pwm_config_set_clkdiv(&pico_config, config->divider);
    pwm_config_set_clkdiv_mode(&pico_config, PWM_DIV_FREE_RUNNING);
    pwm_config_set_phase_correct(&pico_config, false);
    pwm_config_set_output_polarity(&pico_config, false, false);
    pwm.wrap = config->wrap;
  } else {
    // Use default wrap value
    pwm.wrap = 65535; // Default wrap value
  }

  // Now initialize the slice
  pwm_init(slice, &pico_config, false);
  pwm.slice = slice;
  pwm.enabled = false;

  // Return the initialized PWM structure
  return pwm;
}

/**
 * @brief Finalize and release a PWM slice.
 */
void hw_pwm_finalize(hw_pwm_t *pwm) {
  sys_assert(pwm && hw_pwm_valid(pwm));

  // Disable the PWM slice
  hw_pwm_stop(pwm);

  // TODO: Disable interrupts for this slice

  // Reset GPIO pins if assigned
  if (hw_gpio_valid(&pwm->gpio_a)) {
    hw_gpio_finalize(&pwm->gpio_a);
  }
  if (hw_gpio_valid(&pwm->gpio_b)) {
    hw_gpio_finalize(&pwm->gpio_b);
  }

  // Reset the PWM structure
  sys_memset(pwm, 0, sizeof(hw_pwm_t));
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Get PWM configuration for a frequency
 * @ingroup PWM
 * @return Default PWM configuration structure.
 */
hw_pwm_config_t hw_pwm_get_config(float freq) {
  sys_assert(freq > 0.0f && freq <= (float)clock_get_hz(clk_sys));

  // Get system clock frequency
  uint32_t sys_clk = clock_get_hz(clk_sys);
  sys_assert(sys_clk > 0);

  // Calculate target counts using floating point for better precision
  double target_counts_f = (double)sys_clk / (double)freq;
  uint32_t target_counts =
      (uint32_t)(target_counts_f + 0.5); // Round to nearest
  if (target_counts == 0) {
    target_counts = 1;
  }

  // Find best divider (1.0 to 256.0) and wrap (1 to 65536)
  float best_divider = 1.0f;
  uint32_t best_wrap = target_counts;

  // If target_counts > 65536, we need to use a divider
  if (target_counts > 65536) {
    best_divider =
        (float)target_counts / 65536.0f; // Calculate exact divider needed
    if (best_divider > 256.0f) {
      best_divider = 256.0f; // Clamp to max
    }
    best_wrap = (uint32_t)((double)target_counts / best_divider +
                           0.5); // Round to nearest
    if (best_wrap == 0)
      best_wrap = 1; // Ensure non-zero
    if (best_wrap > 65536)
      best_wrap = 65536; // Clamp to max
  }

  hw_pwm_config_t config = {.wrap = (best_wrap > 0) ? (best_wrap - 1)
                                                    : 0, // TOP value is wrap-1
                            .divider = best_divider};
  return config;
}

/**
 * @brief Apply configuration to a PWM slice.
 */
void hw_pwm_set_config(hw_pwm_t *pwm, const hw_pwm_config_t *config,
                       bool start) {
  sys_assert(pwm && hw_pwm_valid(pwm) && config);

  // Validate configuration parameters
  sys_assert(config->wrap <= 65535); // 16-bit wrap value limit
  sys_assert(config->divider >= 1.0f &&
             config->divider <= 256.0f); // Valid divider range

  // Stop PWM if it was running
  if (pwm->enabled) {
    hw_pwm_stop(pwm);
  }

  pwm_config pico_config = pwm_get_default_config();
  // Apply custom configuration
  pwm_config_set_wrap(&pico_config, config->wrap);
  pwm_config_set_clkdiv(&pico_config, config->divider);
  pwm_config_set_clkdiv_mode(&pico_config, PWM_DIV_FREE_RUNNING);
  pwm_config_set_phase_correct(&pico_config, false);
  pwm_config_set_output_polarity(&pico_config, false, false);
  pwm->wrap = config->wrap;

  // Apply the configuration
  pwm_init(pwm->slice, &pico_config, start);
  pwm->enabled = start;
}

///////////////////////////////////////////////////////////////////////////////
// CONTROL

/**
 * @brief Start PWM output.
 */
void hw_pwm_start(hw_pwm_t *pwm) {
  sys_assert(pwm && hw_pwm_valid(pwm));

  sys_printf("Starting PWM slice %d\n", pwm->slice);

  pwm_set_enabled(pwm->slice, true);
  pwm->enabled = true;
}

/**
 * @brief Stop PWM output.
 */
void hw_pwm_stop(hw_pwm_t *pwm) {
  sys_assert(pwm && hw_pwm_valid(pwm));
  pwm_set_enabled(pwm->slice, false);
  pwm->enabled = false;
}

/**
 * @brief Set PWM duty cycle
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @param ch Channel to configure (0 for A, 1 for B)
 * @param duty_percent Duty cycle as a percentage (0 to 100).
 */
void hw_pwm_set_duty(hw_pwm_t *pwm, uint8_t ch, uint8_t duty_percent) {
  sys_assert(pwm && hw_pwm_valid(pwm));
  sys_assert(duty_percent <= 100);
  sys_assert(ch == 0 || ch == 1);

  // Calculate the level based on the wrap value using 32-bit arithmetic to
  // avoid overflow
  uint32_t level_calc = ((uint32_t)(pwm->wrap + 1) * duty_percent) / 100;
  uint16_t level = (uint16_t)level_calc;

  if (ch == 0) {
    pwm_set_chan_level(pwm->slice, PWM_CHAN_A, level);
  } else {
    pwm_set_chan_level(pwm->slice, PWM_CHAN_B, level);
  }
}

/**
 * @brief Set the PWM interrupt callback handler.
 * @ingroup PWM
 * @param callback Pointer to the callback function to handle PWM interrupts,
 *                 or NULL to disable interrupt handling.
 * @param userdata User-defined data pointer to pass to the callback.
 *
 * This function registers a global callback handler that will be invoked
 * whenever a PWM wrap interrupt occurs on any slice that has been configured
 * for interrupt detection. Only one callback can be active at a time.
 *
 * @see hw_pwm_callback_t for callback function signature requirements.
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
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @param enabled True to enable interrupt, false to disable.
 *
 * This function enables or disables the wrap interrupt for the specified
 * PWM slice. The interrupt fires when the counter reaches the wrap value.
 * A callback must be set using hw_pwm_set_callback() to handle interrupts.
 */
void hw_pwm_set_irq_enabled(hw_pwm_t *pwm, bool enabled) {
  sys_assert(pwm && hw_pwm_valid(pwm));
  pwm_set_irq_enabled(pwm->slice, enabled);
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
