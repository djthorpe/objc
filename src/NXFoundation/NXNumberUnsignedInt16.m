#include "NXNumberUnsignedInt16.h"
#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberUnsignedInt16

/**
 * @brief Initialize an instance with a uint16_t value.
 */
- (id)initWithUnsignedInt16:(uint16_t)value {
  self = [super init];
  if (self) {
    _value = value;
  }
  return self;
}

/**
 * @brief Return an instance with a uint16_t value.
 */
+ (NXNumber *)numberWithUnsignedInt16:(uint16_t)value {
  if (value == 0) {
    return [NXNumber zeroValue];
  }
  return
      [[[NXNumberUnsignedInt16 alloc] initWithUnsignedInt16:value] autorelease];
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
  return _value;
}

/**
 * @brief Get the stored value as a uint32_t.
 */
- (uint32_t)unsignedInt32Value {
  return (uint32_t)_value;
}

/**
 * @brief Get the stored value as a uint64_t.
 */
- (uint64_t)unsignedInt64Value {
  return (uint64_t)_value;
}

/**
 * @brief Return the string representation of the value.
 */
- (NXString *)description {
  return [NXString stringWithFormat:@"%u", _value];
}

/**
 * @brief Check for equality with another NXNumber instance.
 */
- (BOOL)isEqual:(id)object {
  if (self == object) {
    return YES;
  }

  // Check if it's another NXNumberUnsignedInt16
  if ([object isKindOfClass:[NXNumberUnsignedInt16 class]]) {
    NXNumberUnsignedInt16 *other = (NXNumberUnsignedInt16 *)object;
    return _value == other->_value;
  }

  // Check if it's any other NXNumber subclass
  if ([object isKindOfClass:[NXNumber class]]) {
    NXNumber *other = (NXNumber *)object;

    // Since this is unsigned, we can safely compare with unsigned values
    // Check if the other object represents a negative signed integer
    if ([other int64Value] < 0) {
      return NO;
    }
    uint64_t otherUint64 = [other unsignedInt64Value];
    return (uint64_t)_value == otherUint64;
  }

  return NO;
}

@end
