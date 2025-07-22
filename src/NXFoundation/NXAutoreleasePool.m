#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

// Define the current autorelease pool class
static id defaultPool = nil;

@implementation NXAutoreleasePool

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

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

///////////////////////////////////////////////////////////////////////////////
// CLASS METHODS

+ (id)currentPool {
  @synchronized(self) {
    // Return the current autorelease pool
    return defaultPool;
  }
}

///////////////////////////////////////////////////////////////////////////////
// INSTANCE METHODS

- (void)addObject:(id)object {
  if (object == nil) {
    return;
  }
  if (!object_isKindOfClass(object, [NXObject class])) {
    sys_panicf(
        "Attempting to add a non-NXObject instance to the autorelease pool: %s",
        object_getClassName(object));
    return;
  }

  @synchronized(self) {
    // If object already added to a pool, panic
    if (((NXObject *)object)->_next != nil) {
      sys_panicf("Object already added to an autorelease pool: %s",
                 object_getClassName(object));
      return;
    }
#ifdef DEBUG
    NXLog(@"  autorelease retain: [%s] @%p", object_getClassName(object),
          object);
#endif
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
#ifdef DEBUG
      NXLog(@"  autorelease release: [%s] @%p", object_getClassName(cur), cur);
#endif
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
