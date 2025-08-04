/**
 * @file Application.h
 * @brief The main application class for NXApplication.
 *
 * @example examples/NXApplication/helloworld/main.m
 * @example examples/NXApplication/gpio/main.m
 * @example examples/NXApplication/timer/main.m
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief The main application class that coordinates application-wide
 * functionality.
 * @ingroup Application
 * @headerfile Application.h NXApplication/NXApplication.h
 *
 * The Application class serves as the central hub for application management
 * in NXApplication. The Application class is created automatically by invoking
 * the NXApplicationMain function, which initializes the application and
 * starts the main run loop.
 *
 * You don't create instances of this class directly; instead, you use the
 * sharedApplication method to access the singleton instance. The Application
 * class is responsible for handling command-line arguments, setting the
 * application delegate, starting the main run loop, handling events and then
 * terminating the application gracefully.
 */
@interface Application : NXObject {
@private
  id<ApplicationDelegate> _delegate; ///< The application delegate
  BOOL _run;       ///< Flag to indicate if the application is running
  int _exitstatus; ///< Exit status of the application
  NXArray *_args;  ///< Command-line arguments passed to the application
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
 * the first of which is typically the application name or path to the
 * executable.
 */
- (NXArray *)args;

/**
 * @brief Gets the current application delegate.
 * @return The current application delegate, or nil if no delegate is set.
 *
 * This method returns the object that serves as the application's delegate.
 *
 * @see ApplicationDelegate protocol for delegate methods.
 */
- (id<ApplicationDelegate>)delegate;

/**
 * @brief This method notifies the app that you want to exit the run loop.
 *
 * The remaining events in the run loop will be processed, and then the
 * application will terminate gracefully.
 *
 * @note This method does not immediately terminate the application; it
 * simply sets a flag that will be checked in the run loop.
 */
- (void)terminate;

/**
 * @brief This method notifies the app that you want to exit the run loop,
 * with a specific exit status.
 *
 * The remaining events in the run loop will be processed, and then the
 * application will terminate gracefully.
 *
 * @note This method does not immediately terminate the application; it
 * simply sets a flag that will be checked in the run loop. The process
 * will exit with the specified status code.
 */
- (void)terminateWithExitStatus:(int)status;

@end
