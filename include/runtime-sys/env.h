/**
 * @file env.h
 * @brief Environment information.
 * @defgroup SystemEnv Environment
 * @ingroup System
 *
 * System methods for reading information about the environment.
 */
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Environment signal types.
 * @ingroup SystemEnv
 *
 * Enumeration of signal types that can be received from the environment
 * or operating system. These signals typically indicate termination or
 * interrupt requests that applications should handle gracefully.
 */
typedef enum {
  SYS_ENV_SIGNAL_NONE = 0, ///< No signal
  SYS_ENV_SIGNAL_TERM =
      (1 << 0), ///< Termination signal (termination request from OS)
  SYS_ENV_SIGNAL_INT =
      (1 << 1), ///< Interrupt signal (interrupt request from user)
  SYS_ENV_SIGNAL_QUIT = (1 << 2), ///< Quit signal (quit request from user)
} sys_env_signal_t;

/**
 * @brief Callback function type for handling environment signals.
 * @ingroup SystemEnv
 * @param signal The type of signal that was received.
 *
 * This typedef defines the signature for callback functions that handle
 * environment signals. The callback receives a signal type parameter
 * indicating which signal was received.
 */
typedef void (*sys_env_signal_callback_t)(sys_env_signal_t signal);

/**
 * @brief Sets a handler for environment signals.
 * @ingroup SystemEnv
 * @param mask Bitmask of sys_env_signal_t values indicating which signals to
 * handle. If zero, all signals are handled.
 * @param callback The callback function to handle signals, or NULL to disable
 * signal handling.
 * @return true if the handler was set or cleared successfully, false if signals
 * are not supported on this platform.
 *
 * This function registers a callback function that will be invoked when the
 * application receives environment signals such as termination requests
 * (SIGTERM), interrupt signals (SIGINT), or quit signals (SIGQUIT). The
 * callback allows applications to perform graceful shutdown procedures when
 * requested.
 *
 * Only one signal handler can be active at a time. Setting a new handler
 * will replace any previously registered handler.
 *
 * Not all platforms support all signal types. Embedded platforms may have
 * limited or no signal support.
 *
 * The signal handler may be called from interrupt context on some
 * platforms. Keep the implementation simple and avoid blocking operations,
 * memory allocation, or complex system calls within the callback.
 *
 * @see sys_env_signal_callback_t for callback function signature requirements.
 */
extern bool sys_env_signalhandler(sys_env_signal_t mask,
                                  sys_env_signal_callback_t callback);

/**
 * @brief Returns a unique identifier for the current environment.
 * @ingroup SystemEnv
 * @return A serial number or unique identifier as a string.
 */
extern const char *sys_env_serial(void);

/**
 * @brief Returns the name of the current environment.
 * @ingroup SystemEnv
 * @return The name of the current environment as a string, which is typically
 * the name of the program or application that is running.
 */
extern const char *sys_env_name(void);

/**
 * @brief Returns the version of the current environment.
 * @ingroup SystemEnv
 * @return The version of the current environment as a string, which is
 * typically the version of the program or application that is running.
 */
extern const char *sys_env_version(void);

#ifdef __cplusplus
}
#endif
