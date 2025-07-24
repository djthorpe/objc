#include "NXNumberUnsignedInt64.h"
#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Re-usable static instances of NXNumberUnsignedInt64 for zero
static NXNumberUnsignedInt64 *zeroNumber;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberUnsignedInt64

/**
 * @brief Initialize an instance with a int64_t value.
 */
- (id)initWithUnsignedInt64:(int64_t)value {
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
    return [NXNumberUnsignedInt64 zeroValue];
  }
  return
      [[[NXNumberUnsignedInt64 alloc] initWithUnsignedInt64:value] autorelease];
}

/**
 * @brief Return a zero instance
 */
+ (NXNumber *)zeroValue {
  @synchronized(self) {
    if (zeroNumber == nil) {
      objc_assert([NXAutoreleasePool currentPool]);
      zeroNumber =
          [[[NXNumberUnsignedInt64 alloc] initWithUnsignedInt64:0] autorelease];
    }
  }
  return zeroNumber;
}

/**
 * @brief Get the stored value as a boolean.
 */
- (BOOL)boolValue {
  return _value ? YES : NO;
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
 * @brief Check for equality with another NXNumber instance.
 */
- (BOOL)isEqual:(id)object {
  if (![object isKindOfClass:[NXNumberUnsignedInt64 class]]) {
    return NO;
  }
  // TODO: Check against int64_t
  NXNumberUnsignedInt64 *other = (NXNumberUnsignedInt64 *)object;
  return _value == other->_value;
}

@end
