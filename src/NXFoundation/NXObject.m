#include <NXFoundation/NXFoundation.h>

@implementation NXObject

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Allocates a new instance of object in the default memory zone.
 */
+ (id)alloc {
  return [self allocWithZone:nil];
}

/**
 * @brief Allocates a new instance of object in a specific zone.
 */
+ (id)allocWithZone:(NXZone *)zone {
  if (zone == nil) {
    zone = [NXZone defaultZone]; // Use the default zone if none is provided
  }
  id instance = [zone allocWithSize:class_getInstanceSize(self)];
  if (instance) {
    object_setClass(instance, self);
    ((NXObject *)instance)->_zone = zone; // Set the zone for the instance
    ((NXObject *)instance)->_retain = 1;  // Retain the instance
  }
  return instance;
}

/**
 * @brief Deallocate the object, freeing its memory.
 * @warning This method intentionally does NOT call [super dealloc] to prevent
 * double-free.
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-missing-super-calls"
- (void)dealloc {
  // If this is an NXZone class, then we simply return, since the zone will
  // handle deallocation.
  if (object_isKindOfClass(self, [NXZone class])) {
    return;
  }
  if (!_zone) {
    panicf("Object dealloc called without a zone");
  }
  if (_retain != 0) {
    panicf("[NXObject dealloc] called with retain count %d", _retain);
  }
  if (self != _zone) {
    [_zone free:self]; // Free the memory in the zone
  }
}
#pragma clang diagnostic pop

///////////////////////////////////////////////////////////////////////////////
// INSTANCE METHODS

/**
 * @brief Increases the retain count of the receiver.
 */
- (id)retain {
  @synchronized(self) {
    _retain++;
  }
  return self;
}

/**
 * @brief Decreases the retain count of the receiver.
 */
- (void)release {
  @synchronized(self) {
    if (_retain == 0) {
      panicf("[NXObject release] called with retain count of zero");
    }
    _retain--;
    if (_retain == 0) {
      [self dealloc];
    }
  }
}

/**
 * @brief Adds the receiver to the autorelease pool.
 */
- (id)autorelease {
  NXAutoreleasePool *pool = [NXAutoreleasePool currentPool];
  if (pool == nil) {
    panicf("[NXObject autorelease] called without an active autorelease pool");
    return nil;
  }
  [pool addObject:self]; // Add the object to the current autorelease pool
  return self;
}

@end
