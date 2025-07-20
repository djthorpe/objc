/**
 * @file NXNumberUnsignedInt.h
 * @brief Defines the NXNumberUnsignedInt class, which represents unsigned
 * integer and boolean values.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief A class for managing and manipulating unsigned integer and boolean
 * values.
 */
@interface NXNumberUnsignedInt : NXObject {
@private
  unsigned int _value; ///< The unsigned integer value stored in this instance.
}

/**
 * @brief Return an instance of an unsigned integer value.
 * @param value The unsigned integer value to store in this instance.
 * @return A newly initialized NXNumberUnsignedInt instance, or nil if
 * initialization failed.
 */
+ (id)numberWithUnsignedInt:(unsigned int)value;

/**
 * @brief Return an instance of a boolean value.
 * @param value The boolean value to store
 * @return A newly initialized NXNumberUnsignedInt instance, or nil if
 * initialization failed.
 */
+ (id)numberWithBool:(BOOL)value;

/**
 * @brief Get the stored value as an unsigned integer.
 * @return The unsigned integer value stored in this instance.
 */
- (unsigned int)unsignedIntValue;

/**
 * @brief Get the stored value as a boolean.
 * @return YES if the stored value is non-zero, NO if it is zero.
 */
- (BOOL)boolValue;

@end
