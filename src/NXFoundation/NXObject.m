#include <NXFoundation/NXFoundation.h>

@implementation NXObject

#pragma mark - Lifecycle

/**
 * Allocates a new instance of object in the default memory zone.
 */
+(id) alloc {
    return [self allocWithZone:nil];
}

/**
 * Allocates a new instance of object in a specific zone.
 */
+(id) allocWithZone:(NXZone* )zone {
    if (zone == nil) {
        zone = [NXZone defaultZone]; // Use the default zone if none is provided
    }
    id instance = [zone allocWithSize:class_getInstanceSize(self)];
    if (instance) {
        object_setClass(instance, self);
        ((NXObject* )instance)->_zone = zone; // Set the zone for the instance
        ((NXObject* )instance)->_retain = 1; // Retain the instance
    }
    return instance;
}

/**
  * Deallocate the object, freeing its memory.
  */
-(void) dealloc {
    if (!_zone) {
        panicf("Object dealloc called without a zone");
    }
    if(_retain != 0) {
        panicf("[NXObject dealloc] called with retain count %d", _retain);
    }
    [_zone free:self]; // Free the memory in the zone        
}

#pragma mark - Retain and Release methods

/**
 * @brief Increases the retain count of the receiver.
 */
-(id) retain {
    // TODO: Implement mutex
    _retain++;
    return self;
}

/**
 * @brief Decreases the retain count of the receiver.
 */
-(void) release {
    // TODO: Implement mutex
    _retain--;
    if (_retain < 0) {
        panicf("[NXObject release] called with retain count below zero");
    }
    if (_retain == 0) {
        [self dealloc];
    }
}

@end
