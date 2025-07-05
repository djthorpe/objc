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
 *
 * \headerfile NXString.h NXFoundation/NXFoundation.h
 */
@interface NXString : NXObject {
@private
    id _value; ///< Internal storage for the string content, retained by this instance.
    BOOL _alloc; ///< Indicates whether the value was allocated.
}

/**
 * @brief Initializes a new string by referencing another string.
 * 
 * This method retains the source string, and if modified, it will create a mutable copy
 * of the modified string whilst releasing the original reference.
 *
 * @param other The source string (either NXConstantString or NXString) to retain.
 * @return The initialized string object.
 */
-(id) initWithString:(id)other;

/**
 * @brief Initializes a new string by referencing a constant c-string.
 * @param cStr A null-terminated C-string to create the string from.
 * @return A new NXString instance containing the C-string content.
 */
-(id) initWithCString:(const char* )cStr;

/**
 * @brief Returns the C-string representation of the string.
 * @return A pointer to a null-terminated C-string representing the string content.
 *
 * This method retrieves the C-string representation of the string, which can be used
 * in C-style string operations.
 */
-(const char* ) cStr;

/**
 * @brief Returns the length of the string.
 * @return The number of bytes in the string.
 *
 * This method returns the length of the string, in bytes, excluding
 * the null terminator.
 */
-(unsigned int) length;

@end
