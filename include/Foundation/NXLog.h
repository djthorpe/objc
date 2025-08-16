/**
 * @file NXLog.h
 * @brief Defines a logging function for printing formatted messages.
 * @details This file provides the `NXLog` function, which is a convenient way
 * to print formatted output to the standard error stream. It is similar to
 * `NSLog` from the Foundation framework.
 */
#pragma once
#include <objc/objc.h>
#include <runtime-sys/sys.h>

/**
 * @def NXLog(format, ...)
 * @ingroup Foundation
 * @headerfile NXLog.h Foundation/Foundation.h
 * @brief Logs a formatted string to the standard output stream.
 * @param format An `NXConstantString` object that contains a printf-style
 * format string with support for %@ (objects) and %t (time intervals).
 * @param ... A comma-separated list of arguments to be printed.
 * @return The length of the formatted string that was printed, not including
 * the newline character.
 *
 * Takes a format string and a variable number of arguments,
 * formats them, and prints the result to standard output, followed by a newline
 * character. The output is automatically flushed.
 *
 * In addition to standard printf format specifiers, NXLog supports:
 * - %@ for formatting objects (calls their description method)
 * - %t for formatting NXTimeInterval values
 *
 * For other formatting options, refer to the documentation for `sys_printf`.
 *
 */
size_t NXLog(id<NXConstantStringProtocol> format, ...);
