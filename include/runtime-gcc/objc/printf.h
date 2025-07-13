/**
 * @file printf.h
 * @brief String formatting functions with Objective-C extensions.
 */
#pragma once
#include <stdarg.h>
#include <stddef.h>

/**
 * @brief Format a string with variable arguments
 *
 * Supports all standard format specifiers plus Objective-C extensions:
 * - %s: null-terminated C string
 * - %d: signed integer (with %ld for long)
 * - %u: unsigned integer (with %lu for unsigned long)
 * - %x/%X: hexadecimal (lower/upper case, with %lx/%lX for long)
 * - %b: binary representation (with %lb for long)
 * - %@: Objective-C object (calls description method, Objective-C only)
 * - %%: literal percent character
 * - Long modifier 'l' supported for integer types
 *
 * @param buf The output buffer where the formatted string will be stored.
 *    If NULL, the function calculates the required length without writing.
 * @param sz The size of the output buffer in bytes. Ignored if buf is NULL.
 * @param format The format string containing directives to be replaced.
 * @param ... Variable arguments containing the values to be formatted.
 * @return The number of bytes that would be written (excluding the null
 *         terminator), regardless of the buffer size. If this value is greater
 *         than or equal to sz, the output was truncated.
 */
size_t objc_sprintf(char *buf, size_t sz, const char *format, ...);

/**
 * @brief Format a string with a va_list argument
 *
 * This is the va_list version of objc_sprintf(), allowing it to be called
 * from other variadic functions. Supports the same format specifiers as
 * objc_sprintf().
 *
 * @param buf The output buffer where the formatted string will be stored.
 *    If NULL, the function calculates the required length without writing.
 * @param sz The size of the output buffer in bytes. Ignored if buf is NULL.
 * @param format The format string containing directives to be replaced.
 * @param va A va_list containing the arguments for the format specifiers.
 * @return The number of bytes that would be written (excluding the null
 *         terminator), regardless of the buffer size. If this value is greater
 *         than or equal to sz, the output was truncated.
 * @see objc_sprintf()
 */
size_t objc_vsprintf(char *buf, size_t sz, const char *format, va_list va);
