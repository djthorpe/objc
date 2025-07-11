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

  @synchronized([self class]) {
    // Set the previous pool to the current default pool
    _prev = defaultPool;

    // Initialize the tail to nil
    _tail = nil;

    // Set the current pool as the new default pool
    defaultPool = self;
  }

  // Return success
  return self;
}

- (void)dealloc {
  // Drain the pool before deallocating
  [self drain];

  @synchronized([self class]) {
    // Reset the default pool to the previous one
    defaultPool = _prev;
  }

  // Call superclass dealloc
  [super dealloc];
}

#pragma mark - Class Methods

+ (id)currentPool {
  @synchronized(self) {
    // Return the current autorelease pool
    return defaultPool;
  }
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

  @synchronized(self) {
    // If object already added to a pool, panic
    if (((NXObject *)object)->_next != nil) {
      panicf("Object already added to an autorelease pool: %s",
             object_getClassName(object));
      return;
    }

    ((NXObject *)object)->_next =
        _tail;      // Link the new object to the current tail
    _tail = object; // Update the tail to the new object
  }
}

- (void)drain {
  @synchronized(self) {
    // Release all objects in the current pool, from tail to head
    id cur = _tail;
    while (cur != nil) {
      id next = ((NXObject *)cur)->_next; // Save next pointer before release
      NXLog(@"Autoreleasing object: @%p", cur);

      // Clear the _next pointer before releasing
      ((NXObject *)cur)->_next = nil;
      [((NXObject *)cur) release]; // Release the current object

      cur = next; // Move to the next object
    }

    // Clear the tail pointer
    _tail = nil;
  }
}

@end
