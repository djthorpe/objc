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

  // Allocate memory for the  data
  _data = sys_malloc(capacity);
  if (_data == 0) {
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
  objc_assert(bytes != NULL);

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
  char *hexBuffer;
  const uint8_t *dataBytes;
  size_t i;
  uint8_t byte;
  NXString *result;

  // Handle empty data case
  if (_size == 0 || _data == NULL) {
    return [NXString stringWithCString:""];
  }

  // Allocate buffer for hex string (2 chars per byte + null terminator)
  hexBuffer = (char *)sys_malloc(_size * 2 + 1);
  if (!hexBuffer) {
    return nil; // Handle memory allocation failure
  }

  // Convert each byte to two hex characters
  dataBytes = (const uint8_t *)_data;

  for (i = 0; i < _size; i++) {
    byte = dataBytes[i];
    hexBuffer[i * 2] = hexDigits[(byte >> 4) & 0x0F];
    hexBuffer[i * 2 + 1] = hexDigits[byte & 0x0F];
  }

  // Null-terminate the string
  hexBuffer[_size * 2] = '\0';

  // Create NXString from C string and free the buffer
  result = [NXString stringWithCString:hexBuffer];
  sys_free(hexBuffer);

  return result;
}

/**
 * @brief Returns a Base64 encoded string representation of the data.
 */
- (NXString *)base64Encoding {
  // TODO
  return nil;
}

/**
 * @brief Prints a hexdump representation of the data to the console.
 */
- (void)dump {
  // TODO
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

- (size_t)JSONBytes {
  // TODO: Return approximate bytes needed for JSON representation
  return _size * 2; // Rough estimate for hex encoding
}

- (NXString *)JSONString {
  // TODO: Implement JSON string representation
  return nil;
}

@end
