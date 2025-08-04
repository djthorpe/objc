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
 *
 * @example examples/NXApplication/helloworld/main.m
 * @example examples/NXApplication/gpio/main.m
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
 * @brief This method notifies the app that you want to exit the run loop.
 */
- (void)stop;

@end
