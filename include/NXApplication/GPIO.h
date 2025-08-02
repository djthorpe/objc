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
 */
+ (GPIO *)inputWithPin:(uint8_t)pin;

/**
 * @brief Returns a GPIO input instance, with pull-up resistor enabled.
 */
+ (GPIO *)pullupWithPin:(uint8_t)pin;

/**
 * @brief Returns a GPIO input instance, with pull-up resistor enabled.
 */
+ (GPIO *)pulldownWithPin:(uint8_t)pin;

/**
 * @brief Returns a GPIO output instance.
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
