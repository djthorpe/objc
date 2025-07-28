/**
 * @file NXLog.h
 * @brief Defines a logging macro for printing formatted messages.
 * @details This file provides the `NXLog` macro, which is a convenient way to
 * print formatted output to the standard error stream. It is similar to `NSLog`
 * from the Foundation framework.
 */
#pragma once
#include <runtime-sys/sys.h>

/**
 * @def NXLog(format, ...)
 * @ingroup Foundation
 * @brief Logs a formatted string to the standard error stream.
 * @param format An `NXConstantString` object that contains a C-style format
 * string.
 * @param ... A comma-separated list of arguments to be printed.
 * @details This macro takes a format string and a variable number of arguments,
 * formats them, and prints the result to `stderr`, followed by a newline
 * character.
 *
 * \headerfile NXLog.h NXFoundation/NXFoundation.h
 */
#define NXLog(format, ...)                                                     \
  do {                                                                         \
    sys_printf((const char *)[format cStr], ##__VA_ARGS__);                    \
    sys_puts("\n");                                                            \
  } while (0)
