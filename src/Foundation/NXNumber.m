#include "NXNumberBool.h"
#include "NXNumberInt16.h"
#include "NXNumberInt32.h"
#include "NXNumberInt64.h"
#include "NXNumberUnsignedInt16.h"
#include "NXNumberUnsignedInt32.h"
#include "NXNumberUnsignedInt64.h"
#include "NXNumberZero.h"
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Generate a random 32-bit signed integer.
 */
int32_t NXRandInt32() { return (int32_t)sys_random_uint32(); }

/**
 * @brief Generate a random 32-bit unsigned integer.
 */
uint32_t NXRandUnsignedInt32() { return sys_random_uint32(); }

///////////////////////////////////////////////////////////////////////////////
// NXNumber

@implementation NXNumber

/**
 * @brief Create an NXNumber object with a boolean value.
 */
+ (NXNumber *)numberWithBool:(BOOL)value {
  return value ? [NXNumberBool trueValue] : [NXNumberBool falseValue];
}

/**
 * @brief Create an NXNumber object with a 16-bit signed integer value.
 */
+ (NXNumber *)numberWithInt16:(int16_t)value {
  return [NXNumberInt16 numberWithInt16:value];
}

/**
 * @brief Create an NXNumber object with a 16-bit unsigned integer value.
 */
+ (NXNumber *)numberWithUnsignedInt16:(uint16_t)value {
  return [NXNumberUnsignedInt16 numberWithUnsignedInt16:value];
}

/**
 * @brief Create an NXNumber object with a 32-bit signed integer value.
 */
+ (NXNumber *)numberWithInt32:(int32_t)value {
  return [NXNumberInt32 numberWithInt32:value];
}

/**
 * @brief Create an NXNumber object with a 32-bit unsigned integer value.
 */
+ (NXNumber *)numberWithUnsignedInt32:(uint32_t)value {
  return [NXNumberUnsignedInt32 numberWithUnsignedInt32:value];
}

/**
 * @brief Create an NXNumber object with a 64-bit signed integer value.
 */
+ (NXNumber *)numberWithInt64:(int64_t)value {
  return [NXNumberInt64 numberWithInt64:value];
}

/**
 * @brief Create an NXNumber object with a 64-bit unsigned integer value.
 */
+ (NXNumber *)numberWithUnsignedInt64:(uint64_t)value {
  return [NXNumberUnsignedInt64 numberWithUnsignedInt64:value];
}

/**
 * @brief Get a singleton NXNumber representing the boolean value true.
 */
+ (NXNumber *)trueValue {
  return [NXNumberBool trueValue];
}

/**
 * @brief Get a singleton NXNumber representing the boolean value false.
 */
+ (NXNumber *)falseValue {
  return [NXNumberBool falseValue];
}

/**
 * @brief Get a singleton NXNumber representing a zero value.
 */
+ (NXNumber *)zeroValue {
  return [NXNumberZero zeroValue];
}

/**
 * @brief Get the stored value as a boolean.
 * @note Default implementation returns NO.
 */
- (BOOL)boolValue {
  return NO;
}

/**
 * @brief Get the stored value as a 16-bit signed integer.
 * @note Default implementation returns 0 as it's always subclassed.
 */
- (int16_t)int16Value {
  return 0;
}

/**
 * @brief Get the stored value as a 16-bit unsigned integer.
 * @note Default implementation returns 0 as it's always subclassed.
 */
- (uint16_t)unsignedInt16Value {
  return 0;
}

/**
 * @brief Get the stored value as a 32-bit signed integer.
 * @note Default implementation returns 0 as it's always subclassed.
 */
- (int32_t)int32Value {
  return 0;
}

/**
 * @brief Get the stored value as a 32-bit unsigned integer.
 * @note Default implementation returns 0 as it's always subclassed.
 */
- (uint32_t)unsignedInt32Value {
  return 0;
}

/**
 * @brief Get the stored value as a 64-bit signed integer.
 * @note Default implementation returns 0 as it's always subclassed.
 */
- (int64_t)int64Value {
  return 0;
}

/**
 * @brief Get the stored value as a 64-bit unsigned integer.
 * @note Default implementation returns 0 as it's always subclassed.
 */
- (uint64_t)unsignedInt64Value {
  return 0;
}

/**
 * @brief Return the JSON string representation of a number value.
 */
- (NXString *)JSONString {
  return [self description];
}

/**
 * @brief Returns the appropriate capacity for the JSON representation.
 * @note Default implementation returns 0 as it's always subclassed.
 */
- (size_t)JSONBytes {
  return 0;
}

@end
