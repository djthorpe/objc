#include "NXString+unicode.h"
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>
#include <string.h>

@implementation NXString

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes a new empty string
 */
- (id)init {
  self = [super init];
  if (self) {
    _value = NULL;
    _data = NULL;
    _length = 0;
    _cap = 0;
  }
  return self;
}

/**
 * @brief Initializes a new empty mutable string
 * @note The capacity argument is the initial size of the string buffer, which
 * includes the null terminator. If the capacity argument is zero, no memory
 * is allocated, and the string starts empty.
 */
- (id)initWithCapacity:(size_t)capacity {
  self = [self init];
  if (self == nil) {
    return nil; // Allocation failed during init
  }
  if (capacity == 0) {
    return self; // Return empty string with no capacity
  }

  // Allocate memory for the string data
  objc_assert(_zone);
  _data = [_zone allocWithSize:capacity];
  if (_data == NULL) {
    [self release];
    return nil; // Allocation failed, return nil
  }

  // Set initial values
  sys_memset(_data, 0, capacity); // Initialize allocated memory to zero
  _value = _data;                 // Set value to point to the allocated data
  _length = 0;                    // Initialize length to 0
  _cap = capacity;

  // Return success
  return self;
}

/**
 * @brief Initializes a new string by referencing another string.
 */
- (id)initWithString:(id<NXConstantStringProtocol, ObjectProtocol>)other {
  self = [self init];
  if (self == nil || self == other || other == nil) {
    return nil;
  }
  if (object_getClass(other) == objc_lookupClass("NXConstantString")) {
    _value = [other cStr];
    _length = [other length];
  } else if ([other isKindOfClass:[NXString class]]) {
    _value = [other cStr];
    _length = [other length];
  } else {
    [self release];
    return nil;
  }
  return self;
}

/**
 * @brief Initialize a new string by referencing a c-string.
 */
- (id)initWithCString:(const char *)cStr {
  self = [self init];
  if (self && cStr != NULL) {
    _value = cStr;
    _length = (unsigned int)strlen(cStr);
  }
  return self;
}

/**
 * @brief Initialize a new string with a format string and arguments.
 */
- (id)initWithFormat:(NXConstantString *)format, ... {
  va_list args;
  va_start(args, format);
  self = [self initWithFormat:format arguments:args];
  va_end(args);
  return self;
}

- (id)initWithFormat:(NXConstantString *)format arguments:(va_list)args {
  self = [self init];
  if (!self) {
    return self;
  }

  // Use a variable argument list to handle the format string
  va_list argsCopy;
  va_copy(argsCopy, args);

  // Get the length of the formatted string
  const char *cFormat = [format cStr];
  _length = sys_vsprintf(NULL, 0, cFormat, args);
  if (_length > 0) {
    // Allocate memory for the string
    objc_assert(_zone);
    _data = [_zone allocWithSize:_length + 1];
    if (_data) {
      sys_vsprintf(_data, _length + 1, cFormat,
                   argsCopy); // Format the string into the allocated memory
      _value = _data;         // Set the value to the allocated data
      _cap = _length + 1;     // Set capacity to length + null terminator
    } else {
      [self release];
      self = nil; // Allocation failed, set self to nil
    }
  }
  va_end(argsCopy);
  return self;
}

/**
 * @brief Return a new empty string.
 */
+ (NXString *)new {
  objc_assert([NXAutoreleasePool currentPool]);
  return [[[NXString alloc] init] autorelease];
}

/**
 * @brief Return a string by referencing a c-string.
 */
+ (NXString *)stringWithCString:(const char *)cStr {
  objc_assert([NXAutoreleasePool currentPool]);
  return [[[NXString alloc] initWithCString:cStr] autorelease];
}

/**
 * @brief Return a new empty string, but with a specific capacity.
 */
+ (NXString *)stringWithCapacity:(size_t)capacity {
  objc_assert([NXAutoreleasePool currentPool]);
  return [[[NXString alloc] initWithCapacity:capacity] autorelease];
}

/**
 * @brief Return a string by referencing another string.
 */
+ (NXString *)stringWithString:
    (id<NXConstantStringProtocol, ObjectProtocol>)other {
  objc_assert([NXAutoreleasePool currentPool]);
  return [[[NXString alloc] initWithString:other] autorelease];
}

/**
 * @brief Return a string instance with a format string and arguments.
 */
+ (NXString *)stringWithFormat:(NXConstantString *)format, ... {
  va_list args;
  va_start(args, format);
  id instance = [[[self alloc] initWithFormat:format
                                    arguments:args] autorelease];
  va_end(args);
  return instance;
}

/**
 * @brief Releases the string's internal value.
 */
- (void)dealloc {
  if (_data) {
    [_zone free:_data]; // Free the allocated data
  }
  [super dealloc];
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

- (BOOL)_makeMutableWithCapacity:(size_t)cap {
  // Validate input - ensure capacity is at least current capacity or minimum
  // required
  size_t minRequired =
      _length + 1; // Minimum for current string + null terminator
  if (cap < minRequired) {
    cap = minRequired;
  }
  if (cap < _cap) {
    cap = _cap; // Don't reduce existing capacity
  }

  // Fast path: if allocated data is already present and capacity is sufficient
  if (_data != NULL && _cap >= cap) {
    return YES; // Already mutable with sufficient capacity
  }

  // If _data is NULL, let's make it mutable directly
  if (_data == NULL) {
    _data = [_zone allocWithSize:cap];
    if (_data == NULL) {
      return NO; // Allocation failed, cannot make mutable
    }

    // Copy existing string content if any
    if (_value != NULL && _length > 0) {
      sys_memcpy(_data, _value, _length + 1);
    } else {
      sys_memset(_data, 0, cap); // Initialize allocated memory to zero
    }
    _value = _data; // Update _value to point to mutable data
    _cap = cap;
    return YES; // Now _data is mutable
  }

  // If _data is not NULL but capacity is insufficient, reallocate
  char *data = [_zone allocWithSize:cap];
  if (data == NULL) {
    return NO; // Allocation failed, cannot make mutable
  }

  // Copy existing data
  if (_length > 0) {
    sys_memcpy(data, _data, _length + 1);
  } else {
    sys_memset(data, 0, cap); // Initialize allocated memory to zero
  }

  // Free old data and update pointers
  [_zone free:_data];
  _data = data;
  _value = _data; // Ensure _value points to the mutable data
  _cap = cap;

  // Return success
  return YES;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Returns the C-string representation of the string.
 */
- (const char *)cStr {
  return _value ? _value : "";
}

/**
 * @brief Returns the length of the string.
 */
- (unsigned int)length {
  if (_value) {
    return _length;
  }
  return 0;
}

/**
 * @brief Returns the size of the currently allocated string buffer.
 */
- (size_t)capacity {
  return _cap;
}

/**
 * @brief Returns the string through a description method.
 */
- (NXString *)description {
  return self;
}

/**
 * @brief Checks if the string is equal to another object.
 */
- (BOOL)isEqual:(id)other {
  if (self == other) {
    return YES;
  }
  if (other == nil ||
      [other conformsTo:@protocol(NXConstantStringProtocol)] == NO) {
    return NO;
  }
  if ([other length] != _length) {
    return NO;
  }
  const char *otherCStr = [other cStr];
  if (otherCStr == _value) {
    return YES; // Same pointer, same content
  }
  return strcmp(_value ? _value : "", otherCStr ? otherCStr : "") == 0;
}

/**
 * @brief Compares this string with another string.
 */
- (NXComparisonResult)compare:(id<NXConstantStringProtocol>)other {
  objc_assert(other);
  const char *otherCStr = [other cStr];
  if (otherCStr == _value) {
    return NXComparisonSame; // Same pointer, same content
  }
  return strcmp(_value ? _value : "", otherCStr ? otherCStr : "");
}

/**
 * @brief Counts the number of occurrences of a byte character.
 */
- (uint32_t)countOccurrencesOfByte:(uint8_t)ch {
  if (_value == NULL) {
    return 0; // No occurrences in a NULL string
  }
  uint32_t count = 0;
  unsigned int i = 0;
  for (i = 0; i < _length; i++) {
    if (_value[i] == ch) {
      count++;
    }
  }
  return count;
}

/**
 * @brief Checks if the string starts with a given prefix.
 */
- (BOOL)hasPrefix:(id<NXConstantStringProtocol>)prefix {
  objc_assert(prefix);
  const char *prefixCStr = [prefix cStr];
  objc_assert(prefixCStr);
  size_t prefixLength = (size_t)[prefix length];

  if (prefixLength == 0) {
    return YES; // An empty prefix matches any string
  }
  if (prefixLength > _length || _value == NULL) {
    return NO; // Prefix is longer than the string
  }
  return strncmp(_value, prefixCStr, prefixLength) == 0;
}

/**
 * @brief Checks if the string ends with a given suffix.
 */
- (BOOL)hasSuffix:(id<NXConstantStringProtocol>)suffix {
  objc_assert(suffix);
  const char *suffixCStr = [suffix cStr];
  objc_assert(suffixCStr);
  size_t suffixLength = (size_t)[suffix length];

  if (suffixLength == 0) {
    return YES; // An empty suffix matches any string
  }
  if (suffixLength > _length || _value == NULL) {
    return NO; // Suffix is longer than the string
  }
  return strncmp(_value + (_length - suffixLength), suffixCStr, suffixLength) ==
         0;
}

/**
 * @brief Converts the string to uppercase.
 */
- (BOOL)toUppercase {
  if (_value == NULL || _length == 0) {
    return YES; // Nothing to convert
  }

  // Ensure string is mutable. Capacity is set to 0 to ensure it reallocates
  // enough space for the current string. If the string is already mutable,
  // this will be a no-op.
  if ([self _makeMutableWithCapacity:0] == NO) {
    return NO; // Failed to make mutable, cannot convert
  }

  size_t i;
  BOOL modified = NO;
  for (i = 0; i < _length; i++) {
    char upperChar = _char_toUpper(_data[i]);
    if (upperChar != _data[i]) {
      modified = YES;
      _data[i] = upperChar;
    }
  }

  return modified;
}

/**
 * @brief Converts the string to lowercase.
 */
- (BOOL)toLowercase {
  if (_value == NULL || _length == 0) {
    return YES; // Nothing to convert
  }

  // Ensure string is mutable. Capacity is set to 0 to ensure it reallocates
  // enough space for the current string. If the string is already mutable,
  // this will be a no-op.
  if ([self _makeMutableWithCapacity:0] == NO) {
    return NO; // Failed to make mutable, cannot convert
  }

  size_t i;
  BOOL modified = NO;
  for (i = 0; i < _length; i++) {
    char lowerChar = _char_toLower(_data[i]);
    if (lowerChar != _data[i]) {
      modified = YES;
      _data[i] = lowerChar;
    }
  }

  return modified;
}

/**
 * @brief Appends another string to this string.
 */
- (BOOL)append:(id<NXConstantStringProtocol>)other {
  objc_assert(other);
  size_t otherLength = [other length];
  if (otherLength == 0) {
    return YES; // Nothing to append, empty string
  }

  // Ensure string is mutable. Capacity is set to 0 to ensure it reallocates
  // enough space for the current string. If the string is already mutable,
  // this will be a no-op.
  if ([self _makeMutableWithCapacity:_length + otherLength + 1] == NO) {
    return NO; // Failed to make mutable, cannot append
  }

  // Append the C-string to the end of the current string
  sys_memcpy(_data + _length, [other cStr], otherLength + 1);
  _length += otherLength; // Update length

  // Return success
  return YES;
}

/**
 * @brief Appends a null-terminated C-string to this string.
 */
- (BOOL)appendCString:(const char *)other {
  objc_assert(other);
  size_t otherLength = strlen(other);
  if (otherLength == 0) {
    return YES; // Nothing to append, empty C-string
  }

  // Ensure string is mutable. Capacity is set to 0 to ensure it reallocates
  // enough space for the current string. If the string is already mutable,
  // this will be a no-op.
  if ([self _makeMutableWithCapacity:_length + otherLength + 1] == NO) {
    return NO; // Failed to make mutable, cannot append
  }

  // Append the C-string to the end of the current string
  sys_memcpy(_data + _length, other, otherLength + 1);
  _length += otherLength; // Update length

  // Return success
  return YES;
}

/**
 * @brief Appends a string with format and arguments to this string.
 */
- (BOOL)appendStringWithFormat:(NXConstantString *)format, ... {
  objc_assert(format);

  va_list args;
  va_start(args, format);

  // Use a variable argument list to handle the format string
  va_list argsCopy;
  va_copy(argsCopy, args);

  // Get the length of the formatted string
  const char *cFormat = [format cStr];
  size_t length = sys_vsprintf(NULL, 0, cFormat, args);

  if (length == 0) {
    va_end(argsCopy);
    va_end(args);
    return YES; // Nothing to append, empty formatted string
  }

  // Ensure string is mutable with enough capacity for current string +
  // formatted string
  if ([self _makeMutableWithCapacity:_length + length + 1] == NO) {
    va_end(argsCopy);
    va_end(args);
    return NO; // Failed to make mutable, cannot append
  }

  // Format the string directly into the allocated memory at the end of current
  // string
  sys_vsprintf(_data + _length, length + 1, cFormat, argsCopy);
  _length += length; // Update length

  va_end(argsCopy);
  va_end(args);

  // Return success
  return YES;
}

/**
 * @brief Trims leading and trailing whitespace from the string.
 * @return YES if the string was modified, NO if it was already trimmed.
 */
- (BOOL)trimWhitespace {
  if (_value == NULL || _length == 0) {
    return NO; // Nothing to trim, empty string
  }

  // Find leading whitespace
  size_t start = 0;
  while (start < _length && _char_isWhitespace(_value[start])) {
    start++;
  }

  // Find trailing whitespace
  size_t end = _length;
  while (end > start && _char_isWhitespace(_value[end - 1])) {
    end--;
  }

  // If no changes, return NO
  if (start == 0 && end == _length) {
    return NO; // No trimming needed
  }

  // Ensure string is mutable with enough capacity for trimmed string
  if ([self _makeMutableWithCapacity:end - start + 1] == NO) {
    return NO; // Failed to make mutable, cannot trim
  }

  // Move trimmed content to the start of the string
  if (start > 0) {
    sys_memcpy(_data, _data + start, end - start);
  }

  // Null-terminate the new string and update length
  _data[end - start] = '\0';
  _length = end - start;

  // String was modified
  return YES;
}

@end
