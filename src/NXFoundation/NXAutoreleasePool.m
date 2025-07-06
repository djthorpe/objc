#include <NXFoundation/NXFoundation.h>

// Define the current autorelease pool class
static id defaultPool = nil;

@implementation NXAutoreleasePool

#pragma mark - Lifecycle

- (id)init {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Set the previous pool to the current default pool
  _prev = defaultPool;

  // Initialize the tail to nil
  _tail = nil;

  // Set the current pool as the new default pool
  defaultPool = self;

  // Return success
  return self;
}

- (void)dealloc {
  // Drain the pool before deallocating
  [self drain];

  // Reset the default pool to the previous one
  defaultPool = _prev;

  // Call superclass dealloc
  [super dealloc];
}

#pragma mark - Class Methods

+ (id)currentPool {
  // Return the current autorelease pool
  return defaultPool;
}

#pragma mark - Instance Methods

- (void)addObject:(id)object {
  if (object == nil) {
    return;
  }
  if (!object_isKindOfClass(object, [NXObject class])) {
    panicf(
        "Attempting to add a non-NXObject instance to the autorelease pool: %s",
        object_getClassName(object));
    return;
  }
  ((NXObject* )object)->_next = _tail; // Link the new object to the current tail
  _tail = object;        // Update the tail to the new object
}

- (void)drain {
  // Release all objects in the current pool
  id _head = _tail;
  while (_head != nil) {
    NXLog(@"Autoreleasing object: %@", _head);
    [((NXObject* )_head) release]; // Release the current object
    _head = ((NXObject* )_head)->_next; // Next autoreleased object
  }
}

@end
