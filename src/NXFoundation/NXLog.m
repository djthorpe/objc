#include "NXFoundation+format.h"
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// SHARED FORMAT HANDLER

// Shared custom handler for NXFoundation that supports %@ for objects and %t
// for time intervals
const char *_nxfoundation_format_handler(char format, va_list *va) {
  if (format == '@') {
    id obj = va_arg(*va, id);
    if (obj == nil) {
      return "<nil>";
    }

    // Check if the object directly conforms to NXConstantStringProtocol
    if ([obj conformsTo:@protocol(NXConstantStringProtocol)]) {
      const char *result = [obj cStr];
      return result;
    }

    // Otherwise, call the object's description method and return the C string
    id desc = [obj description];
    if ([desc conformsTo:@protocol(NXConstantStringProtocol)]) {
      const char *result = [desc cStr];
      return result;
    }
  } else if (format == 't') {
    // Handle NXTimeInterval formatting with %t
    NXTimeInterval interval = va_arg(*va, NXTimeInterval);
    NXString *desc = NXTimeIntervalDescription(interval, Millisecond);
    if (desc != nil && [desc conformsTo:@protocol(NXConstantStringProtocol)]) {
      return [desc cStr];
    }
    return "<time>";
  }

  // Not handled by this custom handler or other error
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

size_t NXLog(id<NXConstantStringProtocol> format, ...) {
  va_list args;

  // Use shared custom handler for %@ and %t
  va_start(args, format);
  size_t result =
      sys_vprintf_ex([format cStr], args, _nxfoundation_format_handler);
  va_end(args);

  // Add newline like NSLog (but don't count it in the result)
  // and flush the output
  sys_putch('\n');
  sys_puts(NULL);

  return result;
}
