#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>
#include <string.h>

#define DEFAULT_MAP_CAPACITY 32

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * @brief Key comparison function for NXMap - compares Objective-C string
 * objects
 */
static bool _nxmap_keyequals(void *keyptr, sys_hashtable_entry_t *entry) {
  id<NXConstantStringProtocol> key1 = (id)keyptr;
  id<NXConstantStringProtocol> key2 = (id)entry->keyptr;

  if (key1 == key2) {
    return true; // Same object
  }

  const char *str1 = [key1 cStr];
  const char *str2 = [key2 cStr];

  // Handle NULL string cases
  if (str1 == NULL && str2 == NULL) {
    return true;
  }
  if (str1 == NULL || str2 == NULL) {
    return false;
  }

  return strcmp(str1, str2) == 0;
}

/**
 * @brief Hash function for NXMap - uses the C string content
 */
static uintptr_t _nxmap_hash(id<NXConstantStringProtocol> key) {
  const char *str = [key cStr];
  // Handle NULL string case
  if (str == NULL) {
    return 0; // Default hash for NULL strings
  }
  return sys_hash_djb2(str);
}

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
  _data = sys_hashtable_init(capacity, _nxmap_keyequals);
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
  if (_data != nil) {
    // First, iterate through all objects and release them
    sys_hashtable_iterator_t iter = {0};
    sys_hashtable_iterator_t *iterptr = &iter;
    sys_hashtable_entry_t *entry;

    while ((entry = sys_hashtable_iterator_next(_data, &iterptr))) {
      objc_assert(entry->value);
      // Release both the key and value objects
      [(id)entry->keyptr release];
      [(id)entry->value release];
    }

    // Now free the hash table
    sys_hashtable_finalize(_data);
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

/**
 * @brief Creates and returns a new NXMap initialized with alternating objects
 * and keys.
 */
+ (NXMap *)mapWithObjectsAndKeys:(id)firstObject,
                                 ... OBJC_REQUIRES_NIL_TERMINATION {
  // Empty map case
  if (firstObject == nil) {
    return [NXMap new];
  }

  // Count the number of object-key pairs, and validate the keys
  va_list args;
  va_start(args, firstObject);
  size_t pairCount = 0;
  id currentArg = firstObject;
  while (currentArg != nil) {
    // Get the key for this object
    id keyArg = va_arg(args, id);
    if (keyArg == nil) {
      // Odd number of arguments - missing key for the last object
      va_end(args);
      return nil;
    }
    if ([keyArg conformsTo:@protocol(NXConstantStringProtocol)] == NO) {
      // Key doesn't conform to required protocol
      va_end(args);
      return nil;
    }
    pairCount++;
    // Get the next object (or nil to terminate)
    currentArg = va_arg(args, id);
  }
  va_end(args);

  // Create the new map with capacity for the pairs
  NXMap *map = [[NXMap alloc] initWithCapacity:pairCount];
  if (map == nil) {
    return nil;
  }

  // Now iterate again to add the object-key pairs
  va_start(args, firstObject);
  id currentObject = firstObject;
  size_t i;
  for (i = 0; i < pairCount; i++) {
    // Get the key for this object
    id<NXConstantStringProtocol, RetainProtocol> key = va_arg(args, id);
    if (key == nil) {
      // This shouldn't happen if first pass was correct, but safety check
      va_end(args);
      [map release];
      return nil;
    }

    if ([map setObject:currentObject forKey:key] == NO) {
      va_end(args);
      [map release]; // Release on failure
      return nil;
    }

    // Get the next object (or nil if we're done)
    currentObject = va_arg(args, id);
  }
  va_end(args);

  return [map autorelease];
}

/**
 * @brief Creates and returns a new NXMap initialized with objects from one
 * array and corresponding keys from another array.
 */
+ (NXMap *)mapWithObjects:(NXArray *)objects forKeys:(NXArray *)keys {
  // Check for nil arrays
  if (objects == nil || keys == nil) {
    return nil;
  }

  // Check for count mismatch
  unsigned int objectCount = [objects count];
  if (objectCount != [keys count]) {
    return nil;
  }

  // Handle empty arrays case - return empty map
  if (objectCount == 0) {
    return [NXMap new];
  }

  // Create the new map with the required capacity
  NXMap *map = [[NXMap alloc] initWithCapacity:objectCount];
  if (map == nil) {
    return nil;
  }

  // Iterate through the arrays and set objects for keys
  unsigned int i;
  for (i = 0; i < objectCount; i++) {
    id object = [objects objectAtIndex:i];
    id key = [keys objectAtIndex:i];

    // Validate object and key
    if (object == nil || key == nil ||
        [key conformsTo:@protocol(NXConstantStringProtocol)] == NO ||
        [key conformsTo:@protocol(RetainProtocol)] == NO) {
      [map release];
      return nil;
    }

    // Set the object for the key
    if ([map setObject:object forKey:key] == NO) {
      [map release];
      return nil;
    }
  }

  return [map autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Returns the number of elements in the map.
 */
- (unsigned int)count {
  return _count;
}

/**
 * @brief Returns the capacity of the map.
 *
 * This is the maximum number of elements the map can hold before resizing.
 */
- (size_t)capacity {
  if (_data == nil) {
    return 0; // No data means no capacity
  }
  return sys_hashtable_capacity(_data);
}

/**
 * @brief Returns an array containing all keys stored in the map.
 */
- (NXArray *)allKeys {
  if (_data == nil || _count == 0) {
    return [NXArray new]; // Return empty array if no data
  }

  // Create an array to hold the keys
  NXArray *keys = [[NXArray alloc] initWithCapacity:_count];
  if (keys == nil) {
    return nil; // Allocation failed, return nil
  }

  // Iterate through the hashtable and collect keys
  sys_hashtable_iterator_t iter = {0};
  sys_hashtable_iterator_t *iterptr = &iter;
  sys_hashtable_entry_t *entry;
  while ((entry = sys_hashtable_iterator_next(_data, &iterptr))) {
    id key = (id)entry->keyptr;
    objc_assert(key);
    if ([keys append:key] == NO) {
      [keys release]; // Release on failure
      return nil;     // Allocation failed, return nil
    }
  }

  return [keys autorelease];
}

/**
 * @brief Returns an array containing all objects stored in the map.
 */
- (NXArray *)allObjects {
  if (_data == nil || _count == 0) {
    return [NXArray new]; // Return empty array if no data
  }

  // Create an array to hold the objects
  NXArray *objects = [[NXArray alloc] initWithCapacity:_count];
  if (objects == nil) {
    return nil; // Allocation failed, return nil
  }

  // Iterate through the hashtable and collect objects
  sys_hashtable_iterator_t iter = {0};
  sys_hashtable_iterator_t *iterptr = &iter;
  sys_hashtable_entry_t *entry;
  while ((entry = sys_hashtable_iterator_next(_data, &iterptr))) {
    id object = (id)entry->value;
    objc_assert(object);
    if ([objects append:object] == NO) {
      [objects release]; // Release on failure
      return nil;        // Allocation failed, return nil
    }
  }

  return [objects autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Resets the map by re-creating the internal data structure.
 * This effectively clears all key-value pairs.
 */
- (void)removeAllObjects {
  if (_data == nil) {
    return;
  }

  // First, iterate through all objects and release them
  sys_hashtable_iterator_t iter = {0};
  sys_hashtable_iterator_t *iterptr = &iter;
  sys_hashtable_entry_t *entry;
  while ((entry = sys_hashtable_iterator_next(_data, &iterptr))) {
    objc_assert(entry->value);
    // Release both the key and value objects
    [(id)entry->keyptr release];
    [(id)entry->value release];
  }

  // Now free the hash table
  sys_hashtable_finalize(_data);
  _count = 0;

  // Now create a new hash table with the same capacity
  _data = sys_hashtable_init(_capacity, _nxmap_keyequals);
  objc_assert(_data);
}

- (BOOL)setObject:(id)anObject
           forKey:(id<NXConstantStringProtocol, RetainProtocol>)key {
  objc_assert(anObject);
  objc_assert(key);

  // Check if map is in valid state
  if (_data == nil) {
    return NO; // Map is in invalid state
  }

  // Use the generic hashtable functions with the key object as the key
  uintptr_t hash = _nxmap_hash(key);
  bool samekey = false;
  sys_hashtable_entry_t *entry =
      sys_hashtable_put(_data, hash, (void *)key, &samekey);
  if (entry == NULL) {
    return NO; // Failed to set object
  }

  // Handle existing vs new keys
  if (samekey) {
    // Existing key - check if value is different
    if (entry->value != (uintptr_t)anObject) {
      // Different value - release old, retain new
      [(id)entry->value release];
      [anObject retain];
      entry->value = (uintptr_t)anObject;
    }
    // Same key, same value - no changes needed
  } else {
    // New key - retain both key and value, set entry, increment count
    [key retain];
    [anObject retain];
    entry->value = (uintptr_t)anObject;
    _count++;
  }

  return YES;
}

- (id)objectForKey:(id<NXConstantStringProtocol, RetainProtocol>)key {
  objc_assert(key);

  // Check if map is in valid state
  if (_data == nil) {
    return nil; // Map is in invalid state
  }

  // Use the generic hashtable function to find the entry
  uintptr_t hash = _nxmap_hash(key);
  sys_hashtable_entry_t *entry =
      sys_hashtable_get_key(_data, hash, (void *)key);
  if (entry == NULL) {
    return nil; // Key not found
  }

  // Return the found object
  return (id)entry->value;
}

- (BOOL)removeObjectForKey:(id<NXConstantStringProtocol, RetainProtocol>)key {
  objc_assert(key);

  // Check if map is in valid state
  if (_data == nil) {
    return NO; // Map is in invalid state
  }

  // Use the generic hashtable function to find and delete the entry
  uintptr_t hash = _nxmap_hash(key);
  sys_hashtable_entry_t *entry =
      sys_hashtable_delete_key(_data, hash, (void *)key);
  if (entry == NULL) {
    return NO; // Key not found
  }

  // Release both the key and value objects, decrement count
  [(id)entry->keyptr release];
  [(id)entry->value release];

  // Clear the slot now that we've released the objects to prevent reuse
  // confusion
  entry->keyptr = NULL;
  entry->value = 0;

  _count--;

  return YES;
}

///////////////////////////////////////////////////////////////////////////////
// METHODS - COLLECTION PROTOCOL

- (BOOL)containsObject:(id)object {
  objc_assert(object);
  return NO; // Not implemented for NXMap
}

///////////////////////////////////////////////////////////////////////////////
// METHODS - JSON PROTOCOL

/**
 * @brief Returns a JSON representation of the instance.
 */
- (NXString *)JSONString {
  return [NXString stringWithString:@"TODO"];
}

/**
 * @brief Returns the appropriate capacity for the JSON
 * representation of the instance.
 */
- (size_t)JSONBytes {
  return 0;
}

@end
