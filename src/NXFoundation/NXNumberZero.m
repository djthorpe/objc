#include "NXNumberZero.h"
#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Re-usable static instance of NXNumberZero
static NXNumberZero *zeroNumber;

///////////////////////////////////////////////////////////////////////////////
// NXNumberZero

@implementation NXNumberZero

/**
 * @brief Get a singleton NXNumberZero representing the value zero.
 */
+ (NXNumber *)zeroValue {
  @synchronized(self) {
    if (zeroNumber == nil) {
      objc_assert([NXAutoreleasePool currentPool]);
      zeroNumber = [[[NXNumberZero alloc] init] autorelease];
    }
  }
  return zeroNumber;
}

/**
 * @brief Get the stored value as a boolean.
 */
- (BOOL)boolValue {
  return NO;
}

/**
 * @brief Get the stored value as a 16-bit signed integer.
 */
- (int16_t)int16Value {
  return 0;
}

/**
 * @brief Get the stored value as a 16-bit unsigned integer.
 */
- (uint16_t)unsignedInt16Value {
  return 0;
}

/**
 * @brief Get the stored value as a 32-bit signed integer.
 */
- (int32_t)int32Value {
  return 0;
}

/**
 * @brief Get the stored value as a 32-bit unsigned integer.
 */
- (uint32_t)unsignedInt32Value {
  return 0;
}

/**
 * @brief Get the stored value as a 64-bit signed integer.
 */
- (int64_t)int64Value {
  return 0;
}

/**
 * @brief Get the stored value as a 64-bit unsigned integer.
 */
- (uint64_t)unsignedInt64Value {
  return 0;
}

/**
 * @brief Return the string representation of the value.
 */
- (NXString *)description {
  return [NXString stringWithCString:"0"];
}

/**
 * @brief Returns the appropriate capacity for the JSON representation.
 */
- (size_t)JSONBytes {
  return 1;
}

/**
 * @brief Check if this number is equal to another object.
 */
- (BOOL)isEqual:(id)object {
  // Fast path: same instance
  if (self == object) {
    return YES;
  }

  // Check if it's another NXNumberZero instance
  if ([object isKindOfClass:[NXNumberZero class]]) {
    return YES;
  }

  // Check if it's any other NXNumber with zero value
  if ([object isKindOfClass:[NXNumber class]]) {
    NXNumber *other = (NXNumber *)object;

    // Try signed comparison first (works for all values)
    int64_t otherInt64 = [other int64Value];
    return otherInt64 == 0;
  }

  return NO;
}

@end
