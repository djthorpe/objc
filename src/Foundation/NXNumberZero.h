/**
 * @file NXNumberZero.h
 * @brief Defines the NXNumberZero class, which represents a singleton zero
 * value for all number types.
 */
#pragma once

#include <NXFoundation/NXFoundation.h>

/**
 * @brief A class for managing a singleton zero value that can represent
 * zero for any numeric type.
 *
 * NXNumberZero is a singleton class that provides a memory-efficient
 * representation of zero that can be converted to any numeric type.
 * This avoids the need to create separate zero instances for each
 * number class type.
 *
 * \headerfile NXNumberZero.h NXFoundation/NXFoundation.h
 */
@interface NXNumberZero : NXNumber

/**
 * @brief Returns the singleton zero value instance.
 * @return The singleton NXNumberZero instance.
 */
+ (NXNumber *)zeroValue;

/**
 * @brief Get the stored value as a boolean.
 * @return Always returns NO.
 */
- (BOOL)boolValue;

/**
 * @brief Get the stored value as a 16-bit signed integer.
 * @return Always returns 0.
 */
- (int16_t)int16Value;

/**
 * @brief Get the stored value as a 16-bit unsigned integer.
 * @return Always returns 0.
 */
- (uint16_t)unsignedInt16Value;

/**
 * @brief Get the stored value as a 32-bit signed integer.
 * @return Always returns 0.
 */
- (int32_t)int32Value;

/**
 * @brief Get the stored value as a 32-bit unsigned integer.
 * @return Always returns 0.
 */
- (uint32_t)unsignedInt32Value;

/**
 * @brief Get the stored value as a 64-bit signed integer.
 * @return Always returns 0.
 */
- (int64_t)int64Value;

/**
 * @brief Get the stored value as a 64-bit unsigned integer.
 * @return Always returns 0.
 */
- (uint64_t)unsignedInt64Value;

/**
 * @brief Return the string representation of the value.
 * @return A string containing "0".
 */
- (NXString *)description;

/**
 * @brief Check if this number is equal to another object.
 * @param object The object to compare with.
 * @return YES if the object represents zero, NO otherwise.
 */
- (BOOL)isEqual:(id)object;

@end
