/**
 * @file NXApplicationMain.h
 * @brief Application framework main entry point
 */
#pragma once
#include "NXApplication+Types.h"
#include <Foundation/Foundation.h>
#include <runtime-hw/hw.h>

/**
 * @brief Main entry point for NXApplication-based programs.
 * @ingroup Application
 * @param argc The number of command-line arguments passed to the program.
 * @param argv An array of command-line argument strings.
 * @param delegate The class to use as the application delegate.
 * @param capabilities The capabilities to enable for the application.
 * @return The exit status of the application (0 for success, non-zero for
 * error).
 *
 * This function serves as the main entry point for applications built with the
 * Application framework. It initializes the application runtime, sets up the
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
 *     return NXApplicationMain(argc, argv, MyAppDelegateClass,
 * NXApplicationCapabilityMulticore | NXApplicationCapabilityPower);
 * }
 * @endcode
 */
int NXApplicationMain(int argc, char *argv[], Class delegate,
                      NXApplicationCapability capabilities);
