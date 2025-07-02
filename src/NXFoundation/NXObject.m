#include <NXFoundation/NXFoundation.h>

@implementation NXObject

#pragma mark - Lifecycle

/**
 * Allocates a new instance of object in a specific zone.
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
        _zone = zone; // Store the zone in the instance
    }
    return instance;
}

/**
  * Deallocate the object, freeing its memory.
  */
-(void) dealloc {
    if (_zone) {
        [_zone free:self]; // Free the memory in the zone        
    }
}

@end
