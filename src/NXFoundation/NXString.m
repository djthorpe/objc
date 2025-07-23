#include "NXZone+malloc.h"
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>
#include <string.h>

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
 * @brief Return a new empty string.
 */
+ (NXString *)new {
  objc_assert([NXAutoreleasePool currentPool]);
  return [[[NXString alloc] init] autorelease];
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
 * @brief Return a string by referencing another string.
 */
+ (NXString *)stringWithString:(id<NXStringProtocol, ObjectProtocol>)other {
  objc_assert([NXAutoreleasePool currentPool]);
  return [[[NXString alloc] initWithString:other] autorelease];
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
 * @brief Return a string by referencing a c-string.
 */
+ (NXString *)stringWithCString:(const char *)cStr {
  objc_assert([NXAutoreleasePool currentPool]);
  return [[[NXString alloc] initWithCString:cStr] autorelease];
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
 * @brief Return a string instance with a format string and arguments.
 */
+ (NXString *)stringWithFormat:(NXConstantString *)format, ... {
  objc_assert([NXAutoreleasePool currentPool]);
  NXString *instance = [NXString alloc];
  if (!instance) {
    return nil;
  }

  // Use a variable argument list to handle the format string
  va_list args;
  va_start(args, format);

  // Get the length of the formatted string
  const char *cFormat = [format cStr];
  instance->_length = sys_vsprintf(NULL, 0, cFormat, args);
  if (instance->_length > 0) {
    // Allocate memory for the string
    instance->_data = [_zone allocWithSize:instance->_length + 1];
    if (instance->_data) {
      sys_vsprintf(instance->_data, instance->_length + 1, cFormat,
                   args);       // Format the string into the allocated memory
      instance->_value = _data; // Set the value to the allocated data
    } else {
      [instance release];
      instance = nil; // Allocation failed, set self to nil
    }
  }

  va_end(args);
  return [instance autorelease]; // Return an autoreleased instance
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

/**
 * @brief Returns the string through a description method.
 */
- (NXString *)description {
  return self;
}

/**
 * @brief Checks if the string is equal to another object.
 */
- (BOOL)isEqual:(id)other {
  if (self == other) {
    return YES;
  }
  if (other == nil ||
      [other conformsToProtocol:@protocol(NXStringProtocol)] == NO) {
    return NO;
  }
  if ([other length] != _length) {
    return NO;
  }
  const char *otherCStr = [other cStr];
  if (otherCStr == _value) {
    return YES; // Same pointer, same content
  }
  return strcmp(_value ? _value : "", otherCStr ? otherCStr : "") == 0;
}

/**
 * @brief Compares this string with another string.
 */
- (NXComparisonResult)compare:(id<NXStringProtocol>)other {
  objc_assert(other);
  const char *otherCStr = [other cStr];
  if (otherCStr == _value) {
    return NXComparisonSame; // Same pointer, same content
  }
  return strcmp(_value ? _value : "", otherCStr ? otherCStr : "");
}

/**
 * @brief Counts the number of occurrences of a character.
 */
- (uint32_t)countOccurrencesOfByte:(const char)ch {
  uint32_t count = 0;
  for (unsigned int i = 0; i < _length; i++) {
    if (_value[i] == ch) {
      count++;
    }
  }
  return count;
}

@end
