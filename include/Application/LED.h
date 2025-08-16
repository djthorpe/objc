/**
 * @file LED.h
 * @brief Defines a class for controlling Light Emitting Diodes (LEDs).
 */
#pragma once
#include <runtime-hw/hw.h>

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The LED class
 * @ingroup Application
 * @headerfile LED.h Application/Application.h
 *
 * LED represents a class for changing Light Emitting Diode (LED) state.
 * You can create a LED instance to control the state (on/off) and brightness
 * of the LED, depending on the hardware capabilities.
 *
 * LEDs can either be the on-board status LED or external LEDs connected to GPIO
 * pins, and can either be configured as binary (on/off) or support varying
 * linear brightness levels, using PWM (Pulse Width Modulation) for finer
 * control.
 */
@interface LED : NXObject {
@private
  hw_led_t _led; ///< Pointer to the LED data
}

/**
 * @brief Returns the on-board status LED.
 * @return An LED instance if the board exposes a status LED; nil otherwise.
 *
 * Returns an instance of the on-board status LED, which may be simple on/off or
 * able to display different brightness or colors.
 *
 */
+ (LED *)status;

/**
 * @brief Create or return a binary (on/off) LED on a GPIO pin.
 * @param pin GPIO pin number.
 * @return An LED instance configured for on/off only, or nil on error
 *         (invalid pin or unsupported).
 */
+ (LED *)binaryOnPin:(uint8_t)pin;

/**
 * @brief Create or return a linear-brightness LED on a GPIO/PWM-capable pin.
 * @param pin GPIO pin number (should support PWM for brightness control).
 * @return An LED instance with brightness control, or nil on error.
 */
+ (LED *)linearOnPin:(uint8_t)pin;

/**
 * @brief Set the LED on/off state.
 * @param on YES to turn on, NO to turn off.
 * @return YES on success; NO if not supported or on failure.
 */
- (BOOL)setState:(BOOL)on;

/**
 * @brief Set LED brightness (linear scale).
 * @param brightness Brightness from 0 (off) to 255 (full).
 * @return YES on success; NO if brightness is unsupported for this LED.
 *
 * For binary LEDs, non-zero values are typically treated as ON.
 */
- (BOOL)setBrightness:(uint8_t)brightness;

/**
 * @brief Blink the LED at a fixed duty cycle.
 * @param duration Period in seconds for a full on/off cycle.
 * @param repeats  YES to continue blinking until stopped; NO for one cycle.
 * @return YES if the blink was started; NO if unsupported or busy.
 */
- (BOOL)blinkWithDuration:(NXTimeInterval)duration repeats:(BOOL)repeats;

/**
 * @brief Fade the LED brightness up/down.
 * @param duration Duration in seconds for a full fade in/out cycle.
 * @param repeats  YES to continue fading until stopped; NO for one cycle.
 * @return YES if the fade was started; NO if unsupported or busy.
 */
- (BOOL)fadeWithDuration:(NXTimeInterval)duration repeats:(BOOL)repeats;

@end
