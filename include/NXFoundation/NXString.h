/**
 * @file NXString.h
 * @brief Defines the NXString class, which provides string manipulation functionality.
 */
#pragma once

/**
 * @brief A class for managing and manipulating string objects.
 *
 * NXString provides functionality for creating and working with text strings.
 * It supports both mutable and immutable string representations.
 */
@interface NXString : NXObject {
@private
    id _other; ///< Internal storage for the string content, retained by this instance.
}

/**
 * @brief Initializes a new string by copying the contents of another string.
 * @param other The source string (either NXConstantString or NXString) to copy.
 * @return The initialized string object.
 */
-(id) initWithString:(id)other;

@end
