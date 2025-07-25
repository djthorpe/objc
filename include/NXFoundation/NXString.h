/**
 * @file NXString.h
 * @brief Defines the NXString class, which provides string manipulation
 * functionality.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief A class for managing and manipulating string objects.
 *
 * NXString provides functionality for creating and working with text strings.
 * It supports both mutable and immutable string representations.
 *
 * \headerfile NXString.h NXFoundation/NXFoundation.h
 */
@interface NXString : NXObject <NXConstantStringProtocol> {
@private
  const char *_value; ///< Pointer to the string data
  unsigned int
      _length; ///< Length of the string in bytes, excluding null terminator
  char *_data; ///< Pointer to the retained string data
  size_t _cap; ///< Capacity of the retained string data buffer
}

/**
 * @brief Return a new empty string.
 */
+ (NXString *)new;

/**
 * @brief Create a new empty string with pre-allocated capacity.
 * @param capacity The initial capacity (in bytes) to allocate for the string
 * buffer. This should include space for the null terminator.
 * @return A new autoreleased NXString instance with the specified capacity
 * allocated.
 *
 * This method creates a mutable string with internal storage
 * pre-allocated to the specified capacity. This is useful when you plan to
 * modify the string (e.g., with append operations) and want to avoid multiple
 * reallocations. The string starts empty but has the capacity to grow up to the
 * specified size without requiring memory reallocation.
 *
 * @note The capacity represents the total buffer size including the null
 * terminator, so a capacity of 10 allows for 9 characters plus the null
 * terminator, for example.
 *
 * @warning If memory allocation fails, this method will return nil.
 */
+ (NXString *)stringWithCapacity:(size_t)capacity;

/**
 * @brief Return a string by referencing another string.
 */
+ (NXString *)stringWithString:
    (id<NXConstantStringProtocol, ObjectProtocol>)other;

/**
 * @brief Return a string by referencing a c-string.
 */
+ (NXString *)stringWithCString:(const char *)cStr;

/**
 * @brief Return a string from format string and arguments.
 */
+ (NXString *)stringWithFormat:(NXConstantString *)format, ...;

/**
 * @brief Initializes a new string by referencing another string.
 *
 * This method retains the source string, and if modified, it will create a
 * mutable copy of the modified string whilst releasing the original reference.
 *
 * @param other The source string (either NXConstantString or NXString) to
 * retain.
 * @return The initialized string object.
 */
- (id)initWithString:(id<NXConstantStringProtocol, ObjectProtocol>)other;

/**
 * @brief Initializes a new empty mutable string with pre-allocated capacity.
 *
 * @param capacity The initial capacity (in bytes) to allocate for the string
 * buffer. This should include space for the null terminator. If 0 is passed, no
 * memory is allocated and the string starts empty.
 *
 * @return The initialized string object with the specified capacity allocated.
 *
 * @note This method creates a mutable string with internal storage
 * pre-allocated to the specified capacity. The string starts empty but has the
 * capacity to grow up to the specified size without requiring memory
 * reallocation.
 *
 * @warning If memory allocation fails, this method will return nil.
 */
- (id)initWithCapacity:(size_t)capacity;

/**
 * @brief Initializes a new string by referencing a constant c-string.
 * @param cStr A null-terminated C-string to create the string from.
 * @return A new NXString instance containing the C-string content.
 */
- (id)initWithCString:(const char *)cStr;

/**
 * @brief Initializes a new string with a formatted string.
 * @param format The format string to use for initialization.
 * @param ... Variable arguments for the format string.
 * @return A new NXString instance containing the formatted content.
 */
- (id)initWithFormat:(NXConstantString *)format, ...;

/**
 * @brief Initializes a new string with a formatted string and arguments.
 * @param format The format string to use for initialization.
 * @param args A va_list containing the arguments for the format string.
 * @return A new NXString instance containing the formatted content.
 *
 * This initializer is used internally to initialize the string with a format
 * and variable arguments. It is not intended to be called directly.
 */
- (id)initWithFormat:(NXConstantString *)format arguments:(va_list)args;

/**
 * @brief Returns the C-string representation of the string.
 * @return A pointer to a null-terminated C-string representing the string
 * content.
 *
 * This method retrieves the C-string representation of the string, which can be
 * used in C-style string operations.
 */
- (const char *)cStr;

/**
 * @brief Returns the length of the string.
 * @return The number of bytes in the string.
 *
 * This method returns the length of the string, in bytes, excluding
 * the null terminator.
 */
- (unsigned int)length;

/**
 * @brief Returns the capacity of the string.
 * @return The total allocated size of the string buffer, including the null
 * terminator.
 *
 * This method returns the current capacity of the string's internal buffer,
 * which may be larger than the actual length of the string. This is useful for
 * understanding how much space is available for appending or modifying the
 * string without requiring reallocation. If zero is returned, it indicates
 * that the string is currently immutable.
 */
- (size_t)capacity;

/**
 * @brief Appends another string to this string.
 * @param other The string to append.
 * @return YES if successfully modified, NO if it wasn't possible to allocate
 * additional capacity.
 */
- (BOOL)append:(id<NXConstantStringProtocol>)other;

/**
 * @brief Appends a null-terminated C-string to this string.
 * @param other The C-string to append.
 * @return YES if successfully modified, NO if it wasn't possible to allocate
 * additional capacity.
 */
- (BOOL)appendCString:(const char *)other;

/**
 * @brief Appends a string with format and arguments to this string.
 * @param format The format string to use for appending.
 * @param ... Variable arguments for the format string.
 * @return YES if successfully modified, NO if it wasn't possible to allocate
 * additional capacity.
 */
- (BOOL)appendStringWithFormat:(NXConstantString *)format, ...;

/**
 * @brief Trims leading and trailing whitespace from the string.
 * @return YES if the string was modified, NO if it was already trimmed.
 */
- (BOOL)trimWhitespace;

/**
 * @brief Trims leading and trailing string values from the string.
 * @param prefix The NXConstantString or NXString instance to trim from the
 * start, or NULL to skip.
 * @param suffix The NXConstantString or NXString instance to trim from the
 * end, or NULL to skip.
 * @return YES if the string was modified, NO if it was already trimmed.
 */
- (BOOL)trimPrefix:(id<NXConstantStringProtocol>)prefix
            suffix:(id<NXConstantStringProtocol>)suffix;

/**
 * @brief Checks if the string starts with a given prefix.
 * @param prefix The NXConstantString or NXString instance to check as a
 * prefix.
 * @return YES if the string starts with the specified prefix, NO otherwise.
 */
- (BOOL)hasPrefix:(id<NXConstantStringProtocol>)prefix;

/**
 * @brief Checks if the string ends with a given suffix.
 * @param suffix The NXConstantString or NXString instance to check as a
 * suffix.
 * @return YES if the string ends with the specified suffix, NO otherwise.
 */
- (BOOL)hasSuffix:(id<NXConstantStringProtocol>)suffix;

/**
 * @brief Counts the number of occurrences of a byte character.
 * @param ch The character to count occurrences of.
 * @return The number of times the character appears in the string.
 */
- (uint32_t)countOccurrencesOfByte:(uint8_t)ch;

/**
 * @brief Returns a quoted version of the string.
 * @return A new NXString instance containing the string enclosed in
 * double quotes.
 *
 * This method creates a new string that contains the original string
 * enclosed in double quotes. Special characters within the string are escaped
 * according to JSON string escaping rules.
 */
// - (NXString *)quotedString;

/**
 * @brief Counts the number of occurrences of a string.
 * @param other The NXConstantString or NXString instance to count occurrences
 * of.
 * @return The number of times the string appears in the string.
 */
// - (uint32_t)countOccurrencesOfString:(id<NXConstantStringProtocol>)other;

/**
 * @brief Checks if the string contains a given substring.
 * @param other The NXConstantString or NXString instance to check for
 * containment.
 * @return YES if the string contains the specified substring, NO otherwise.
 */
- (BOOL)containsString:(id<NXConstantStringProtocol>)other;

/**
 * @brief Compares this string with another string.
 * @param other The string to compare against.
 * @return NXComparisonResult indicating whether this string is earlier
 * (Ascending), Same, or later (Descending) than the other string, according
 * to lexicographical order.
 */
- (NXComparisonResult)compare:(id<NXConstantStringProtocol>)other;

/**
 * @brief Converts the string to uppercase.
 * @return YES if the string was modified to uppercase, NO if the string was
 * not modified (e.g., it is already uppercase or unable to make mutable).
 *
 * This method modifies the string in place, converting all alphabetic
 * characters to their uppercase equivalents.
 */
- (BOOL)toUppercase;

/**
 * @brief Converts the string to lowercase.
 * @return YES if the string was modified to lowercase, NO if the string was
 * not modified (e.g., it is already lowercase or unable to make mutable).
 *
 * This method modifies the string in place, converting all alphabetic
 * characters to their lowercase equivalents.
 */
- (BOOL)toLowercase;

@end
