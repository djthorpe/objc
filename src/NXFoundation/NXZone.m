#include <NXFoundation/NXFoundation.h>
#include "NXZone+malloc.h"

// Define the first zone allocated as the default zone
static id defaultZone = nil;

@implementation NXZone

#pragma mark - Lifecycle

/*
 ** Cannot use [[alloc] init] with NXZone.
 ** These methods are marked as unavailable to prevent misuse.
 */
+(id) alloc __attribute__((unavailable("Use +zoneWithSize: instead of +alloc."))) {
    return nil;
}

-(id) init __attribute__((unavailable("Use +zoneWithSize: instead of -init."))) {
    return nil;
}

/*
 ** Create a new zone with a specific size in bytes, and set to the
 ** default zone if it's not yet been set.
 */
+(id) zoneWithSize:(size_t)size {
    // Create a new zone with the size of the zone instance, plus the size of the data block
    NXZone* zone = (NXZone* )__zone_malloc(class_getInstanceSize(self) + size);
    if (zone) {
        // Set the class of the allocated memory to NXZone        
        zone->_data = size ? (void* )zone + class_getInstanceSize(self) : NULL;
        zone->_size = size;
    } else {
        return nil;
    }

    // Set the default zone if it hasn't been set yet
    if (!defaultZone) {
        defaultZone = zone;
    }

    // Return the new zone instance
    return zone;
}

/*
 ** Deallocate the zone, freeing the allocated memory.
 */
-(void) dealloc {
    if (defaultZone == self) {
        defaultZone = nil; // Clear the default zone if this is it
    }
    __zone_free(self); // Free the allocated memory block
}

#pragma mark - Properties

+(id) defaultZone {
    return defaultZone;
}

#pragma mark - Methods

+(id) alloc:(size_t)size {
    // Allocate a new zone with the specified size
    NXLog(@"TODO: Allocating zone with size: %zu", size);
    return __zone_malloc(size);
}

+(id) free:(void* )ptr {
    // Deallocate the zone pointed to by ptr
    NXLog(@"TOOD: Freee zone with @%p", ptr);
    __zone_free(ptr);
}

@end
