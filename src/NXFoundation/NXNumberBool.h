/**
 * @file NXNumberBool.h
 * @brief Defines the NXNumberBool class, which represents boolean values.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief A class for managing and manipulating boolean values.
 */
@interface NXNumberBool : NXNumber {
@private
  BOOL _value; ///< The boolean value stored in this instance.
}

/**
 * @brief Initialize an instance with a boolean value.
 * @param value The boolean value to store.
 * @return An initialized NXNumberBool instance.
 */
- (id)initWithBool:(BOOL)value;

/**
 * @brief Return an instance of an true value.
 * @return A statically allocated NXNumberBool instance representing true.
 */
+ (NXNumber *)trueValue;

/**
 * @brief Return an instance of a false value.
 * @return A statically allocated NXNumberBool instance representing false.
 */
+ (NXNumber *)falseValue;

/**
 * @brief Get the stored value as a boolean.
 * @return The boolean value stored in this instance.
 */
- (BOOL)boolValue;

/**
 * @brief Get the stored value as a int64_t.
 * @return The int64_t value stored in this instance. If the value is true, it
 * returns 1; if false, it returns 0.
 */
- (int64_t)int64Value;

/**
 * @brief Returns the 64-bit unsigned integer value stored in this NXNumber
 * instance.
 * @return The 64-bit unsigned integer value stored in this instance.
 */
- (uint64_t)unsignedInt64Value;

@end
