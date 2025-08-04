/**
 * @file NXTimer.h
 * @brief Defines a class for controlling timers.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The NXTimer class
 * @ingroup Application
 * @headerfile NXTimer.h NXApplication/NXApplication.h
 *
 * NXTimer represents a class for controlling timers.
 */
@interface NXTimer : NXObject {
@protected
  sys_timer_t _timer;          ///< Scheduled timer
  id<TimerDelegate> _delegate; ///< The timer delegate
  BOOL _repeats; ///< Flag to indicate if the timer should be canceled on
                 ///< firing
}

/**
 * @brief Creates a new timer with the specified time interval and repeat
 * behaviour.
 * @param interval The time interval in seconds between timer firings.
 * @param repeats YES if the timer should repeat automatically, NO for a
 * one-shot timer.
 * @return A new NXTimer instance, or nil if the timer could not be created.
 *
 * This class method creates and returns a new timer configured with the
 * specified interval and repeat behavior. The timer is not automatically
 * scheduled - you must set a delegate to ensure the timer is properly managed.
 */
+ (NXTimer *)timerWithTimeInterval:(NXTimeInterval)interval
                           repeats:(BOOL)repeats;

/**
 * @brief Gets the current timer delegate.
 * @return The current timer delegate, or nil if no delegate is set.
 *
 * This method returns the object that serves as the timer's delegate.
 *
 * @see setDelegate: for setting the timer delegate.
 */
- (id<TimerDelegate>)delegate;

/**
 * @brief Sets the timer delegate.
 * @param delegate The object to set as the timer delegate, or nil to remove the
 * current delegate.
 *
 * The delegate object will receive timer-related callbacks when the timer
 * fires. The delegate should conform to the TimerDelegate protocol.
 *
 * @see delegate for getting the current timer delegate.
 */
- (void)setDelegate:(id<TimerDelegate>)delegate;

/**
 * @brief Manually fires the timer.
 * @return YES if the timer was successfully fired, NO otherwise.
 *
 * This method immediately triggers the timer's firing behavior, calling the
 * delegate's timer callback method. For repeating timers, this does not affect
 * the regular firing schedule.
 */
- (BOOL)fire;

/**
 * @brief Invalidates and stops the timer.
 *
 * Once a timer is invalidated, it cannot be reused and will no longer fire.
 * This method should be called to clean up timers that are no longer needed
 * to prevent memory leaks and unwanted timer callbacks.
 */
- (void)invalidate;

/**
 * @brief Checks if the timer is still valid and active.
 * @return YES if the timer is valid and can fire, NO if it has been
 * invalidated.
 *
 * A timer becomes invalid when it has been explicitly invalidated or when
 * a non-repeating timer has already fired.
 */
- (BOOL)valid;

/**
 * @brief Gets the timer's configured time interval.
 * @return The time interval in seconds between timer firings.
 *
 * This returns the interval that was set when the timer was created.
 * The interval cannot be changed after timer creation.
 */
- (NXTimeInterval)timeInterval;

@end
