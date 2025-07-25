#include "NXNumberInt32.h"
#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberInt32

/**
 * @brief Initialize an instance with a int32_t value.
 */
- (id)initWithInt32:(int32_t)value {
  self = [super init];
  if (self) {
    _value = value;
  }
  return self;
}

/**
 * @brief Return an instance with a int32_t value.
 */
+ (NXNumber *)numberWithInt32:(int32_t)value {
  if (value == 0) {
    return [NXNumber zeroValue];
  }
  return [[[NXNumberInt32 alloc] initWithInt32:value] autorelease];
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
  objc_assert(_value >= INT16_MIN && _value <= INT16_MAX);
  return (int16_t)_value;
}

/**
 * @brief Get the stored value as a uint16_t.
 */
- (uint16_t)unsignedInt16Value {
  objc_assert(_value >= 0 && _value <= UINT16_MAX);
  return (uint16_t)_value;
}

/**
 * @brief Get the stored value as a int32_t.
 */
- (int32_t)int32Value {
  return _value;
}

/**
 * @brief Get the stored value as a uint32_t.
 */
- (uint32_t)unsignedInt32Value {
  objc_assert(_value >= 0);
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
  objc_assert(_value >= 0);
  // Ensure the value is non-negative before casting to uint64_t
  return (uint64_t)_value;
}

/**
 * @brief Return the string representation of the value.
 */
- (NXString *)description {
  return [NXString stringWithFormat:@"%d", _value];
}

/**
 * @brief Returns the appropriate capacity for the JSON representation.
 */
- (size_t)JSONBytes {
  // Format: "-2147483648"
  return 11;
}

/**
 * @brief Check for equality with another NXNumber instance.
 */
- (BOOL)isEqual:(id)object {
  if (self == object) {
    return YES;
  }

  // Check if it's another NXNumberInt32
  if ([object isKindOfClass:[NXNumberInt32 class]]) {
    NXNumberInt32 *other = (NXNumberInt32 *)object;
    return _value == other->_value;
  }

  // Check if it's any other NXNumber subclass
  if ([object isKindOfClass:[NXNumber class]]) {
    NXNumber *other = (NXNumber *)object;

    // If this value is non-negative, compare with unsigned value
    if (_value >= 0) {
      uint64_t otherUint64 = [other unsignedInt64Value];
      return (uint64_t)_value == otherUint64;
    } else {
      // If this value is negative, compare with signed value
      int64_t otherInt64 = [other int64Value];
      return (int64_t)_value == otherInt64;
    }
  }

  return NO;
}

@end
