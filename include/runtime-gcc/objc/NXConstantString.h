/**
 * @file NXConstantString.h
 * @brief Defines the NXConstantString class for constant strings.
 *
 * This class provides an immutable string object. It is a lightweight
 * alternative to more complex string classes.
 */
#pragma once
#include "Object.h"

/**
 * @brief A constant string class.
 *
 * This class is used to represent immutable strings. It stores a pointer
 * to a C-string and its length. For compatibility with modern Objective-C code,
 * it is aliased to `NSString` when compiling with Clang.
 *
 * \headerfile NXConstantString.h objc/objc.h
 */
@interface NXConstantString : Object {
  const char *_data;    ///< Pointer to the null-terminated C-string.
  unsigned int _length; ///< Length of the string in bytes, not including the
                        ///< null terminator.
}

/**
 * @brief Initializes a new constant string from a C-string.
 * @param cStr A null-terminated C-string.
 * @return A new NXConstantString instance.
 */
- (id)initWithCString:(const char *)cStr;

/**
 * @brief Returns the C-string representation of the constant string.
 * @return A pointer to the null-terminated C-string.
 */
- (const char *)cStr;

/**
 * @brief Returns the length of the constant string.
 * @return The length of the string in bytes, not including the null terminator.
 */
- (unsigned int)length;

@end

#ifdef __clang__
/**
 * @brief Provides a compatibility alias for `NSString`.
 *
 * When compiling with Clang, `NXConstantString` is aliased to `NSString`
 * to allow for greater compatibility with modern Objective-C code and
 * frameworks.
 */
@compatibility_alias NSString NXConstantString;
#endif
