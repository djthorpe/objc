/**
 * @file NXNumberUnsignedInt64.h
 * @brief Defines the NXNumberUnsignedInt64 class, which represents 64-bit
 * unsigned integer values.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief A class for managing and manipulating unsigned 64-bit integer values.
 */
@interface NXNumberUnsignedInt64 : NXNumber {
@private
  uint64_t
      _value; ///< The 64-bit unsigned integer value stored in this instance.
}

+ (NXNumber *)numberWithUnsignedInt64:(uint64_t)value;

/**
 * @brief Return an instance of a zero value.
 * @return A statically allocated NXNumberUnsignedInt64 instance representing
 * zero.
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
