#include "NXString+format.h"
#include <Foundation/Foundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// SHARED FORMAT HANDLER

/**
 * @brief Custom handler for Foundation that supports %@ for objects
 */
const char *_nxstring_format_description_handler(id obj) {
  if (obj == nil) {
    return "<nil>";
  }
  // Fast path: if it's already a string/constant string, avoid description.
  if ([obj conformsTo:@protocol(NXConstantStringProtocol)]) {
    return [obj cStr];
  }
  NXString *desc = [obj description];
  if (desc) {
    return [desc cStr];
  }
  return NULL;
}

// Non-allocating identity formatter: [ClassName @0xPTR]
static inline const char *_nxstring_format_identity_handler(id obj) {
  if (obj == nil) {
    return "<nil>";
  }
  // Thread-local scratch buffer to avoid heap alloc
#if defined(__APPLE__) || defined(__linux__)
  static __thread char buf[64];
#else
  static char buf[64];
#endif
  const char *cls = object_getClassName(obj);
  if (!cls) {
    cls = "?";
  }
  // Format into the local buffer with sys_sprintf (no global printf lock)
  sys_sprintf(buf, sizeof(buf), "[%s @%p]", cls, obj);
  return buf;
}

/**
 * @brief Custom handler for Foundation that supports %t for NXTimeInterval
 */
const char *_nxstring_format_time_interval_handler(NXTimeInterval interval) {
  (void)interval; // Suppress unused parameter warning

  // TODO: Implement time interval formatting

  // Format the time interval as needed
  return "<t>";
}

/**
 * @brief Custom handler for Foundation that supports %q for JSON
 */
const char *_nxstring_format_json_handler(id obj) {
  if (obj == nil) {
    return "null";
  }
  if ([obj conformsTo:@protocol(JSONProtocol)]) {
    NXString *jStr = [obj JSONString];
    if (jStr) {
      return [jStr cStr];
    }
  }
  return NULL;
}

/**
 * @brief Shared custom handler for Foundation that supports %@ for objects
 * and %t for time intervals
 */
const char *_nxstring_format_handler(char format, va_list *va) {
  switch (format) {
  case '@': {
    id obj = va_arg(*va, id);
    return _nxstring_format_description_handler(obj);
  }
  case 'O': {
    id obj = va_arg(*va, id);
    return _nxstring_format_identity_handler(obj);
  }
  case 'q': {
    id obj = va_arg(*va, id);
    return _nxstring_format_json_handler(obj);
  }
  case 't': {
    NXTimeInterval interval = va_arg(*va, NXTimeInterval);
    return _nxstring_format_time_interval_handler(interval);
  }
  }
  return NULL; // Not handled by this custom handler
}

/*

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
    return [desc cStr];
  } else if (format == 'q') {
    id obj = va_arg(*va, id);
    if (obj == nil) {
      return "null";
    }
    return "json";
  }

  // Not handled by this custom handler or other error
  return NULL;
}
*/
