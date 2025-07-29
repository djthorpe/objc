/**
 * @file sys.h
 * @brief Defines system management functions.
 * @defgroup System System Management
 */
#pragma once
#include "date.h"
#include "event.h"
#include "hash.h"
#include "memory.h"
#include "printf.h"
#include "random.h"
#include "sync.h"
#include "thread.h"
#include "timer.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Outputs a string to the system console or standard output.
 * @ingroup System
 * @param str A pointer to a null-terminated string to be output. If `str` is
 * `NULL`, existing output is flushed.
 *
 * This function writes the specified null-terminated string to the
 * system's standard output stream, and flushes the output buffer.
 */
extern void sys_puts(const char *str);

/**
 * @brief Outputs a character to the system console or standard output.
 * @param ch The character to be output.
 * @ingroup System
 *
 * This function writes the specified character to the
 * system's standard output stream, but does not flush the output buffer.
 */
extern void sys_putch(const char ch);

/**
 * @brief Initializes the system on startup.
 * @ingroup System
 *
 * This function must be called at the start of the program to initialize
 * the system environment, including standard input/output streams and any
 * necessary subsystems. It prepares the system for normal operation.
 * It may include platform-specific initialization steps.
 */
extern void sys_init(void);

/**
 * @brief Cleans up the system on shutdown.
 * @ingroup System
 *
 * This function should be called at the end of the program to perform any
 * necessary cleanup tasks, such as releasing resources and shutting down
 * subsystems. It prepares the system for termination.
 */
extern void sys_exit(void);

#ifdef __cplusplus
}
#endif
