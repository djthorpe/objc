#include <NXFoundation/NXFoundation.h>
#include "NXZone+malloc.h"
#include <string.h>

// Define the first zone allocated as the default zone
static id defaultZone = nil;

@implementation NXZone

#pragma mark - Lifecycle

/*
 ** Cannot use [[alloc] init] with NXZone.
 ** These methods are marked as unavailable to prevent misuse.
 */
+(id) alloc {
    return nil;
}

-(id) init {
    return nil;
}

/*
 ** Create a new zone with a specific size in bytes, and set to the
 ** default zone if it's not yet been set.
 */
+(id) zoneWithSize:(size_t)size {
    // Calculate aligned size for the object
    size_t alignedObjectSize = (class_getInstanceSize(self) + 7) & ~7; // 8-byte alignment
    
    // Allocate memory for object + data block
    void* memory = __zone_malloc(alignedObjectSize + size);
    if (!memory) {
        return nil;
    } else {
        memset(memory, 0, alignedObjectSize + size);
    }
    
    // Initialize the object properly
    NXZone* zone = (NXZone* )memory;
    object_setClass(zone, self);    
    
    // Set up instance variables
    zone->_size = size;
    zone->_data = size ? (uint8_t*)memory + alignedObjectSize : NULL;

    // Set the default zone if it hasn't been set yet
    if (!defaultZone) {
        defaultZone = zone;
    }

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

-(void *) allocWithSize:(size_t)size {
    // Allocate memory and return a pointer
    // TODO: Use a more sophisticated memory allocation strategy
    return __zone_malloc(size);
}

-(void) free:(void* )ptr {
    // Deallocate the zone pointed to by ptr
    // TODO: Use a more sophisticated memory release strategy
    __zone_free(ptr);
}

@end
