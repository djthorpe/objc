/**
 * @file NXString.h
 * @brief Defines the NXString class, which provides string manipulation
 * functionality.
 */
#pragma once

typedef enum {
  NXComparisonAscending = -1,
  NXComparisonSame = 0,
  NXComparisonDescending = 1
} NXComparisonResult;

/**
 * @brief A class for managing and manipulating string objects.
 *
 * NXString provides functionality for creating and working with text strings.
 * It supports both mutable and immutable string representations.
 *
 * \headerfile NXString.h NXFoundation/NXFoundation.h
 */
@interface NXString : NXObject <NXStringProtocol> {
@private
  const char *_value;
  char *_data; ///< Pointer to the retained string data
  unsigned int _length;
}

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
- (id)initWithString:(id<NXStringProtocol>)other;

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
 * @brief Appends another string to this string.
 * @param other The string to append.
 * @note If the string needs to be modified, it will create a mutable copy,
 * or attempt to increase capacity if necessary. If the operation fails,
 * it will currently panic, but should raise an exception in the future,
 * when exception handling is implemented.
 */
- (void)appendString:(id<NXStringProtocol>)other;

/**
 * @brief Appends a string with format and arguments to this string.
 * @param format The format string to use for appending.
 * @param ... Variable arguments for the format string.
 * @note If the string needs to be modified, it will create a mutable copy,
 * or attempt to increase capacity if necessary. If the operation fails,
 * it will currently panic, but should raise an exception in the future,
 * when exception handling is implemented.
 */
- (void)appendStringWithFormat:(NXConstantString *)format, ...;

/**
 * @brief Appends a null-terminated C-string to this string.
 * @param other The C-string to append.
 * @note If the string needs to be modified, it will create a mutable copy,
 * or attempt to increase capacity if necessary. If the operation fails,
 * it will currently panic, but should raise an exception in the future,
 * when exception handling is implemented.
 */
- (void)appendCString:(const char *)other;

/**
 * @brief Returns a quoted version of the string.
 * @return A new NXString instance containing the string enclosed in
 * double quotes.
 *
 * This method creates a new string that contains the original string
 * enclosed in double quotes. Special characters within the string are escaped
 * according to JSON string escaping rules.
 */
- (NXString *)quotedString;

/**
 * @brief Trims leading and trailing whitespace from the string.
 * @return YES if the string was modified, NO if it was already trimmed.
 */
- (BOOL)trimSpace;

/**
 * @brief Trims leading and trailing string values from the string.
 * @param prefix The NXConstantString or NXString instance to trim from the
 * start, or NULL to skip.
 * @param suffix The NXConstantString or NXString instance to trim from the
 * end, or NULL to skip.
 * @return YES if the string was modified, NO if it was already trimmed.
 */
- (BOOL)trimPrefix:(id<NXStringProtocol>)prefix
            suffix:(id<NXStringProtocol>)suffix;

/**
 * @brief Checks if the string starts with a given prefix.
 * @param prefix The NXConstantString or NXString instance to check as a
 * prefix.
 * @return YES if the string starts with the specified prefix, NO otherwise.
 */
- (BOOL)hasPrefix:(id<NXStringProtocol>)prefix;

/**
 * @brief Checks if the string ends with a given suffix.
 * @param suffix The NXConstantString or NXString instance to check as a
 * suffix.
 * @return YES if the string ends with the specified suffix, NO otherwise.
 */
- (BOOL)hasSuffix:(id<NXStringProtocol>)suffix;

/**
 * @brief Counts the number of occurrences of a character.
 * @param ch The character to count occurrences of.
 * @return The number of times the character appears in the string.
 */
- (uint32_t)countOccurrencesOfByte:(const char)ch;

/**
 * @brief Counts the number of occurrences of a string.
 * @param other The NXConstantString or NXString instance to count occurrences of.
 * @return The number of times the string appears in the string.
 */
- (uint32_t)countOccurrencesOfString:(id<NXStringProtocol>)other;

/**
 * @brief Checks if the string contains a given substring.
 * @param other The NXConstantString or NXString instance to check for
 * containment.
 * @return YES if the string contains the specified substring, NO otherwise.
 */
- (BOOL)containsString:(id<NXStringProtocol>)other;

/**
 * @brief Compares this string with another string.
 * @param other The string to compare against.
 * @return NXComparisonResult indicating whether this string is earlier
 * (Ascending), Same, or later (Descending) than the other string, according
 * to lexicographical order.
 */
- (NXComparisonResult)compare:(id<NXStringProtocol>)other;

@end
