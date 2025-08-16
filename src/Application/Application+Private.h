/**
 * @file Application+Private.h
 * @brief Application class private header.
 */
#pragma once
#include <Application/Application.h>

/**
 * @brief Callback function for application timer events.
 */
void _app_timer_callback(sys_timer_t *timer);

/**
 * @brief Category for private methods of the Application class.
 */
@interface Application (Private)

/**
 * @brief Sets the command-line arguments passed to the application.
 * @param args An array of command-line arguments as strings.
 */
- (void)setArgs:(NXArray *)args;

/**
 * @brief Handles application signals.
 * @param signal The signal received from the environment.
 */
- (void)signal:(NXApplicationSignal)signal;

/**
 * @brief Sets the application delegate.
 * @param delegate The object to serve as the application delegate, or nil to
 * remove the current delegate.
 *
 * This method assigns an object to serve as the application's delegate.
 * The delegate object should conform to the ApplicationDelegate protocol
 * and will receive notifications about application lifecycle events.
 *
 * The application does not retain the delegate object, so the caller is
 * responsible for ensuring the delegate remains valid for the lifetime
 * of the application or until a new delegate is set.
 *
 * @note Setting a new delegate replaces any previously set delegate.
 * @note It is safe to pass nil to remove the current delegate.
 * @see delegate for getting the current application delegate.
 * @see ApplicationDelegate protocol for delegate methods.
 */
- (void)setDelegate:(id<ApplicationDelegate>)delegate;

/**
 * @brief Starts the application's main run loop.
 * @return An integer exit status code, which can be set with
 * terminateWithExitStatus:
 *
 * This method begins the application's main execution cycle,
 * processing events and maintaining the application state until a
 * termination condition is met. The run loop continues executing until
 * the application receives a termination signal or explicitly requests to
 * stop.
 *
 * This method typically blocks the calling thread and should be called
 * from the main thread after all application initialization is complete.
 */
- (int)run;

@end
