/**
 * @file printf.h
 * @brief Functions for formatted strings output.
 * @defgroup SystemFormatting String Formatting
 * @ingroup System
 *
 * Methods for creating and outputting
 * formatted strings.
 */
#pragma once
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prints formatted output to the system console.
 * @ingroup SystemFormatting
 * @param format A printf-style format string that specifies how subsequent
 *               arguments are formatted and printed.
 * @param ... Additional arguments corresponding to format specifiers in format.
 * @return The number of characters printed.
 */
extern size_t sys_printf(const char *format, ...);

/**
 * @brief Prints formatted output using a va_list argument.
 * @ingroup SystemFormatting
 * @param format A printf-style format string.
 * @param args A va_list containing the arguments for the format string.
 * @return The number of characters printed.
 */
extern size_t sys_vprintf(const char *format, va_list args);

/**
 * @brief Prints formatted output to a string buffer.
 * @ingroup SystemFormatting
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
 * @ingroup SystemFormatting
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

#ifdef __cplusplus
}
#endif
