/**
 * @file NXNumberInt.h
 * @brief Defines the NXNumberInt class, which represents signed
 * integer.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief A class for managing and manipulating integer values.
 */
@interface NXNumberInt : NXObject {
@private
  int _value; ///< The integer value stored in this instance.
}

/**
 * @brief Return an instance of an integer value.
 * @param value The integer value to store in this instance.
 * @return A newly initialized NXNumberInt instance, or nil if
 * initialization failed.
 */
+ (id)numberWithInt:(int)value;

/**
 * @brief Get the stored value as a signed integer.
 * @return The integer value stored in this instance.
 */
- (int)intValue;

/**
 * @brief Get the stored value as an unsigned integer.
 * @return The unsigned integer value stored in this instance.
 */
- (unsigned int)unsignedIntValue;

@end
