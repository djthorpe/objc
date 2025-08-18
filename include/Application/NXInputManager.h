/**
 * @file NXInputManager.h
 * @brief The input manager class for handling user input.
 */
#pragma once
#include <Foundation/Foundation.h>

/**
 * @brief The input manager class that handles user input.
 * @ingroup Application
 * @headerfile NXInputManager.h Application/Application.h
 */
@interface NXInputManager : NXObject {
@private
  id<InputManagerDelegate> _delegate; ///< The input manager delegate
}

/**
 * @brief Returns the shared input manager instance.
 */
+ (id)sharedInstance;

/**
 * @brief Gets the current input manager delegate.
 * @return The current input manager delegate, or nil if no delegate is set.
 *
 * This method returns the object that serves as the input manager's delegate.
 *
 * @see setDelegate: for setting the input manager delegate.
 */
- (id<InputManagerDelegate>)delegate;

/**
 * @brief Sets the input manager delegate.
 * @param delegate The object to set as the input manager delegate, or nil to
 * remove the current delegate.
 *
 * The delegate object will receive input manager-related callbacks when the
 * input manager fires. The delegate should conform to the InputManagerDelegate
 * protocol.
 */
- (void)setDelegate:(id<InputManagerDelegate>)delegate;

/**
 * @brief Trigger a key event.
 *
 * Notify the input manager that a key or button event occurred. The manager
 * will translate the `keyCode`/`scanCode` into an input state and notify its
 * delegate via the configured `InputManagerDelegate` callbacks.
 *
 * @param sender The source that generated the event (may be nil).
 * @param keyCode The platform-independent key code (see KEYCODE_* values).
 * @param scanCode Hardware scan code or scancode variant for the key event;
 *                 this may be 0 if unavailable. Implementations may use the
 *                 scan code for low-level mapping or to disambiguate keys.
 *
 * @note The input manager will combine this event with existing state flags
 *       (see `NXInputState`) and emit higher-level events such as clicks,
 *       long-press, or repeat as appropriate.
 */
- (void)keyEvent:(id<InputManagerSource>)sender
         keyCode:(uint16_t)keyCode
        scanCode:(uint16_t)scanCode;

/**
 * @brief Report movement for pointer devices (mouse, touch, or stylus).
 *
 * Use this method to report either relative motion (for mice) or absolute
 * coordinates (for touchscreens). The `point` contains device coordinates; the
 * interpretation depends on the `absolute` flag.
 *
 * @param sender The source that generated the movement event.
 * @param point The X/Y coordinates of the movement. For relative motion this
 *              is an offset; for absolute motion it is a device coordinate.
 * @param absolute When YES the `point` is absolute coordinates; when NO the
 *                 `point` is a delta (relative) movement.
 * @param slot Non-zero indicates a touch/press is active. When set the
 *              input manager will also emit `KEYCODE_BTNTOUCH` as a key event
 *              alternative; use 0 this is not a touch event.
 */
- (void)moveEvent:(id<InputManagerSource>)sender
            point:(NXPoint)point
         absolute:(BOOL)absolute
             slot:(uint8_t)slot;
