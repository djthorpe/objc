#include "NXMap+hash.h"
#include <NXFoundation/NXFoundation.h>

#define DEFAULT_MAP_CAPACITY 32

@implementation NXMap

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes a new empty map with a default capacity.
 */
- (id)initWithCapacity:(size_t)capacity {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Ensure minimum capacity of 1 for hash table
  if (capacity == 0) {
    capacity = 1;
  }

  // Allocate memory for the map data, and release if allocation fails
  _data = objc_hash_table_init(capacity);
  if (_data == nil) {
    [self release];
    self = nil;
  } else {
    _count = 0;
    _capacity = capacity;
  }

  // Return self
  return self;
}

/**
 * @brief Initializes a new NXMap instance with the default capacity.
 */
- (id)init {
  return [self initWithCapacity:DEFAULT_MAP_CAPACITY];
}

- (void)dealloc {
  // Release all stored objects before freeing the map data
  if (_data != nil) {
    // First, iterate through all objects and release them
    objc_hash_iterator_t iter = {0};
    const char *key;
    void *value;

    while (objc_hash_iterator_next(_data, &iter, &key, &value)) {
      if (value != nil) {
        [(id)value release];
      }
    }

    // Now free the hash table
    objc_hash_table_free(_data);
  }

  // Call superclass dealloc
  [super dealloc];
}

/**
 * @brief Returns a new empty map with the default capacity.
 */
+ (NXMap *)new {
  return [[[NXMap alloc] init] autorelease];
}

/**
 * @brief Returns a new empty map with the specified capacity.
 */
+ (NXMap *)mapWithCapacity:(size_t)capacity {
  return [[[NXMap alloc] initWithCapacity:capacity] autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Returns the number of elements in the map.
 */
- (unsigned int)count {
  return _count;
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Resets the map by re-creating the internal data structure.
 * This effectively clears all key-value pairs.
 */
- (void)removeAllObjects {
  if (_data != nil) {
    // First, iterate through all objects and release them
    objc_hash_iterator_t iter = {0};
    const char *key;
    void *value;

    while (objc_hash_iterator_next(_data, &iter, &key, &value)) {
      if (value != nil) {
        [(id)value release];
      }
    }

    // Now free the hash table and recreate it
    objc_hash_table_free(_data);
    _data = objc_hash_table_init(_capacity);
    objc_assert(_data);
    _count = 0;
  }
}

- (BOOL)setObject:(id)anObject forKey:(id<NXConstantStringProtocol>)key {
  objc_assert(anObject);
  objc_assert(key);

  // Add the object to the hash table, checking for overwrites
  const char *keystr = [key cStr];
  objc_assert(keystr);
  void *overwritten_value;
  if (objc_hash_set(_data, keystr, anObject, &overwritten_value) == false) {
    return NO; // Failed to set object
  }

  if (overwritten_value == nil) {
    // New key - retain the object and increment the count
    [anObject retain];
    _count++;
  } else if (overwritten_value != anObject) {
    // Existing key with different value - retain new object, release old object
    // (count unchanged)
    [anObject retain];
    [(id)overwritten_value release];
  }

  // Return success
  return YES;
}

- (id)objectForKey:(id<NXConstantStringProtocol>)key {
  objc_assert(key);

  // Retrieve the object from the hash table
  const char *keystr = [key cStr];
  objc_assert(keystr);
  id obj = objc_hash_lookup(_data, keystr);
  if (obj == nil) {
    return nil; // Key not found
  }

  // Return the found object
  return obj;
}

- (BOOL)removeObjectForKey:(id<NXConstantStringProtocol>)key {
  objc_assert(key);

  // Remove the object from the hash table
  const char *keystr = [key cStr];
  objc_assert(keystr);
  id obj = objc_hash_delete(_data, keystr);
  if (obj == nil) {
    return NO; // Key not found
  }

  // Release the object and decrement the count
  [obj release];
  _count--;

  // Return success
  return YES;
}

@end
