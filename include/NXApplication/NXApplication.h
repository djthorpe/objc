
/**
 * @file NXApplication.h
 * @brief Application framework header
 * @defgroup Application Application Framework
 * @ingroup objc
 *
 * Application-related classes and types, including application lifecycle,
 * event handling, display and frames, and hardware interfaces.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>
#include <objc/objc.h>

/**
 * @brief Main entry point for NXApplication-based programs.
 * @ingroup Application
 * @param argc The number of command-line arguments passed to the program.
 * @param argv An array of command-line argument strings.
 * @param delegate The class to use as the application delegate.
 * @return The exit status of the application (0 for success, non-zero for
 * error).
 *
 * This function serves as the main entry point for applications built with the
 * NXApplication framework. It initializes the application runtime, sets up the
 * event loop, and manages the application lifecycle from startup to shutdown.
 *
 * The function handles:
 * - Runtime initialization and configuration
 * - Command-line argument processing
 * - Application instance creation and setup
 * - Main event loop execution
 * - Cleanup and resource deallocation on exit
 *
 * This function should be called from your program's main() function, typically
 * by simply returning the result of NXApplicationMain().
 *
 * @note This function does not return until the application terminates.
 * @note Command-line arguments are processed according to application
 * configuration.
 * @note The return value should be used as the program's exit status.
 *
 * @code
 * int main(int argc, char *argv[]) {
 *     return NXApplicationMain(argc, argv);
 * }
 * @endcode
 */
int NXApplicationMain(int argc, char *argv[], Class delegate);

#if __OBJC__

// Forward Declaration of classes
@class Application;
@class GPIO;

// Protocols and Category Definitions
#include "ApplicationDelegate+Protocol.h"

// Class Definitions
#include "Application.h"
#include "GPIO.h"

#endif // __OBJC__
