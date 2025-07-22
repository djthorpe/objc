/**
 * @file sys.h
 * @brief Defines system management functions.
 *
 * This file declares various system methods for managing memory, threads
 * and resources.
 */
#pragma once
#include "systhread.h"
#include "systime.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Allocates a block of memory.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or `NULL` if the allocation fails.
 */
extern void *sys_malloc(size_t size);

/**
 * @brief Frees a block of memory.
 * @param ptr A pointer to the memory block to be deallocated.
 */
extern void sys_free(void *ptr);

/**
 * @brief Pauses the execution of the current thread for a specified time.
 * @param msec The number of milliseconds to sleep.
 */
extern void sys_sleep(int32_t msec);

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

/**
 * @brief Prints formatted output to the system console.
 * @param format A printf-style format string that specifies how subsequent
 *               arguments are formatted and printed.
 * @param ... Additional arguments corresponding to format specifiers in format.
 * @return The number of characters printed.
 */
extern size_t sys_printf(const char *format, ...);

/**
 * @brief Prints formatted output using a va_list argument.
 * @param format A printf-style format string.
 * @param args A va_list containing the arguments for the format string.
 * @return The number of characters printed.
 */
extern size_t sys_vprintf(const char *format, va_list args);

/**
 * @brief Prints formatted output to a string buffer.
 * @param buf Pointer to the destination buffer where the formatted string
 *            will be stored. If NULL, only the length is calculated.
 * @param sz Size of the destination buffer in bytes, including space for
 *           the null terminator.
 * @param format A printf-style format string.
 * @param ... Additional arguments corresponding to format specifiers in format.
 * @return The number of characters that would have been written if the buffer
 *         was sufficiently large, not counting the null terminator.
 */
extern size_t sys_sprintf(char *buf, size_t sz, const char *format, ...);

/**
 * @brief Prints formatted output to a string buffer using a va_list argument.
 * @param buf Pointer to the destination buffer where the formatted string
 *            will be stored. If NULL, only the length is calculated.
 * @param sz Size of the destination buffer in bytes, including space for
 *           the null terminator.
 * @param format A printf-style format string.
 * @param args A va_list containing the arguments for the format string.
 * @return The number of characters that would have been written if the buffer
 *         was sufficiently large, not counting the null terminator.
 */
extern size_t sys_vsprintf(char *buf, size_t sz, const char *format,
                           va_list args);