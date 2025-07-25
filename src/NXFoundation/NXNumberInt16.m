#include "NXNumberInt16.h"
#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberInt16

/**
 * @brief Initialize an instance with a int16_t value.
 */
- (id)initWithInt16:(int16_t)value {
  self = [super init];
  if (self) {
    _value = value;
  }
  return self;
}

/**
 * @brief Return an instance with a int16_t value.
 */
+ (NXNumber *)numberWithInt16:(int16_t)value {
  if (value == 0) {
    return [NXNumber zeroValue];
  }
  return [[[NXNumberInt16 alloc] initWithInt16:value] autorelease];
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
  return _value;
}

/**
 * @brief Get the stored value as a int32_t.
 */
- (int32_t)int32Value {
  return (int32_t)_value;
}

/**
 * @brief Get the stored value as a int64_t.
 */
- (int64_t)int64Value {
  return (int64_t)_value;
}

/**
 * @brief Get the stored value as a uint16_t.
 */
- (uint16_t)unsignedInt16Value {
  objc_assert(_value >= 0);
  return (uint16_t)_value;
}

/**
 * @brief Get the stored value as a uint32_t.
 */
- (uint32_t)unsignedInt32Value {
  objc_assert(_value >= 0);
  return (uint32_t)_value;
}

/**
 * @brief Get the stored value as a uint64_t.
 */
- (uint64_t)unsignedInt64Value {
  objc_assert(_value >= 0);
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
  return 6; // "-32768"
}

/**
 * @brief Check for equality with another NXNumber instance.
 */
- (BOOL)isEqual:(id)object {
  if (self == object) {
    return YES;
  }

  // Check if it's another NXNumberInt16
  if ([object isKindOfClass:[NXNumberInt16 class]]) {
    NXNumberInt16 *other = (NXNumberInt16 *)object;
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
