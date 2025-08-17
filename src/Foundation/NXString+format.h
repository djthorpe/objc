#pragma once
#include <runtime-sys/sys.h>
#include <stdarg.h>

/**
 * @brief Shared custom format handler for Foundation that supports %@ for
 * objects, %t for time intervals and %q for JSON strings.
 * @param format The format specifier character (e.g., '@' for '%@', 't' for
 * '%t').
 * @param va Pointer to the va_list containing the arguments.
 * @return A string representation of the formatted value, or NULL if the
 *         format specifier is not handled by this custom handler.
 *
 * This handler supports:
 * - %@ for object descriptions (calls [object description])
 * - %t for NXTimeInterval formatting
 * - %q for JSON string formatting (calls [object JSONString])
 * - %O prints a non-allocating object identity of the form
 * "[ClassName @0xPTR]" without calling -description.
 */
extern const char *_nxstring_format_handler(char format, va_list *va);
