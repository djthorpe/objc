
/**
 * @file NXNumber.h
 * @brief Defines number-related functions and classes for the NXFoundation
 * framework.
 */
#pragma once

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
 * @return A new NXNumber instance containing the boolean value.
 */
+ (NXNumber *)numberWithBool:(BOOL)value;

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
 * @brief Returns the boolean value stored in this NXNumber instance.
 * @return The boolean value stored in this instance.
 *
 * This method returns YES if the stored value is non-zero, NO otherwise.
 */
- (BOOL)boolValue;

@end
