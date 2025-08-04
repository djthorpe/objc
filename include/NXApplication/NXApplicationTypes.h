/**
 * @file NXApplicationTypes.h
 * @brief Application framework types
 * @ingroup Application
 *
 * Application-related classes and types.
 */
#pragma once

/**
 * @brief Application signal types for handling system events.
 * @ingroup Application
 */
typedef enum {
  NXApplicationSignalNone = 0, ///< No signal
  NXApplicationSignalTerm =
      (1 << 0), ///< Termination signal (SIGTERM equivalent)
  NXApplicationSignalInt =
      (1 << 1), ///< Interrupt signal (SIGINT/Ctrl+C equivalent)
  NXApplicationSignalQuit = (1 << 2), ///< Quit signal (SIGQUIT equivalent)
} NXApplicationSignal;
