
/**
 * @file NXNumber.h
 * @brief Defines number-related functions and classes for the NXFoundation
 * framework.
 */
#pragma once

#include <NXFoundation/NXObject.h>
#include <stdint.h>

@class NXString;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Generate a random signed 32-bit integer.
 * @details This function generates a random signed integer value using the
 * system's random number generator.
 * @return A random signed integer value.
 * @note This function is not thread-safe.
 */
int32_t NXRandInt32();

/**
 * @brief Generate a random unsigned 32-bit integer.
 * @details This function generates a random unsigned integer value.
 * @return A random unsigned integer value.
 * @note This function is not thread-safe.
 */
uint32_t NXRandUnsignedInt32();

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The base class for NXNumber instances.
 *
 * NXNumber represents a numeric value in the NXFoundation framework.
 *
 * \headerfile NXNumber.h NXFoundation/NXFoundation.h
 */
@interface NXNumber : NXObject

/**
 * @brief Creates a new NXNumber instance with a boolean value.
 * @param value The boolean value to wrap in the number instance.
 * @return A new NXNumber instance containing the value.
 */
+ (NXNumber *)numberWithBool:(BOOL)value;

/**
 * @brief Creates a new NXNumber instance with a 16-bit signed integer value.
 * @param value The 16-bit signed integer value to wrap in the number instance.
 * @return A new NXNumber instance containing the value.
 */
+ (NXNumber *)numberWithInt16:(int16_t)value;

/**
 * @brief Creates a new NXNumber instance with an unsigned 16-bit integer value.
 * @param value The 16-bit unsigned integer value to wrap in the number
 * instance.
 * @return A new NXNumber instance containing the value.
 */
+ (NXNumber *)numberWithUnsignedInt16:(uint16_t)value;

/**
 * @brief Creates a new NXNumber instance with a 32-bit signed integer value.
 * @param value The 32-bit signed integer value to wrap in the number instance.
 * @return A new NXNumber instance containing the value.
 */
+ (NXNumber *)numberWithInt32:(int32_t)value;

/**
 * @brief Creates a new NXNumber instance with an unsigned 32-bit integer value.
 * @param value The 32-bit unsigned integer value to wrap in the number
 * instance.
 * @return A new NXNumber instance containing the value.
 */
+ (NXNumber *)numberWithUnsignedInt32:(uint32_t)value;

/**
 * @brief Creates a new NXNumber instance with a 64-bit signed integer value.
 * @param value The 64-bit signed integer value to wrap in the number instance.
 * @return A new NXNumber instance containing the value.
 */
+ (NXNumber *)numberWithInt64:(int64_t)value;

/**
 * @brief Creates a new NXNumber instance with an unsigned 64-bit integer value.
 * @param value The 64-bit unsigned integer value to wrap in the number
 * instance.
 * @return A new NXNumber instance containing the value.
 */
+ (NXNumber *)numberWithUnsignedInt64:(uint64_t)value;

/**
 * @brief Returns a NXNumber instance representing the boolean value true.
 * @return A NXNumber instance with the value true.
 */
+ (NXNumber *)trueValue;

/**
 * @brief Returns a NXNumber instance representing the boolean value false.
 * @return A NXNumber instance with the value false.
 */
+ (NXNumber *)falseValue;

/**
 * @brief Return an instance of a zero value.
 * @return A statically allocated NXNumber instance representing zero.
 */
+ (NXNumber *)zeroValue;

/**
 * @brief Returns the boolean value stored in this NXNumber instance.
 * @return The boolean value stored in this instance.
 *
 * This method returns YES if the stored value is non-zero, NO otherwise.
 */
- (BOOL)boolValue;

/**
 * @brief Returns the 16-bit signed integer value stored in this NXNumber
 * instance.
 * @return The 16-bit signed integer value stored in this instance.
 */
- (int16_t)int16Value;

/**
 * @brief Returns the 16-bit unsigned integer value stored in this NXNumber
 * instance.
 * @return The 16-bit unsigned integer value stored in this instance.
 */
- (uint16_t)unsignedInt16Value;

/**
 * @brief Returns the 32-bit signed integer value stored in this NXNumber
 * instance.
 * @return The 32-bit signed integer value stored in this instance.
 */
- (int32_t)int32Value;

/**
 * @brief Returns the 32-bit unsigned integer value stored in this NXNumber
 * instance.
 * @return The 32-bit unsigned integer value stored in this instance.
 */
- (uint32_t)unsignedInt32Value;

/**
 * @brief Returns the 64-bit signed integer value stored in this NXNumber
 * instance.
 * @return The 64-bit signed integer value stored in this instance.
 */
- (int64_t)int64Value;

/**
 * @brief Returns the 64-bit unsigned integer value stored in this NXNumber
 * instance.
 * @return The 64-bit unsigned integer value stored in this instance.
 */
- (uint64_t)unsignedInt64Value;

@end
