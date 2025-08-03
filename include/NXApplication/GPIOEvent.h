/**
 * @file GPIOEvent.h
 * @brief Defines a class for GPIO event handling.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The GPIO Event class
 * @ingroup Application
 * @headerfile GPIOEvent.h NXApplication/NXApplication.h
 *
 * GPIOEvent represents a class for handling events related to General Purpose
 * Input/Output (GPIO) pins.
 *
 */
@interface GPIOEvent : NXObject {
@private
  uint8_t _pin;           ///< The GPIO pin number associated with the event
  hw_gpio_event_t _event; ///< The type of GPIO event (e.g., rising, falling)
}

/**
 * @brief Returns the GPIO pin number associated with the event.
 * @return The GPIO pin number.
 */
- (uint8_t)pin;

@end
