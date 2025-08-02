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

#ifdef __cplusplus
}
#endif
