/**
 * @file led.h
 * @brief LED control interface
 * @defgroup LED LED
 * @ingroup Hardware
 *
 * LED control interface for hardware platforms.
 *
 * @example examples/runtime/led/main.c
 */
#pragma once
#include "pwm.h"
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

#define HW_LED_CTX_SIZE 64 ///< Size of the LED context buffer

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

/**
 * @brief LED capabilities flags.
 * @ingroup LED
 *
 * This enum defines the capabilities of an LED unit, such as whether it
 * supports binary on/off control or linear brightness control via PWM.
 */
typedef enum {
  HW_LED_CAP_NONE = 0,          ///< No capabilities
  HW_LED_CAP_BINARY = (1 << 0), ///< Supports binary on/off control
  HW_LED_CAP_LINEAR = (1 << 1), ///< Supports linear brightness control via PWM
  HW_LED_CAP_GPIO = (1 << 2),   ///< GPIO pin control
} hw_led_cap_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize an LED unit.
 * @ingroup LED
 * @param gpio The GPIO pin number for the LED.
 * @param pwm Optional pointer to a PWM structure for linear control.
 * @return A LED structure representing the initialized unit.
 *
 * This function initializes an LED unit. If linear control is
 * supported (non-NULL PWM pointer associated with the GPIO pin),
 * it will set up the PWM channel for the LED.
 *
 * If the pin number is 0xFF then it assumes the platform on-board status LED
 * is being initialized (which may be implemented via a Wi‑Fi / radio module
 * rather than a raw GPIO). If the PWM structure is NULL, the LED will be
 * controlled in a simple on/off manner (binary) without PWM.
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
 * it will stop the PWM output and release the GPIO pin
 * associated with the LED.
 */
void hw_led_finalize(hw_led_t *led);

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Check if an LED handle is valid and initialized.
 * @ingroup LED
 * @param led Pointer to the LED structure to check.
 * @return True if the LED has been successfully initialized and not yet
 *         finalized; false otherwise.
 */
bool hw_led_valid(hw_led_t *led);

/**
 * @brief Return LED capabilities.
 * @ingroup LED
 * @param led Pointer to the LED structure to check.
 * @return Bitmask of @ref hw_led_cap_t values describing supported operations.
 *
 * Notes:
 *  - HW_LED_CAP_BINARY implies on/off control (present for every valid LED).
 *  - HW_LED_CAP_LINEAR implies brightness (0..255) via PWM.
 *  - HW_LED_CAP_GPIO indicates a directly drivable GPIO (as distinct from a
 *    virtual / module-managed status LED).
 */
hw_led_cap_t hw_led_capabilities(hw_led_t *led);

/**
 * @brief Set the LED binary state (full on or off).
 * @ingroup LED
 * @param led Pointer to the LED structure.
 * @param on True to turn on the LED to full brightness, false to turn it off.
 * @return True on success, false on invalid handle or hardware failure.
 *
 * Cancels any ongoing blink or fade before applying the new state. For a
 * linear (PWM) LED, "on" maps to brightness 255 and "off" maps to 0.
 */
bool hw_led_set_state(hw_led_t *led, bool on);

/**
 * @brief Set the LED brightness (linear LEDs only).
 * @ingroup LED
 * @param led Pointer to the LED structure.
 * @param brightness Desired brightness (0 = off, 255 = full on).
 * @return True on success (including the degenerate binary case when only
 *         0 or 255 are meaningful), false if unsupported or invalid.
 *
 * Cancels any ongoing blink or fade before applying the new brightness.
 * For non-PWM LEDs this degenerates to binary on/off; intermediate values
 * are coerced to 0 or 255.
 */
bool hw_led_set_brightness(hw_led_t *led, uint8_t brightness);

/**
 * @brief Blink the LED (toggle full on/off).
 * @ingroup LED
 * @param led Pointer to the LED structure.
 * @param period_ms Full ON+OFF cycle period in milliseconds.
 * @param repeats True to continue indefinitely, false to perform a single
 *                full cycle (ending in OFF).
 * @return True on success, false otherwise.
 *
 *  - period_ms defines the total time for ON then OFF (two half-periods).
 *  - The implementation enforces a minimum full cycle (currently 200 ms)
 *    for stability; shorter requests are clamped.
 *  - Cancels any prior blink/fade before starting.
 */
bool hw_led_blink(hw_led_t *led, uint32_t period_ms, bool repeats);

/**
 * @brief Fade (breathe) the LED (PWM LEDs only).
 * @ingroup LED
 * @param led Pointer to the LED structure.
 * @param period_ms Full up+down brightness cycle period in milliseconds.
 * @param repeats True to continue indefinitely, false to perform one full
 * cycle.
 * @return True on success, false otherwise.
 *
 *  - period_ms spans a full 0→255→0 traversal.
 *  - Interval per brightness step ~ period_ms / 510 (minimum 1 ms).
 *  - If linear (PWM) capability is absent, this function transparently
 *    falls back to hw_led_blink() using the same period.
 *  - Cancels any prior blink/fade before starting.
 */
bool hw_led_fade(hw_led_t *led, uint32_t period_ms, bool repeats);
