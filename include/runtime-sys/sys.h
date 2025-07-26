/**
 * @file sys.h
 * @brief Defines system management functions.
 * @defgroup System
 *
 * This file declares various system methods for managing memory, threads
 * and resources.
 */
#pragma once
#include "hash.h"
#include "memory.h"
#include "printf.h"
#include "random.h"
#include "thread.h"
#include "time.h"
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
 */
extern void sys_init(void);

/**
 * @brief Cleans up the system on shutdown.
 * @ingroup System
 */
extern void sys_exit(void);

/**
 * @brief Aborts the current process immediately.
 * @ingroup System
 * @note This function does not return to the caller.
 *
 *  This function terminates the current process abnormally and
 *  immediately. It does not perform any cleanup operations and
 *  does not call exit handlers or destructors.
 */
extern void sys_abort(void);

/**
 * @brief Prints a formatted panic message and aborts the process.
 * @ingroup System
 * @param fmt A printf-style format string specifying the panic message.
 * @param ... Additional arguments corresponding to format specifiers in fmt.
 * @note This function does not return to the caller.
 */
extern void sys_panicf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
