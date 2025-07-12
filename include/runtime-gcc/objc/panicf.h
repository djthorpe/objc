/**
 * @file panicf.h
 * @brief Defines the panic function for handling fatal errors.
 * @details This file declares the `panicf` function, which is used to report
 * fatal errors and terminate the program.
 */
#pragma once

/**
 * @brief Reports a fatal error and terminates the program.
 * @param fmt A printf-style format string.
 * @param ... Additional arguments for the format string.
 * @details This function prints a formatted error message to the standard error
 * stream and then aborts the program. It is used for unrecoverable errors.
 */
void panicf(const char *fmt, ...);
