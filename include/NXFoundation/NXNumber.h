
/**
 * @file NXNumber.h
 * @brief Defines number-related functions and classes for the NXFoundation
 * framework.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Generate a random signed integer.
 * @details This function generates a random signed integer value using the
 * system's random number generator.
 * @return A random signed integer value.
 * @note This function is not thread-safe.
 */
int NXRandInt();

/**
 * @brief Generate a random unsigned integer.
 * @details This function generates a random unsigned integer value.
 * @return A random unsigned integer value.
 * @note This function is not thread-safe.
 */
unsigned int NXRandUnsignedInt();
