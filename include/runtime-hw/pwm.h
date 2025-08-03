/**
 * @file pwm.h
 * @brief Pulse Width Modulation (PWM) interface
 * @defgroup PWM PWM
 * @ingroup Hardware
 *
 * Pulse Width Modulation (PWM) interface for hardware platforms.
 * This module provides functions to initialize PWM channels, configure
 * frequency and duty cycle, and control PWM output.
 *
 * The PWM implementation supports multiple slices, each with two channels (A
 * and B). On the Raspberry Pi Pico, there are 8 PWM slices (16 channels total)
 * on RP2040 and 12 PWM slices (24 channels total) on RP2350.
 *
 * @example pico/pwm_demo/main.c
 * An example of using PWM to control LED brightness and servo motors.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief PWM configuration structure for setup parameters.
 * @ingroup PWM
 */
typedef struct {
  uint32_t wrap; ///< Counter wrap value (TOP), determines PWM period
  float divider; ///< Clock divider
} hw_pwm_config_t;

/**
 * @brief PWM slice structure representing a PWM instance.
 * @ingroup PWM
 */
typedef struct hw_pwm_t {
  uint8_t slice;    ///< PWM slice number
  hw_gpio_t gpio_a; ///< GPIO Channel A
  hw_gpio_t gpio_b; ///< GPIO Channel B
  uint32_t wrap;    ///< Current wrap value for duty cycle calculations
  bool enabled;     ///< PWM slice enabled state
} hw_pwm_t;

/**
 * @brief PWM interrupt callback function pointer.
 * @ingroup PWM
 * @param slice The PWM slice number that triggered the interrupt.
 * @param userdata User-defined data pointer passed when setting the callback.
 *
 * This callback function is called when a PWM wrap interrupt occurs on a slice
 * that has been configured for interrupt detection. The callback is executed
 * in interrupt context, so it should be kept short and avoid blocking
 * operations.
 *
 * @see hw_pwm_set_callback() for setting up the callback handler.
 */
typedef void (*hw_pwm_callback_t)(uint8_t slice, void *userdata);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize PWM for one or two GPIO pins.
 * @ingroup PWM
 * @param gpio_x The first GPIO pin number to configure for PWM output.
 * @param gpio_y The second GPIO pin number to configure for PWM output. If
 * zero, only the first pin will be used, which could be channel A or B
 * @param config Optional pointer to a configuration structure. If NULL, default
 * configuration will be used.
 * @return A PWM structure representing the initialized slice.
 *
 * This function determines which PWM slice and channel(s) the GPIO pin belongs
 * to, initializes the slice with default settings, and configures the GPIO pin
 * for PWM output. The PWM slice is not started automatically.
 */
hw_pwm_t hw_pwm_init(uint8_t gpio_x, uint8_t gpio_y, hw_pwm_config_t *config);

/**
 * @brief Validate the PWM slice.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @return True if the PWM slice is valid, false otherwise.
 *
 * The result of hw_pwm_init can return an empty PWM structure if the
 * initialization fails. This function checks if the PWM slice is valid.
 */
static inline bool hw_pwm_valid(hw_pwm_t *pwm) {
  return pwm && pwm->slice != 0xFF;
}

/**
 * @brief Finalize and release a PWM slice.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure to finalize.
 *
 * This function disables the PWM slice, resets its configuration,
 * and releases any associated GPIO pins. After calling this function,
 * the PWM slice should not be used until re-initialized.
 */
void hw_pwm_finalize(hw_pwm_t *pwm);

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

/**
 * @brief Get PWM configuration.
 * @ingroup PWM
 * @param freq Desired frequency in Hz.
 * @return Default PWM configuration structure.
 */
hw_pwm_config_t hw_pwm_get_config(float freq);

/**
 * @brief Apply configuration to a PWM slice.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @param config Pointer to the configuration to apply.
 * @param start If true, the PWM will be started after configuration.
 *
 * This function applies the specified configuration to the PWM slice.
 * The PWM slice will be stopped before configuration and optionally
 * restarted if the start parameter is true.
 */
void hw_pwm_set_config(hw_pwm_t *pwm, const hw_pwm_config_t *config,
                       bool start);

///////////////////////////////////////////////////////////////////////////////
// CONTROL

/**
 * @brief Start PWM output.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 *
 * This function enables the PWM slice to start generating output.
 * The PWM will begin counting from its current counter value.
 */
void hw_pwm_start(hw_pwm_t *pwm);

/**
 * @brief Stop PWM output.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 *
 * This function disables the PWM slice, stopping output generation.
 * The counter value is preserved and will resume from this point
 * when the PWM is restarted.
 */
void hw_pwm_stop(hw_pwm_t *pwm);

/**
 * @brief Set PWM duty cycle
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @param ch Channel to configure (0 for A, 1 for B)
 * @param duty_percent Duty cycle as a percentage (0 to 100).
 */
void hw_pwm_set_duty(hw_pwm_t *pwm, uint8_t ch, uint8_t duty_percent);

///////////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLING

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
void hw_pwm_set_callback(hw_pwm_callback_t callback, void *userdata);

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
void hw_pwm_set_irq_enabled(hw_pwm_t *pwm, bool enabled);
