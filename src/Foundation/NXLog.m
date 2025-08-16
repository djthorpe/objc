#include "NXString+format.h"
#include <Foundation/Foundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief logging function that formats a string with objects and time
 * intervals. Appends a newline and flushes the output.
 */
size_t NXLog(id<NXConstantStringProtocol> format, ...) {
  va_list args;

  // Use shared custom handler for %@ and %t
  va_start(args, format);
  size_t result = sys_vprintf_ex([format cStr], args, _nxstring_format_handler);
  va_end(args);

  // Add newline like NSLog (but don't count it in the result)
  // and flush the output
  sys_putch('\n');
  sys_puts(NULL);

  return result;
}
