/**
 * @file Application.h
 * @brief The main application class for NXApplication.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief The main application class that coordinates application-wide
 * functionality.
 * @ingroup Application
 *
 * The Application class serves as the central hub for application management
 * in NXApplication. It follows the singleton pattern, providing a shared
 * instance that manages the application lifecycle, coordinates event
 * processing, and maintains the main run loop.
 */
@interface Application : NXObject {
@private
  void *_data;                       ///< Pointer to the application data
  id<ApplicationDelegate> _delegate; ///< The application delegate
  BOOL _run;  ///< Flag to indicate if the application is running
  BOOL _stop; ///< Flag to indicate if the run loop should stop
}

/**
 * @brief Returns the shared application instance.
 */
+ (id)sharedApplication;

/**
 * @brief Gets the current application delegate.
 * @return The current application delegate, or nil if no delegate is set.
 *
 * This method returns the object that serves as the application's delegate.
 *
 * @see setDelegate: for setting the application delegate.
 * @see ApplicationDelegate protocol for delegate methods.
 */
- (id<ApplicationDelegate>)delegate;

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

/**
 * @brief This method notifies the app that you want to exit the run loop.
 */
- (void)stop;

@end
