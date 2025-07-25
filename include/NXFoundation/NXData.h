/**
 * @file NXData.h
 * @brief Defines a structure for storing binary data.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS

/**
 * @brief Hash algorithms supported by NXData.
 */
typedef enum {
  NXHashAlgorithmMD5,    // MD5 hash (128-bit)
  NXHashAlgorithmSHA256, // SHA-256 hash (256-bit)
} NXHashAlgorithm;

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The NXData class
 *
 * NXData represents a structure for storing binary data.
 *
 * \headerfile NXData.h NXFoundation/NXFoundation.h
 */
@interface NXData : NXObject <JSONProtocol> {
  void *_data;  // Raw data buffer
  size_t _size; // Current size in bytes
  size_t _cap;  // Total capacity in bytes
}

/**
 * @brief Return a new empty NXData instance.
 */
+ (NXData *)new;

/**
 * @brief Initializes a new NXData instance with the specified capacity.
 * @param capacity The initial capacity of the data, in bytes.
 * @return A new NXData instance with the specified capacity.
 */
- (id)initWithCapacity:(size_t)capacity;

/**
 * @brief Initializes a new NXData instance with a copy of the specified string.
 * @param aString The string to copy.
 * @return A new NXData instance containing a copy of the specified string.
 */
- (id)initWithString:(id<NXConstantStringProtocol>)aString;

/**
 * @brief Initializes a new NXData instance with a copy of bytes.
 * @param bytes The bytes to copy.
 * @param size The number of bytes to copy.
 * @return A new NXData instance containing a copy of the specified bytes.
 */
- (id)initWithBytes:(const void *)bytes size:(size_t)size;

/**
 * @brief Returns a new NXData instance with the specified capacity.
 * @param capacity The initial capacity of the data, in bytes.
 * @return A new NXData instance with the specified capacity.
 *
 * This method allocates a new NXData instance with the specified capacity.
 */
+ (NXData *)dataWithCapacity:(size_t)capacity;

/**
 * @brief Returns a new NXData instance with a copy of the specified string.
 * @param aString The string to copy.
 * @return A new NXData instance containing a copy of the specified string.
 *
 * This method allocates a new NXData instance and copies the contents of the
 * specified string into it.
 */
+ (NXData *)dataWithString:(id<NXConstantStringProtocol>)aString;

/**
 * @brief Returns a new NXData instance with a copy of bytes.
 * @param bytes The bytes to copy.
 * @param size The number of bytes to copy.
 * @return A new NXData instance containing a copy of the specified bytes.
 *
 * This method allocates a new NXData instance and copies the contents of the
 * specified bytes into it.
 */
+ (NXData *)dataWithBytes:(const void *)bytes size:(size_t)size;

/**
 * @brief Returns the size of the data in bytes.
 */
- (size_t)size;

/**
 * @brief Returns the capacity of the data in bytes.
 */
- (size_t)capacity;

/**
 * @brief Returns a pointer to the raw data bytes.
 * @return A pointer to the raw data bytes, or NULL if the data is empty.
 */
- (const void *)bytes;

/**
 * @brief Creates a new NXData instance with a hash of the data.
 * @param algorithm The hash algorithm to use.
 * @return A new NXData instance containing the hash of the data.
 */
- (NXData *)hashWithAlgorithm:(NXHashAlgorithm)algorithm;

/**
 * @brief Returns a hexadecimal string representation of the data.
 */
- (NXString *)hexString;

/**
 * @brief Returns a Base64 encoded string representation of the data.
 */
- (NXString *)base64Encoding;

/**
 * @brief Prints a hexdump representation of the data to the console.
 */
- (void)dump;

/**
 * @brief Appends a string to the data.
 * @param aString The string to append.
 * @return YES if successful, NO otherwise.
 */
- (BOOL)appendString:(id<NXConstantStringProtocol>)aString;

/**
 * @brief Appends raw bytes to the data.
 * @param bytes The bytes to append.
 * @param size The number of bytes to append.
 * @return YES if successful, NO otherwise.
 */
- (BOOL)appendBytes:(const void *)bytes size:(size_t)size;

/**
 * @brief Appends another NXData instance to this data, by copying its contents.
 * @param data The data to append.
 * @return YES if successful, NO otherwise.
 */
- (BOOL)appendData:(NXData *)data;

@end
