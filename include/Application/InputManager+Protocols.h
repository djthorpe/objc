/**
 * @file InputManager+Protocols.h
 * @brief Defines a protocols for the input manager delegate.
 *
 * The InputManagerDelegate protocol defines methods that are called by the
 * input manager when key and movement events occur. The InputManagerSource
 * defines the input sources (e.g., keyboard, mouse, touchscreen) that can
 * generate these events.
 */
#pragma once

/**
 * @protocol InputManagerSource
 * @ingroup Application
 * @headerfile InputManager+Protocols.h Application/Application.h
 * @brief A protocol that defines the methods for an input manager source.
 */
@protocol InputManagerSource

// TODO: Nothing here yet

@end

/**
 * @protocol InputManagerDelegate
 * @ingroup Application
 * @headerfile InputManager+Protocols.h Application/Application.h
 * @brief A protocol that defines the methods for an input manager delegate.
 */
@protocol InputManagerDelegate

/**
 * @brief Delegate callback for key/button events.
 *
 * @param sender The source of the event (input device or source object).
 * @param code The platform-independent logical key code (see KEYCODE_*).
 * @param scanCode Hardware scan code or platform-specific scancode; may be
 *                 0 if unavailable.
 * @param ch The character produced by the key event, or 0 if none.
 * @param state Bitmask of `NXInputState` flags representing the current
 *              key/button state and modifier state (clicks, repeat, shift,
 *              control, etc.).
 *
 * Called when a key, button, or touchscreen press is reported to the input
 * manager. The delegate receives both the logical key code and the hardware
 * scan code (if available), along with a character value when the event
 * produces a printable character.
 *
 * If the event represents a touchscreen press, the input manager will
 * emit `KEYCODE_BTNTOUCH` as the logical code and set the corresponding
 * state flags.
 */
- (void)keyEvent:(id<InputManagerSource>)sender
            code:(uint16_t)keyCode
        scanCode:(uint16_t)scanCode
            char:(char)ch
           state:(NXInputState)state;

/**
 * @brief Delegate callback for pointer movement (mouse or touchscreen).
 *
 * @param sender The source of the movement event.
 * @param point The X/Y coordinates.
 * @param slot For touch events this may contain the contact slot/index; for
 *             simple pointer devices this will typically be 0.
 *
 * Reports pointer motion from mice, touchpads, or touchscreens. The
 * `point` parameter should be interpreted as an absolute value, which can
 * be positive or negative, depending on the device.
 */
- (void)moveEvent:(id<InputManagerSource>)sender
            point:(NXPoint)point
             slot:(uint8_t)slot;

@end
