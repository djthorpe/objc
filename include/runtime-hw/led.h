/**
 * @file led.h
 * @brief LED control interface
 * @defgroup LED LED
 * @ingroup Hardware
 *
 * LED control interface for hardware platforms.
 */
#pragma once
#include "pwm.h"
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

#define HW_LED_CTX_SIZE 32 ///< Size of the LED context buffer

/**
 * @brief LED structure definition for hardware control.
 * @ingroup LED
 * @headerfile led.h runtime-hw/led.h
 *
 * This structure represents an LED unit, including its GPIO pin,
 * optional PWM control for brightness, and internal context buffer
 * for maintaining LED state.
 */
typedef struct {
  uint8_t gpio;  ///< GPIO pin number for the LED
  hw_pwm_t *pwm; ///< Optional pointer to a PWM structure for linear control
  uint8_t ctx[HW_LED_CTX_SIZE]; ///< Internal context buffer for LED state
} hw_led_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a LED unit.
 * @ingroup LED
 * @param gpio The GPIO pin number for the LED.
 * @param pwm Optional pointer to a PWM structure for linear control.
 * @return A LED structure representing the initialized unit.
 *
 * This function initializes an LED unit. If linear control is
 * supported, the PWM structure is valid and associated with the GPIO pin,
 * it will set up the PWM channel for the LED.
 *
 * If the pin number is 0xFF then it assumes the on-board status LED
 * is being initialized. If the PWM structure is NULL, the LED will be
 * controlled in a simple on/off manner without PWM.
 */
hw_led_t hw_led_init(uint8_t gpio, hw_pwm_t *pwm);

/**
 * @brief Return the GPIO pin number for the on-board status LED.
 * @ingroup LED
 * @return The GPIO pin number used for the status LED, or 0xFF if not
 * available: for example, if the GPIO is controlled through the Wi-Fi
 * subsystem.
 */
uint8_t hw_led_status_gpio();

/**
 * @brief Finalize and release an LED.
 * @ingroup LED
 * @param led Pointer to the LED structure to finalize.
 *
 * This function disables the LED unit. If a PWM structure was used,
 * it will not stop the PWM output, but it will release the GPIO pin
 * associated with the LED.
 */
void hw_led_finalize(hw_led_t *led);

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Check if an LED is valid.
 * @ingroup LED
 * @param led Pointer to the LED structure to check.
 * @return True if the LED is valid, false otherwise.
 */
bool hw_led_valid(hw_led_t *led);

/**
 * @brief Set the LED state.
 * @ingroup LED
 * @param led Pointer to the LED structure.
 * @param on True to turn on the LED to full brightness, false to turn it off.
 * @return True if the LED was successfully set to the desired state, false
 * otherwise.
 *
 * This function sets the state of the LED. If linear control is supported,
 * it will use the PWM channel to set the brightness level. It will cancel
 * any ongoing fade or blink operations.
 */
bool hw_led_set_state(hw_led_t *led, bool on);

/**
 * @brief Set the LED brightness.
 * @ingroup LED
 * @param led Pointer to the LED structure.
 * @param brightness The brightness level to set (0-255).
 *
 * This function sets the brightness of the LED. If linear control is supported,
 * it will use the PWM channel to set the brightness level. It will cancel
 * any ongoing fade or blink operations.
 */
bool hw_led_set_brightness(hw_led_t *led, uint8_t brightness);

/**
 * @brief Blink the LED continuously.
 * @ingroup LED
 * @param led Pointer to the LED structure.
 * @param period_ms The period of the blink in milliseconds.
 * @param repeats True to repeat the blink indefinitely, false to blink once.
 * @return True if the LED was successfully set to blink, false otherwise.
 *
 * This function blinks the LED continuously with the specified period.
 * The LED will turn on and off at the specified interval.
 */
bool hw_led_blink(hw_led_t *led, uint8_t period_ms, bool repeats);

/**
 * @brief Fade the LED continuously.
 * @ingroup LED
 * @param led Pointer to the LED structure.
 * @param period_ms The period of the fade in milliseconds.
 * @param repeats True to repeat the fade indefinitely, false to fade once.
 * @return True if the LED was successfully set to fade, false otherwise.
 *
 * This function fades the LED continuously with the specified period.
 * The LED will gradually change its brightness over the specified interval,
 * if it supports linear control through PWM. Otherwise, it will blink
 * instead.
 */
bool hw_led_fade(hw_led_t *led, uint8_t period_ms, bool repeats);
