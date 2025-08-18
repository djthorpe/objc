/**
 * @file NXApplicationTypes.h
 * @brief Application framework types
 *
 * Application-related classes and types.
 */
#pragma once

/**
 * @brief Application signal types for handling system events.
 * @ingroup Application
 *
 * This enum defines the various signals that the application delegate can
 * receive related to system events.
 *
 * NXApplicationSignalInt indicates that the application is being interrupted,
 * usually by the user pressing Ctrl+C. NXApplicationSignalQuit indicates that
 * the application is being asked to quit, typically by the user sending a
 * SIGQUIT signal. NXApplicationSignalTerm indicates that the application is
 * being terminated, typically by the user sending a SIGTERM signal. When the
 * watchdog is enabled, it may also indicate that the application manually
 * triggered a restart.
 *
 * NXApplicationSignalPowerSourceUSB indicates that the power source has changed
 * to USB. NXApplicationSignalPowerSourceBattery indicates that either the power
 * source has changed to battery, or the battery level has changed. The battery
 * level can be monitored using the appropriate APIs.
 */
typedef enum {
  NXApplicationSignalNone = 0, ///< No signal
  NXApplicationSignalTerm =
      (1 << 0), ///< Termination signal (SIGTERM equivalent)
  NXApplicationSignalInt =
      (1 << 1), ///< Interrupt signal (SIGINT/Ctrl+C equivalent)
  NXApplicationSignalQuit = (1 << 2),     ///< Quit signal (SIGQUIT equivalent)
  NXApplicationPowerSourceUSB = (1 << 3), ///< Power source changed to USB
  NXApplicationPowerSourceBattery =
      (1 << 4), ///< Power source changed to battery, or battery level changed
  NXApplicationWatchdogDidReset =
      (1 << 5), ///< Application was reset by a watchdog timer
} NXApplicationSignal;

/**
 * @brief Application capabilities.
 * @ingroup Application
 *
 * Define capabilities to enable in the application. These are passed to
 * NXApplicationMain which then enables the corresponding features.
 *
 * NXApplicationCapabilityMulticore enables the use of multiple CPU cores to
 * process tasks concurrently. NXApplicationCapabilityPower enables power
 * management features, which means additional signals are passed to the
 * application for power-related events, such as power source and battery
 * changes.
 *
 * NXApplicationCapabilityWatchdog enables the use of a watchdog timer to
 * monitor the application's health, and resets the device if it becomes
 * unresponsive.
 */
typedef enum {
  NXApplicationCapabilityNone = 0,             ///< No capability
  NXApplicationCapabilityMulticore = (1 << 0), ///< Enable Multicore capability
  NXApplicationCapabilityPower = (1 << 1),     ///< Enable Power management
  NXApplicationCapabilityWatchdog = (1 << 2),  ///< Enable Watchdog
} NXApplicationCapability;
