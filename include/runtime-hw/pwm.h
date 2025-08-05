/**
 * @file pwm.h
 * @brief Pulse Width Modulation (PWM) interface
 * @defgroup PWM PWM
 * @ingroup Hardware
 *
 * Pulse Width Modulation (PWM) interface for hardware platforms.
 * This module provides functions to initialize PWM units,
 * configure frequency and duty cycle, and control PWM output.
 *
 * The PWM implementation supports multiple units (slices), each with one or
 * more GPIO outputs. On the Raspberry Pi Pico, there are 8 PWM units on
 * the RP2040 and 12 PWM units on the RP2350.
 *
 * Interrupts can be configured to trigger on PWM wrap events, allowing
 * for precise timing and control in applications such as motor control,
 * LED dimming, and audio generation.
 *
 * There is one global PWM callback, which is called when a PWM wrap
 * interrupt occurs on any unit. The callback is executed in interrupt context,
 * so it should be kept short and avoid blocking operations.
 *
 * The frequency of the PWM output is platform-dependent, but on the Pico
 * this can range from approximately 8 Hz to 60 MHz. The value that is set
 * in the PWM configuration is not necessarily the exact frequency,
 * but rather the closest achievable frequency based on the hardware
 * capabilities.
 *
 * Similarly the duty cycle (the ratio of HIGH vs LOW for each PWM period)
 * is set as a percentage (0-100) of the PWM period.
 * The actual duty cycle may not be exact due to hardware limitations,
 * but it will be as close as possible to the requested value.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief PWM configuration structure for setup parameters.
 * @ingroup PWM
 * @headerfile pwm.h runtime-hw/hw.h
 */
typedef struct {
  uint32_t wrap; ///< Counter wrap value (TOP), determines PWM period
  float divider; ///< Clock divider
} hw_pwm_config_t;

/**
 * @brief PWM structure representing a PWM instance.
 * @ingroup PWM
 * @headerfile pwm.h runtime-hw/hw.h
 */
typedef struct hw_pwm_t {
  uint8_t unit;  ///< PWM unit number
  uint32_t wrap; ///< Current wrap value for duty cycle calculations
  float divider; ///< Clock divider
  bool enabled;  ///< PWM unit enabled state
} hw_pwm_t;

/**
 * @brief PWM interrupt callback function pointer.
 * @ingroup PWM
 * @param unit The PWM unit that triggered the interrupt.
 * @param userdata User-defined data pointer passed when setting the callback.
 *
 * This callback function is called when a PWM wrap interrupt occurs on a unit
 * that has been configured for interrupt detection. The callback is executed
 * in interrupt context, so it should be kept short and avoid blocking
 * operations.
 *
 * @see hw_pwm_set_callback() for setting up the callback handler.
 */
typedef void (*hw_pwm_callback_t)(uint8_t unit, void *userdata);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a PWM unit.
 * @ingroup PWM
 * @param unit The PWM unit number to initialize.
 * @param config Optional pointer to a configuration structure. If NULL, default
 * configuration will be used.
 * @return A PWM structure representing the initialized unit.
 *
 * This function initializes the PWM unit and channel(s) the GPIO pin belongs
 * to and initializes the unit with default settings. The PWM unit is not
 * started and is set to the disabled state.
 */
hw_pwm_t hw_pwm_init(uint8_t unit, hw_pwm_config_t *config);

/**
 * @brief Get the number of PWM units.
 * @ingroup PWM
 * @return Number of PWM units available.
 *
 * This function returns the total number of PWM units that can be used. It
 * will return zero if PWM is not supported on the platform.
 */
uint8_t hw_pwm_count();

/**
 * @brief Validate the PWM unit.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @return True if the PWM unit is valid, false otherwise.
 *
 * The result of hw_pwm_init can return an empty PWM structure if the
 * initialization fails. This function checks if the PWM unit is valid.
 */
static inline bool hw_pwm_valid(hw_pwm_t *pwm) {
  if (pwm == NULL) {
    return false;
  }
  if (pwm->unit == 0xFF) {
    return false;
  }
  if (pwm->unit >= hw_pwm_count()) {
    return false;
  }
  return true;
}

/**
 * @brief Finalize and release a PWM unit.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure to finalize.
 *
 * This function disables the PWM unit, resets its configuration,
 * and releases any associated GPIO pins. After calling this function,
 * the PWM unit should not be used until re-initialized.
 */
void hw_pwm_finalize(hw_pwm_t *pwm);

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

/**
 * @brief Get the PWM unit for a GPIO pin
 * @ingroup PWM
 * @param gpio The GPIO pin number.
 * @return The PWM unit number associated with the GPIO pin
 *
 * This function retrieves the PWM unit number associated with a GPIO pin.
 * The GPIO pin must be less than the total number of GPIO pins available.
 */
uint8_t hw_pwm_gpio_unit(uint8_t gpio);

/**
 * @brief Get PWM configuration.
 * @ingroup PWM
 * @param freq Desired frequency in Hz.
 * @return Default PWM configuration structure, with the frequency set to the
 * specified value.
 */
hw_pwm_config_t hw_pwm_get_config(float freq);

/**
 * @brief Returns the frequency configured for a PWM unit.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @return The frequency configured for the PWM unit, or 0 if not valid.
 *
 * This function retrieves the frequency value from the PWM configuration.
 */
float hw_pwm_get_freq(hw_pwm_t *pwm);

/**
 * @brief Apply configuration to a PWM unit.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @param config Pointer to the configuration to apply.
 *
 * This function applies the specified configuration to the PWM unit.
 * If the PWM unit is currently enabled, it will be stopped before applying
 * the new configuration, and then restarted.
 */
void hw_pwm_set_config(hw_pwm_t *pwm, const hw_pwm_config_t *config);

///////////////////////////////////////////////////////////////////////////////
// CONTROL

/**
 * @brief Start PWM output on a specific GPIO pin.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 * @param gpio GPIO pin number to attach.
 * @param duty_percent Duty cycle as a percentage, between 0 and 100.
 * @return True if the PWM was started successfully, false if the GPIO pin is
 * invalid.
 *
 * This function attaches the specified GPIO pin to the PWM channel,
 * starts the PWM output, sets the duty cycle for the specified PWM channel.
 *
 * If the duty_percent is less than or equal to 0, the PWM output will always
 * be low. If the duty_percent is 100 or greater, the PWM output will always be
 * high. If the duty_percent is between 0 and 100, the PWM output will toggle
 * between high and low at the specified frequency, with the duty cycle
 * determining the proportion of time the output is high.
 */
bool hw_pwm_start(hw_pwm_t *pwm, uint8_t gpio, float duty_percent);

/**
 * @brief Stop PWM output.
 * @ingroup PWM
 * @param pwm Pointer to the PWM structure.
 *
 * This function disables the PWM unit, stopping output generation.
 * The counter value is preserved and will resume from this point
 * when the PWM is restarted.
 *
 * The IRQ state is not affected by this function, so if the PWM was
 * configured to generate interrupts, the interrupt handler will still be
 * called when the PWM is restarted.
 */
void hw_pwm_stop(hw_pwm_t *pwm);

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
