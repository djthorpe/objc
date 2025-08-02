/**
 * @file Application.h
 * @brief The main application class for NXApplication.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

/**
 * @brief The main application class that coordinates application-wide
 * functionality.
 * @ingroup Application
 * @headerfile Application.h NXApplication/NXApplication.h
 *
 * The Application class serves as the central hub for application management
 * in NXApplication. It follows the singleton pattern, providing a shared
 * instance that manages the application lifecycle, coordinates event
 * processing, and maintains the main run loop.
 */
@interface Application : NXObject {
@private
  sys_event_queue_t _queue;          ///< Event queue
  id<ApplicationDelegate> _delegate; ///< The application delegate
  BOOL _run;      ///< Flag to indicate if the application is running
  NXArray *_args; ///< Command-line arguments passed to the application
}

/**
 * @brief Returns the shared application instance.
 */
+ (id)sharedApplication;

/**
 * @brief Returns command-line arguments passed to the application.
 * @return An array of command-line arguments as strings.
 *
 * This method retrieves the command-line arguments that were passed to the
 * application at startup. It returns an array containing the arguments,
 * the first of which is typically the application name or path.
 */
- (NXArray *)args;

/**
 * @brief Sets the command-line arguments passed to the application.
 * @param args An array of command-line arguments as strings.
 */
- (void)setArgs:(NXArray *)args;

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
