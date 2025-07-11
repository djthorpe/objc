/**
 * @file NXApplication.h
 * @brief The main application class for NXFoundation applications.
 */
#pragma once
#include "NXObject.h"

/**
 * @brief The main application class that coordinates application-wide
 * functionality.
 *
 * NXApplication serves as the central hub for application management
 * in NXFoundation. It follows the singleton pattern, providing a shared
 * instance that manages the application lifecycle, coordinates event
 * processing, and maintains the main run loop. This class is typically
 * subclassed to provide application-specific behavior and initialization.
 */
@interface NXApplication : NXObject {
}

/**
 * @brief Returns the shared application instance.
 */
+ (id)sharedApplication;

/**
 * @brief Starts the application's main run loop.
 *
 * This method begins the application's main execution cycle,
 * processing events and maintaining the application state until a termination
 * condition is met. The run loop continues executing until the application
 * receives a termination signal or explicitly requests to stop.
 *
 * This method typically blocks the calling thread and should be called from the
 * main thread after all application initialization is complete.
 */
- (void)run;

@end

/**
 * @brief Main entry point for NXFoundation applications with custom application
 * class.
 * @param app The Class object representing the NXApplication subclass to
 * instantiate. This class must inherit from NXApplication. If nil is passed,
 * the default NXApplication class will be used.
 * @return The application's exit status code. Typically 0 for success,
 *         or a non-zero value to indicate an error condition.
 */
extern int NXApplicationMain(Class app);
