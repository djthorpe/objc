/**
 * @file NXApplication.h
 * @brief The main application class for NXFoundation applications.
 */
#pragma once
#include "NXObject.h"

/**
 * @brief The main application class that coordinates application-wide
 * functionality.
 * @ingroup Foundation
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
+ (int)run;

@end
