/**
 * @file NXLog.h
 * @brief Defines a logging macro for printing formatted messages.
 * @details This file provides the `NXLog` macro, which is a convenient way to
 * print formatted output to the standard error stream. It is similar to `NSLog`
 * from the Foundation framework.
 */
#pragma once

/**
 * @def NXLog(format, ...)
 * @brief Logs a formatted string to the console.
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
    objc_printf((const char *)[format cStr], ##__VA_ARGS__);                   \
    objc_printf("\n");                                                         \
  } while (0)
