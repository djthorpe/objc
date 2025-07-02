#pragma once

/*
 * Memory management for NXFoundation
 */
@interface NXZone : NXObject {
    size_t _size; // Size of the zone, or 0 if the size is not limited
    void* _data; // Pointer to the allocated memory block
}

// Lifecycle
+(id) defaultZone; // Returns the default zone
+(id) zoneWithSize:(size_t)size; // Creates a new zone with the specified size
-(void) dealloc; // Deallocates the zone

// Methods
-(void* ) alloc:(size_t)size; // Allocates memory of the specified size
-(void) free:(void* )ptr; // Frees the allocated memory
-(void* ) realloc:(void* )ptr size:(size_t)size; // Reallocates memory to the specified size and returns the new pointer, or NULL if the reallocation fails
-(void) reset; // Resets the zone, freeing all allocated memory

@end