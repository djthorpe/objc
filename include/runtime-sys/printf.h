/**
 * @file printf.h
 * @brief Functions for formatted strings output.
 * @defgroup SystemFormatting String Formatting
 * @ingroup System
 *
 * Methods for creating and outputting formatted strings.
 *
 * This file declares types and functions for formatted output to the system
 * console or to a string buffer. It supports printf-style formatting with
 * various format specifiers, width modifiers, and flags.
 *
 * The sys_printf() function is the main entry point for formatted output,
 * and it supports a wide range of format specifiers, including integers,
 * strings, and characters. It also supports width modifiers for padding
 * and alignment, as well as flags for zero-padding, left-alignment, and
 * sign handling. There are also functions for formatted output of Objective-C
 * objects and NXTimeInterval values.
 *
 * The sys_sprintf() function is similar to sys_printf(), but it outputs
 * the formatted string to a buffer instead of the console. It also supports
 * the same format specifiers and flags, and it returns the number of characters
 * that would have been written to the buffer, not counting the null terminator.
 *
 * The format specifiers supported are:
 * - `%%c`: Character
 * - `%%s`: String
 * - `%@`: NXObject (Objective-C object). This only works if the Objective-C
 * runtime is available.
 * - `%%d`: Signed integer
 * - `%%u`: Unsigned integer
 * - `%%x`: Unsigned hexadecimal integer
 * - `%%b`: Unsigned binary integer
 * - `%%o`: Unsigned octal integer
 * - `%%X`: Uppercase hexadecimal integer
 * - `%%t`: NXTimeInterval (Objective-C time interval)
 * - `%%p`: Pointer address
 * - `%%`: Literal percent sign
 *
 *
 * The format specifiers can be modified with flags and width:
 * - Width:
 *   - `%80d`: Number specifying the minimum width of the output
 * - Data Type:
 *   - `%%ld` or `%%lu` long data type
 *   - `%%zu` size_t data type
 * - Flags:
 *   - `%-10s`: Left-aligned output
 *   - `%05d`: Zero-padded outpu
 *   - `%+d`: Forces a sign for positive numbers
 *   - `%#x`, `%#b`, `%#o`: Forces a prefix for hexadecimal (0x), binary (0b)
 * and octal (0) formats
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
