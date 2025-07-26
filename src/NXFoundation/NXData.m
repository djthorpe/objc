#include "NXString+unicode.h"
#include <NXFoundation/NXFoundation.h>

@implementation NXData

/**
 * @brief Initializes a new empty NXData instance.
 */
- (id)init {
  self = [super init];
  if (self) {
    _data = NULL;
    _size = 0;
    _cap = 0;
  }
  return self;
}

/**
 * @brief Initializes a new NXData instance with a specified capacity.
 */
- (id)initWithCapacity:(size_t)capacity {
  self = [self init];
  if (!self) {
    return nil;
  }
  if (capacity == 0) {
    return self; // No need to allocate if capacity is zero
  }

  // Allocate memory for the data
  _data = sys_malloc(capacity);
  if (_data == NULL) {
    [self release];
    return nil;
  } else {
    _cap = capacity;
  }

  // Return self
  return self;
}

/**
 * @brief Initializes a new NXData instance with a string constant
 */
- (id)initWithString:(id<NXConstantStringProtocol>)aString {
  objc_assert(aString);

  // Check length of the string
  size_t length = [aString length];
  if (length == 0) {
    return [self init]; // No need to copy if string is empty
  }

  // Allocate memory for the data
  self = [self initWithCapacity:length + 1]; // +1 for null terminator
  if (!self) {
    return nil;
  } else {
    _size = _cap;
  }

  // Copy the string data into the NXData instance, including
  // null termination
  objc_assert(_data);
  sys_memcpy(_data, [aString cStr], length + 1);

  // Return self
  return self;
}

/**
 * @brief Initializes a new NXData instance with bytes
 */
- (id)initWithBytes:(const void *)bytes size:(size_t)size {
  objc_assert(bytes);

  // Check size
  if (size == 0) {
    return [self init]; // Handle NULL/empty input gracefully
  }

  // Set the capacity
  self = [self initWithCapacity:size];
  if (!self) {
    return nil;
  } else {
    _size = size;
  }

  // Copy the bytes into the NXData instance
  objc_assert(_data);
  sys_memcpy(_data, bytes, size);

  // Return self
  return self;
}

/**
 * @brief Deallocates the NXData instance and frees allocated memory.
 */
- (void)dealloc {
  sys_free(_data);
  [super dealloc];
}

/**
 * @brief Return a new empty NXData instance.
 */
+ (NXData *)new {
  return [[[NXData alloc] init] autorelease];
}

/**
 * @brief Returns a new NXData instance with the specified capacity.
 */
+ (NXData *)dataWithCapacity:(size_t)capacity {
  return [[[NXData alloc] initWithCapacity:capacity] autorelease];
}

/**
 * @brief Returns a new NXData instance with a copy of the specified string.
 */
+ (NXData *)dataWithString:(id<NXConstantStringProtocol>)aString {
  return [[[NXData alloc] initWithString:aString] autorelease];
}

/**
 * @brief Returns a new NXData instance with a copy of the specified bytes.
 */
+ (NXData *)dataWithBytes:(const void *)bytes size:(size_t)size {
  return [[[NXData alloc] initWithBytes:bytes size:size] autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Returns the size of the data in bytes.
 */
- (size_t)size {
  return _size;
}

/**
 * @brief Returns the capacity of the data in bytes.
 */
- (size_t)capacity {
  return _cap;
}

/**
 * @brief Returns a pointer to the raw data bytes.
 */
- (const void *)bytes {
  return _data;
}

/**
 * @brief Creates a new NXData instance with a hash of the data.
 */
- (NXData *)hashWithAlgorithm:(NXHashAlgorithm)algorithm {
  (void)algorithm; // Mark as intentionally unused
  // TODO: Implement hash computation
  return nil;
}

/**
 * @brief Returns a hexadecimal string representation of the data.
 */
- (NXString *)hexString {
  static const char hexDigits[] = "0123456789ABCDEF";

  // Handle empty data case
  if (_size == 0 || _data == NULL) {
    return [NXString stringWithCString:""];
  }

  // Make a new mutable string with enough capacity, including the null
  // terminator
  NXString *result = [NXString stringWithCapacity:(_size * 2) + 1];
  if (result == nil) {
    return nil; // Handle memory allocation failure
  }

  // Get the buffer from the result string to write hex characters into,
  // since we've created a mutable string this shouldn't fail
  char *data = (char *)[result bytes];
  objc_assert(data);

  size_t i;
  size_t j = 0;
  for (i = 0; i < _size; i++) {
    char byte = ((char *)_data)[i];
    data[j++] = hexDigits[(byte >> 4) & 0x0F];
    data[j++] = hexDigits[byte & 0x0F];
  }
  data[j] = '\0';

  // Return the hex string
  return result;
}

/**
 * @brief Returns the capacity needed for the Base64 encoded string
 * representation of the data.
 */
- (size_t)base64Capacity {
  return ((_size + 2) / 3) * 4 + 1; // +1 for null terminator
}

/**
 * @brief Returns a Base64 encoded string representation of the data.
 */
- (NXString *)base64String {
  static const char b64_table[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  // Handle empty data case
  if (_size == 0 || _data == NULL) {
    return [NXString stringWithCString:""];
  }

  // Calculate the required buffer size
  size_t cap = [self base64Capacity];
  objc_assert(cap > 0);

  // Create a new mutable string with enough capacity, including the null
  NXString *result = [NXString stringWithCapacity:cap + 1];
  if (result == NULL) {
    return nil; // Handle memory allocation failure
  }

  // Get the buffer and cast data properly
  char *output = (char *)[result bytes];
  const uint8_t *input = (const uint8_t *)_data;
  objc_assert(output);

  // Process complete 3-byte groups
  size_t i;
  size_t j = 0;
  for (i = 0; i + 2 < _size; i += 3) {
    uint32_t bits = (input[i] << 16) | (input[i + 1] << 8) | input[i + 2];
    output[j++] = b64_table[(bits >> 18) & 0x3f];
    output[j++] = b64_table[(bits >> 12) & 0x3f];
    output[j++] = b64_table[(bits >> 6) & 0x3f];
    output[j++] = b64_table[bits & 0x3f];
  }

  // Handle remaining bytes (1 or 2)
  if (i < _size) {
    uint32_t bits = input[i] << 16;
    if (i + 1 < _size) {
      bits |= input[i + 1] << 8;
    }
    output[j++] = b64_table[(bits >> 18) & 0x3f];
    output[j++] = b64_table[(bits >> 12) & 0x3f];
    if (i + 1 < _size) {
      output[j++] = b64_table[(bits >> 6) & 0x3f];
      output[j++] = '=';
    } else {
      output[j++] = '=';
      output[j++] = '=';
    }
  }
  output[j] = '\0';

  // Return the Base64 encoded string
  return result;
}

/**
 * @brief Prints a hexdump representation of the data to the console.
 */
- (void)dump {
  if (_size == 0) {
    sys_printf("No data to dump.\n");
    return;
  }

  // Get data
  uint8_t *data = (uint8_t *)_data;
  objc_assert(data);

  size_t i, j;
  for (i = 0; i < _size; i += 16) {
    // Print the address
    sys_printf("%p  ", (void *)(data + i));

    // Print hex values
    for (j = 0; j < 16; j++) {
      if (i + j < _size)
        sys_printf("%02x ", data[i + j]);
      else
        sys_printf("   "); // Padding for incomplete lines

      // Extra space after 8 bytes for readability
      if (j == 7)
        sys_printf(" ");
    }

    // Print ASCII representation
    sys_printf(" |");
    for (j = 0; j < 16; j++) {
      if (i + j < _size) {
        sys_printf("%c", _char_toPrintable(data[i + j]));
      } else {
        sys_printf(" "); // Padding for incomplete lines
      }
    }
    sys_printf("|\n");
  }
}

/**
 * @brief Appends a string to the data.
 * @param aString The string to append.
 * @return YES if successful, NO otherwise.
 */
- (BOOL)appendString:(id<NXConstantStringProtocol>)aString {
  (void)aString; // Mark as intentionally unused
  // TODO: Implement string appending
  return NO;
}

/**
 * @brief Appends raw bytes to the data.
 * @param bytes The bytes to append.
 * @param size The number of bytes to append.
 * @return YES if successful, NO otherwise.
 */
- (BOOL)appendBytes:(const void *)bytes size:(size_t)size {
  (void)bytes; // Mark as intentionally unused
  (void)size;  // Mark as intentionally unused
  // TODO
  return NO;
}

/**
 * @brief Appends another NXData instance to this data, by copying its
 * contents.
 * @param data The data to append.
 * @return YES if successful, NO otherwise.
 */
- (BOOL)appendData:(NXData *)data {
  (void)data; // Mark as intentionally unused
  // TODO
  return NO;
}

///////////////////////////////////////////////////////////////////////////////
// JSON PROTOCOL METHODS

/**
 * @brief Determines the approximate capacity of the JSON representation, which
 * is encoded as a base64 string.
 */
- (size_t)JSONBytes {
  return [self base64Capacity];
}

/**
 * @brief Return the base64 encoded JSON string representation of the data.
 */
- (NXString *)JSONString {
  return [self base64String];
}

@end
