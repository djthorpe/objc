#include "NXNumberUnsignedInt64.h"
#include <Foundation/Foundation.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberUnsignedInt64

/**
 * @brief Initialize an instance with a uint64_t value.
 */
- (id)initWithUnsignedInt64:(uint64_t)value {
  self = [super init];
  if (self) {
    _value = value;
  }
  return self;
}

/**
 * @brief Return an instance with a uint64_t value.
 */
+ (NXNumber *)numberWithUnsignedInt64:(uint64_t)value {
  if (value == 0) {
    return [NXNumber zeroValue];
  }
  return
      [[[NXNumberUnsignedInt64 alloc] initWithUnsignedInt64:value] autorelease];
}

/**
 * @brief Get the stored value as a boolean.
 */
- (BOOL)boolValue {
  return _value ? YES : NO;
}

/**
 * @brief Get the stored value as a int16_t.
 */
- (int16_t)int16Value {
  objc_assert(_value <= INT16_MAX);
  return (int16_t)_value;
}

/**
 * @brief Get the stored value as a uint16_t.
 */
- (uint16_t)unsignedInt16Value {
  objc_assert(_value <= UINT16_MAX);
  return (uint16_t)_value;
}

/**
 * @brief Get the stored value as a int32_t.
 */
- (int32_t)int32Value {
  objc_assert(_value <= INT32_MAX);
  return (int32_t)_value;
}

/**
 * @brief Get the stored value as a uint32_t.
 */
- (uint32_t)unsignedInt32Value {
  objc_assert(_value <= UINT32_MAX);
  return (uint32_t)_value;
}

/**
 * @brief Returns the 64-bit integer value.
 */
- (int64_t)int64Value {
  return (int64_t)_value;
}

/**
 * @brief Returns the 64-bit unsigned integer value.
 */
- (uint64_t)unsignedInt64Value {
  return _value;
}

/**
 * @brief Return the string representation of the value.
 */
- (NXString *)description {
  return [NXString stringWithFormat:@"%llu", _value];
}

/**
 * @brief Returns the appropriate capacity for the JSON representation.
 */
- (size_t)JSONBytes {
  // Format: "18446744073709551615"
  return 20; // 20 characters for the maximum value of uint64_t
}

/**
 * @brief Check for equality with another NXNumber instance.
 */
- (BOOL)isEqual:(id)object {
  if (self == object) {
    return YES;
  }

  // Check if it's another NXNumberUnsignedInt64
  if ([object isKindOfClass:[NXNumberUnsignedInt64 class]]) {
    NXNumberUnsignedInt64 *other = (NXNumberUnsignedInt64 *)object;
    return _value == other->_value;
  }

  // Check if it's any other NXNumber subclass
  if ([object isKindOfClass:[NXNumber class]]) {
    NXNumber *other = (NXNumber *)object;
    int64_t otherInt64 = [other int64Value];

    // Only compare if the other number is non-negative
    if (otherInt64 >= 0) {
      return _value == (uint64_t)otherInt64;
    }
  }

  return NO;
}

@end
