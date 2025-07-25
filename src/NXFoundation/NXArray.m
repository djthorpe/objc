#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>
#include <stdarg.h>

@implementation NXArray

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes a new empty array
 */
- (id)init {
  self = [super init];
  if (self) {
    _data = NULL;
    _length = 0;
    _cap = 0;
  }
  return self;
}

/**
 * @brief Initializes a new NXArray instance with the specified capacity.
 */
- (id)initWithCapacity:(size_t)capacity {
  self = [self init];
  if (!self) {
    return nil;
  }

  // Just return self if capacity is zero
  if (capacity == 0) {
    return self;
  }

  // Allocate memory for the array data
  _data = sys_malloc(capacity * sizeof(void *));
  if (_data == 0) {
    [self release];
    self = nil;
    return nil;
  } else {
    _cap = capacity;
  }

  // Initialize all elements to NULL
  sys_memset(_data, 0, capacity * sizeof(void *));

  // Return self
  return self;
}

/**
 * @brief Internal helper method to initialize with va_list.
 */
- (id)_initWithFirstObject:(id<RetainProtocol>)firstObject
                 arguments:(va_list)args {
  self = [self init];
  if (!self) {
    return nil;
  }

  // If no first object, return empty array
  if (firstObject == nil) {
    return self;
  }

  // Count objects first to determine capacity needed
  va_list argsCopy;
  va_copy(argsCopy, args);

  size_t objectCount = 1; // Count the first object
  id currentObject;
  while ((currentObject = va_arg(argsCopy, id)) != nil) {
    objectCount++;
  }
  va_end(argsCopy);

  // Do not allow for objectCount == NXNotFound
  if (objectCount == (size_t)NXNotFound) {
    [self release];
    return nil;
  }

  // Allocate memory for the objects
  _data = sys_malloc(objectCount * sizeof(void *));
  if (_data == NULL) {
    [self release];
    return nil;
  }

  // Set capacity and length
  _cap = objectCount;
  _length = objectCount;

  // Initialize all elements to NULL first
  sys_memset(_data, 0, objectCount * sizeof(void *));

  // Now populate the array with the objects
  _data[0] = [firstObject retain]; // Store and retain the first object

  size_t index = 1;
  while ((currentObject = va_arg(args, id)) != nil && index < objectCount) {
    _data[index] = [currentObject retain]; // Store and retain each object
    index++;
  }

  return self;
}

/**
 * @brief Initializes an array with a variadic list of objects.
 */
- (id)initWithObjects:(id<RetainProtocol>)firstObject, ... {
  va_list args;
  va_start(args, firstObject);
  self = [self _initWithFirstObject:firstObject arguments:args];
  va_end(args);
  return self;
}

/**
 * @brief Deallocates the array and releases all contained objects.
 */
- (void)dealloc {
  // Release all objects in the array
  if (_data != NULL) {
    size_t i;
    for (i = 0; i < _length; i++) {
      if (_data[i] != NULL) {
        [(id)_data[i] release];
      }
    }
    // Free the array data
    sys_free(_data);
  }

  [super dealloc];
}

/**
 * @brief Returns a new empty NXArray instance.
 */
+ (NXArray *)new {
  return [[[NXArray alloc] init] autorelease];
}

/**
 * @brief Returns a new NXArray instance with the specified capacity.
 */
+ (NXArray *)arrayWithCapacity:(size_t)capacity {
  return [[[NXArray alloc] initWithCapacity:capacity] autorelease];
}

/**
 * @brief Returns a new NXArray instance with the specified objects.
 */
+ (NXArray *)arrayWithObjects:(id<RetainProtocol>)firstObject, ... {
  va_list args;
  va_start(args, firstObject);

  // Create the array instance using the helper method
  NXArray *array = [[[NXArray alloc] _initWithFirstObject:firstObject
                                                arguments:args] autorelease];

  va_end(args);

  return array;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

- (BOOL)_setCapacity:(size_t)cap {
  if (cap < _length) {
    // Minimum capacity is _length
    return NO;
  }
  if (cap == _cap) {
    // No change needed
    return YES;
  }
  if (cap == NXNotFound) {
    // Fail if maximum capacity is reached
    return NO;
  }

  // Allocate new memory for the larger or smaller capacity
  void **data = sys_malloc(cap * sizeof(void *));
  if (data == NULL) {
    // Allocation failed
    return NO;
  }

  // Copy existing data to the new array, release old data
  if (_data != NULL) {
    sys_memcpy(data, _data, _length * sizeof(void *));
    sys_free(_data);
  }

  // Set the new data pointer and capacity
  _data = data;
  _cap = cap;

  // Return success
  return YES;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Returns the number of elements in the array.
 */
- (unsigned int)count {
  return (unsigned int)_length;
}

/**
 * @brief Returns the capacity of the array.
 */
- (size_t)capacity {
  return _cap;
}

/**
 * @brief Returns the first object in the array.
 */
- (id)firstObject {
  if (_length == 0) {
    return nil;
  }
  return _data[0];
}

/**
 * @brief Returns the last object in the array.
 */
- (id)lastObject {
  if (_length == 0) {
    return nil;
  }
  return _data[_length - 1];
}

/**
 * @brief Returns the object at the specified index.
 */
- (id)objectAtIndex:(unsigned int)index {
  objc_assert(index < _length); // Ensure index is within bounds
  return _data[index];
}

/**
 * @brief Return the JSON string representation of the array.
 */
- (NXString *)JSONString {
  NXString *json = [NXString stringWithCapacity:[self JSONBytes] + 1];

  [json appendCString:"["];

  BOOL firstElement = YES;
  size_t i = 0;
  for (i = 0; i < _length; i++) {
    id object = _data[i];

    // Skip nil objects (this shouldn't happen, but be defensive)
    if (object == nil) {
      continue;
    }

    // Add comma if not the first element
    if (!firstElement) {
      [json appendCString:", "];
    }
    firstElement = NO;

    // If object does not conform to JSONProtocol, use the description
    if (![object conformsTo:@protocol(JSONProtocol)]) {
      object = [object description];
    }

    // Perform the append
    [json append:[object JSONString]];
  }

  [json appendCString:"]"];
  return json;
}

/**
 * @brief Returns the appropriate capacity for the JSON representation.
 */
- (size_t)JSONBytes {
  // Estimate the size based on the length and potential escaping
  size_t cap = 2; // For the brackets []
  size_t i;
  for (i = 0; i < _length; i++) {
    id object = _data[i];

    // Skip nil objects (this shouldn't happen, but be defensive)
    if (object == nil) {
      continue;
    }

    if (![object conformsTo:@protocol(JSONProtocol)]) {
      object = [object description];
    }
    cap += [object JSONBytes] + 2; // +2 for the comma and space
  }
  return cap;
}

/**
 * @brief Returns YES if the collection contains the specified object.
 */
- (BOOL)containsObject:(id)object {
  // Return NO if object is nil
  if (object == nil) {
    return NO;
  }

  size_t i;
  for (i = 0; i < _length; i++) {
    id element = _data[i];

    // Skip nil elements (defensive programming)
    if (element == nil) {
      continue;
    }

    // Check for direct pointer equality first
    if (element == object) {
      return YES;
    }

    // Check if element is a collection and recursively search it
    if ([element conformsTo:@protocol(CollectionProtocol)]) {
      if ([element containsObject:object]) {
        return YES;
      }
    }
  }
  return NO;
}

/**
 * @brief Appends an object to the end of the array.
 * @param object The object to append to the array.
 * @return YES if the object was successfully appended, NO otherwise.
 */
- (BOOL)append:(id<RetainProtocol, ObjectProtocol>)object {
  objc_assert(object);

  // We cannot insert if we're trying to insert 'self' because
  // that would create a circular reference
  if (object == self) {
    return NO;
  } else if ([object conformsTo:@protocol(CollectionProtocol)]) {
    if ([(id<CollectionProtocol>)object containsObject:self]) {
      return NO; // Prevent inserting collections directly
    }
  }

  // Check if we need to grow the capacity
  if (_length >= _cap) {
    // Grow capacity by 1.5x to be conservative, with special cases for small
    // values
    size_t cap = _cap < 2 ? _cap + 1 : _cap + (_cap >> 1);
    if ([self _setCapacity:cap] == NO) {
      return NO; // Failed to increase capacity
    }
  }

  // Insert the object at the end
  objc_assert(_length < _cap); // Ensure we have space
  _data[_length] = [object retain];
  _length++;

  // Return YES to indicate success
  return YES;
}

/**
 * @brief Inserts an object at the specified index in the array.
 */
- (BOOL)insert:(id<RetainProtocol, ObjectProtocol>)object
       atIndex:(unsigned int)index {
  objc_assert(object);
  objc_assert(index <= _length); // Ensure index is within bounds

  // We cannot insert if we're trying to insert 'self' because
  // that would create a circular reference
  if (object == self) {
    return NO;
  } else if ([object conformsTo:@protocol(CollectionProtocol)]) {
    if ([(id<CollectionProtocol>)object containsObject:self]) {
      return NO; // Prevent inserting collections directly
    }
  }

  // Check if we need to grow the capacity
  if (_length >= _cap) {
    // Grow capacity by 1.5x to be conservative, with special cases for small
    // values
    size_t cap = _cap < 2 ? _cap + 1 : _cap + (_cap >> 1);
    if ([self _setCapacity:cap] == NO) {
      return NO; // Failed to increase capacity
    }
  }

  // Shift all elements from index to the right by one position
  if (index < _length) {
    sys_memmove(&_data[index + 1], &_data[index],
                (_length - index) * sizeof(void *));
  }

  // Insert the object at the specified index
  objc_assert(_length < _cap); // Ensure we have space
  _data[index] = [object retain];
  _length++;

  // Return YES to indicate success
  return YES;
}

/**
 * @brief Returns the index for the specified object.
 * @param object The object to find in the array.
 * @return The index of the object in the array, or NXNotFound if the object
 * is not found.
 */
- (unsigned int)indexForObject:(id<ObjectProtocol>)object {
  objc_assert(object);

  unsigned int i;
  for (i = 0; i < _length; i++) {
    id element = (id)_data[i];
    if ([element isEqual:object]) {
      return i;
    }
  }
  return NXNotFound;
}

@end
