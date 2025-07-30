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
 * @headerfile NXLog.h NXFoundation/NXFoundation.h
 * @brief Logs a formatted string to the standard error stream.
 * @param format An `NXConstantString` object that contains a C-style format
 * string.
 * @param ... A comma-separated list of arguments to be printed.
 * @return The length of the formatted string that was be printed, not including
 * the null terminator.
 *
 * Takes a format string and a variable number of arguments,
 * formats them, and prints the result to `stderr`, followed by a newline
 * character.
 *
 * For formatting options, refer to the documentation for `sys_printf`.
 *
 */
size_t NXLog(id<NXConstantStringProtocol> format, ...);
