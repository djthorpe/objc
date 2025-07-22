/**
 * @file sys.h
 * @brief Defines system management functions.
 *
 * This file declares various system methods for managing memory, threads
 * and resources.
 */
#pragma once
#include "malloc.h"
#include "printf.h"
#include "thread.h"
#include "time.h"
#include <stdint.h>

/**
 * @brief Outputs a string to the system console or standard output.
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
 *
 * This function writes the specified character to the
 * system's standard output stream, but does not flush the output buffer.
 */
extern void sys_putch(const char ch);

/**
 * @brief Returns a random number as a 32-bit unsigned integer.
 * @return A random unsigned 32-bit integer value.
 * @warning The function may not be thread-safe depending on the platform
 *          implementation.
 *
 *  This function generates a random unsigned 32-bit integer value
 *  using the system's random number generator. The quality and
 *  distribution of randomness depends on the underlying platform
 *  implementation.
 */
extern uint32_t sys_random_uint32(void);

/**
 * @brief Aborts the current process immediately.
 * @note This function does not return to the caller.
 *
 *  This function terminates the current process abnormally and
 *  immediately. It does not perform any cleanup operations and
 *  does not call exit handlers or destructors.
 */
extern void sys_abort(void);

/**
 * @brief Prints a formatted panic message and aborts the process.
 * @param fmt A printf-style format string specifying the panic message.
 * @param ... Additional arguments corresponding to format specifiers in fmt.
 * @note This function does not return to the caller.
 */
extern void sys_panicf(const char *fmt, ...);
