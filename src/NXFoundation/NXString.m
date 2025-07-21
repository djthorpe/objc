#include "NXZone+malloc.h"
#include <NXFoundation/NXFoundation.h>
#include <string.h>

@implementation NXString

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes a new string by referencing another string.
 */
- (id)initWithString:(id)other {
  self = [super init];
  if (self == nil || self == other) {
    return nil;
  }
  _value = NULL;
  _length = 0;
  if (other != nil) {
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
  }
  return self;
}

/**
 * @brief Initialize a new string by referencing a c-string.
 */
- (id)initWithCString:(const char *)cStr {
  self = [super init];
  if (self == nil) {
    return nil;
  }
  _value = cStr;
  _length = (unsigned int)strlen(cStr);
  return self;
}

/**
 * @brief Releases the string's internal value.
 */
- (void)dealloc {
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
