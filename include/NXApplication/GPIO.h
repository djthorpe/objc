/**
 * @file GPIO.h
 * @brief Defines a class for controlling General Purpose Input/Output (GPIO)
 * pins.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The GPIO class
 * @ingroup Application
 * @headerfile GPIO.h NXApplication/NXApplication.h
 *
 * GPIO represents a class for controlling General Purpose Input/Output (GPIO)
 * pins.
 *
 */
@interface GPIO : NXObject {
@private
  void *_data; ///< Pointer to the GPIO data
}

/**
 * @brief Returns a GPIO input instance.
 * @param pin The GPIO pin number to configure as input.
 * @return A new GPIO instance configured for input, or nil if the pin is
 * invalid or initialization failed.
 *
 * This method creates a GPIO instance configured as a floating input pin.
 * The pin will not have any internal pull-up or pull-down resistors enabled,
 * so external circuitry should be used to ensure proper logic levels.
 *
 * @note The pin number must be valid for the target platform.
 * @see pullupWithPin: for input with pull-up resistor
 * @see pulldownWithPin: for input with pull-down resistor
 */
+ (GPIO *)inputWithPin:(uint8_t)pin;

/**
 * @brief Returns a GPIO input instance, with pull-up resistor enabled.
 * @param pin The GPIO pin number to configure as input with pull-up.
 * @return A new GPIO instance configured for input with pull-up, or nil if the
 * pin is invalid or initialization failed.
 *
 * This method creates a GPIO instance configured as an input pin with the
 * internal pull-up resistor enabled. The pin will read as logical high (1)
 * when not driven low by external circuitry.
 *
 * @note The pin number must be valid for the target platform.
 * @see inputWithPin: for floating input
 * @see pulldownWithPin: for input with pull-down resistor
 */
+ (GPIO *)pullupWithPin:(uint8_t)pin;

/**
 * @brief Returns a GPIO input instance, with pull-down resistor enabled.
 * @param pin The GPIO pin number to configure as input with pull-down.
 * @return A new GPIO instance configured for input with pull-down, or nil if
 * the pin is invalid or initialization failed.
 *
 * This method creates a GPIO instance configured as an input pin with the
 * internal pull-down resistor enabled. The pin will read as logical low (0)
 * when not driven high by external circuitry.
 *
 * @note The pin number must be valid for the target platform.
 * @see inputWithPin: for floating input
 * @see pullupWithPin: for input with pull-up resistor
 */
+ (GPIO *)pulldownWithPin:(uint8_t)pin;

/**
 * @brief Returns a GPIO output instance.
 * @param pin The GPIO pin number to configure as output.
 * @return A new GPIO instance configured for output, or nil if the pin is
 * invalid or initialization failed.
 *
 * This method creates a GPIO instance configured as an output pin.
 * The initial state of the pin is platform-dependent and should be
 * explicitly set using setState: after creation.
 *
 * @note The pin number must be valid for the target platform.
 * @see setState: for controlling the output state
 */
+ (GPIO *)outputWithPin:(uint8_t)pin;

/**
 * @brief Returns the total number of GPIO pins available.
 * @return The total number of GPIO pins. Returns 0 if GPIO is not available.
 */
+ (uint8_t)count;

/**
 * @brief Returns the state of the GPIO pin.
 * @return true if the pin is high (logical 1), false if low (logical 0).
 */
- (BOOL)state;

/**
 * @brief Sets the state of the GPIO pin.
 * @param state true to set the pin high (logical 1), false to set low (logical
 * 0).
 */
- (void)setState:(BOOL)state;

@end
