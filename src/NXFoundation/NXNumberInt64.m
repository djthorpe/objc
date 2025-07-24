#include "NXNumberInt64.h"
#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Re-usable static instances of NXNumberInt64 for zero
static NXNumberInt64 *zeroNumber;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberInt64

/**
 * @brief Initialize an instance with a int64_t value.
 */
- (id)initWithInt64:(int64_t)value {
  self = [super init];
  if (self) {
    _value = value;
  }
  return self;
}

/**
 * @brief Return an instance with a int64_t value.
 */
+ (NXNumber *)numberWithInt64:(int64_t)value {
  if (value == 0) {
    return [NXNumberInt64 zeroValue];
  }
  return [[[NXNumberInt64 alloc] initWithInt64:value] autorelease];
}

/**
 * @brief Return a zero instance
 */
+ (NXNumber *)zeroValue {
  @synchronized(self) {
    if (zeroNumber == nil) {
      objc_assert([NXAutoreleasePool currentPool]);
      zeroNumber = [[[NXNumberInt64 alloc] initWithInt64:0] autorelease];
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
  return _value;
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
  return [NXString stringWithFormat:@"%lld", _value];
}

/**
 * @brief Check for equality with another NXNumber instance.
 */
- (BOOL)isEqual:(id)object {
  if (![object isKindOfClass:[NXNumberInt64 class]]) {
    return NO;
  }
  NXNumberInt64 *other = (NXNumberInt64 *)object;
  return _value == other->_value;
}

@end
