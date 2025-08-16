#include "NXNumberBool.h"
#include <Foundation/Foundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Re-usable static instances of NXNumberBool for true, false
static NXNumberBool *trueNumber;
static NXNumberBool *falseNumber;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberBool

/**
 * @brief Initialize an instance with a boolean value.
 */
- (id)initWithBool:(BOOL)value {
  self = [super init];
  if (self) {
    _value = value;
  }
  return self;
}

/**
 * @brief Return a true instance
 */
+ (NXNumber *)trueValue {
  @synchronized(self) {
    if (trueNumber == nil) {
      objc_assert([NXAutoreleasePool currentPool]);
      trueNumber = [[[NXNumberBool alloc] initWithBool:YES] autorelease];
    }
  }
  return trueNumber;
}

/**
 * @brief Return a false instance
 */
+ (NXNumber *)falseValue {
  @synchronized(self) {
    if (falseNumber == nil) {
      objc_assert([NXAutoreleasePool currentPool]);
      falseNumber = [[[NXNumberBool alloc] initWithBool:NO] autorelease];
    }
  }
  return falseNumber;
}

/**
 * @brief Get the stored value as a boolean.
 */
- (BOOL)boolValue {
  return _value;
}

/**
 * @brief Get the stored value as a int16_t.
 */
- (int16_t)int16Value {
  return _value ? 1 : 0;
}

/**
 * @brief Get the stored value as a uint16_t.
 */
- (uint16_t)unsignedInt16Value {
  return _value ? 1 : 0;
}

/**
 * @brief Get the stored value as a int32_t.
 */
- (int32_t)int32Value {
  return _value ? 1 : 0;
}

/**
 * @brief Get the stored value as a uint32_t.
 */
- (uint32_t)unsignedInt32Value {
  return _value ? 1 : 0;
}

/**
 * @brief Get the stored value as a int64_t.
 */
- (int64_t)int64Value {
  return _value ? 1 : 0;
}

/**
 * @brief Get the stored value as a uint64_t.
 */
- (uint64_t)unsignedInt64Value {
  return _value ? 1 : 0;
}

/**
 * @brief Return the string representation of the boolean value.
 */
- (NXString *)description {
  return [NXString stringWithCString:(_value ? "true" : "false")];
}

/**
 * @brief Returns the appropriate capacity for the JSON representation.
 */
- (size_t)JSONBytes {
  return _value ? 4 : 5;
}

/**
 * @brief Check for equality with another NXNumber instance.
 */
- (BOOL)isEqual:(id)object {
  if (self == object) {
    return YES;
  }

  // Check if it's another NXNumberBool
  if ([object isKindOfClass:[NXNumberBool class]]) {
    NXNumberBool *other = (NXNumberBool *)object;
    return _value == other->_value;
  }

  // Check if it's any other NXNumber subclass
  if ([object isKindOfClass:[NXNumber class]]) {
    NXNumber *other = (NXNumber *)object;

    // Compare boolean values - true equals 1, false equals 0
    if (_value) {
      // This is true, check if other equals 1
      int64_t otherInt64 = [other int64Value];
      return (otherInt64 == 1);
    } else {
      // This is false, check if other equals 0
      int64_t otherInt64 = [other int64Value];
      return (otherInt64 == 0);
    }
  }

  return NO;
}

@end
