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
  if (index >= _length) {
    return nil; // Return nil for out-of-bounds access instead of crashing
  }
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

@end
