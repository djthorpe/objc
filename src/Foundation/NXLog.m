#include "NXString+format.h"
#include <Foundation/Foundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

#define NXLOG_BUFFER_SIZE 80

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief logging function that formats a string with objects and time
 * intervals. Appends a newline and flushes the output.
 */
size_t NXLog(id<NXConstantStringProtocol> format, ...) {
  static char buf[NXLOG_BUFFER_SIZE];
  static int depth = 0;
  if (++depth > 1) {
    depth--; // ensure we don't permanently block future logs
    return 0;
  }

  va_list args;
  va_start(args, format);
  const char *cFormat = [format cStr];
  // First attempt: format directly into the static buffer. The returned len
  // is the total required length (untruncated). If it fits, we already have
  // the full string and can print once.
  size_t len = sys_vsprintf_ex(buf, NXLOG_BUFFER_SIZE, cFormat, args,
                               _nxstring_format_handler);
  if (len < NXLOG_BUFFER_SIZE) {
    sys_puts(buf);
  } else {
    // Truncated: reformat using a heap buffer. Copy the va_list first.
    va_list args_copy;
    va_copy(args_copy, args);
    char *buf = (char *)sys_malloc(len + 1);
    if (buf) {
      sys_vsprintf_ex(buf, len + 1, cFormat, args_copy,
                      _nxstring_format_handler);
      sys_puts(buf);
      sys_free(buf);
    } else {
      // OOM: print the truncated static buffer
      sys_puts(buf);
    }
    va_end(args_copy);
  }
  va_end(args);
  depth--;
  return len;
}
