#include "NXZone+malloc.h"
#include <NXFoundation/NXFoundation.h>
#include <string.h>
#include <sys/sys.h>

@implementation NXString

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes a new empty string
 */
- (id)init {
  self = [super init];
  if (self) {
    _value = NULL;
    _data = NULL;
    _length = 0;
  }
  return self;
}

/**
 * @brief Initializes a new string by referencing another string.
 */
- (id)initWithString:(id<NXStringProtocol, ObjectProtocol>)other {
  self = [self init];
  if (self == nil || self == other || other == nil) {
    return nil;
  }
  if ([other class] == [NXConstantString class]) {
    _value = [other cStr];
    _length = [other length];
  } else if ([other isKindOfClass:[NXString class]]) {
    _value = [other cStr];
    _length = [other length];
  } else {
    [self release];
    return nil;
  }
  return self;
}

/**
 * @brief Initialize a new string by referencing a c-string.
 */
- (id)initWithCString:(const char *)cStr {
  self = [self init];
  if (self) {
    _value = cStr;
    _length = (unsigned int)strlen(cStr);
  }
  return self;
}

/**
 * @brief Initialize a new string with a format string and arguments.
 */
- (id)initWithFormat:(NXConstantString *)format, ... {
  self = [super init];
  if (!self) {
    return self;
  }

  // Use a variable argument list to handle the format string
  va_list args;
  va_start(args, format);

  // Get the length of the formatted string
  const char *cFormat = [format cStr];
  _length = sys_vsprintf(NULL, 0, cFormat, args);
  if (_length > 0) {
    // Allocate memory for the string
    _data = [_zone allocWithSize:_length + 1];
    if (_data) {
      sys_vsprintf(_data, _length + 1, cFormat,
                   args); // Format the string into the allocated memory
      _value = _data;     // Set the value to the allocated data
    } else {
      [self release];
      self = nil; // Allocation failed, set self to nil
    }
  }

  va_end(args);
  return self;
}

/**
 * @brief Releases the string's internal value.
 */
- (void)dealloc {
  if (_data) {
    [_zone free:_data]; // Free the allocated data
  }
  [super dealloc];
}

///////////////////////////////////////////////////////////////////////////////
// INSTANCE METHODS

/**
 * @brief Returns the C-string representation of the string.
 */
- (const char *)cStr {
  return _value ? _value : "";
}

/**
 * @brief Returns the length of the string.
 */
- (unsigned int)length {
  if (_value) {
    return _length;
  }
  return 0;
}

@end
