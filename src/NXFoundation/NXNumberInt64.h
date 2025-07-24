/**
 * @file NXNumberInt64.h
 * @brief Defines the NXNumberInt64 class, which represents 64-bit integer
 * values.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief A class for managing and manipulating 64-bit integer values.
 */
@interface NXNumberInt64 : NXNumber {
@private
  int64_t _value; ///< The 64-bit integer value stored in this instance.
}

+ (NXNumber *)numberWithInt64:(int64_t)value;

/**
 * @brief Return an instance of an true value.
 * @return A statically allocated NXNumberInt64 instance representing zero.
 */
+ (NXNumber *)zeroValue;

/**
 * @brief Get the stored value as a boolean.
 * @return The boolean value stored in this instance.
 */
- (BOOL)boolValue;

/**
 * @brief Get the stored value as a int64_t.
 * @return The int64_t value stored in this instance.
 */
- (int64_t)int64Value;

/**
 * @brief Returns the 64-bit unsigned integer value stored in this NXNumber
 * instance.
 * @return The 64-bit unsigned integer value stored in this instance.
 */
- (uint64_t)unsignedInt64Value;

@end
