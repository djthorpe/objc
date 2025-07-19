#include "NXZone+arena.h"
#include <NXFoundation/NXFoundation.h>
#include <string.h>

// Define the first zone allocated as the default zone
static id defaultZone = nil;

@implementation NXZone

#pragma mark - Lifecycle

/*
 ** Cannot use [[alloc] init] with NXZone.
 ** These methods are marked as unavailable to prevent misuse.
 */
+ (id)alloc {
  return nil;
}

- (id)init {
  return nil;
}

/**
 * Create a new zone with a specific size in bytes, and set to the
 * default zone if it's not yet been set.
 */
+ (id)zoneWithSize:(size_t)size {
  objc_assert(size > 0);

  // Calculate aligned size for the NXZone object
  size_t alignedObjectSize =
      (class_getInstanceSize(self) + 7) & ~7; // 8-byte alignment

  // Allocate an arena
  objc_arena_t *arena = objc_arena_new(NULL, alignedObjectSize + size);
  if (!arena) {
    return nil;
  }

  // Allocate memory for the zone object
  NXZone *zone = objc_arena_alloc_inner(arena, alignedObjectSize);
  if (zone == NULL) {
    objc_arena_delete(arena);
    return nil;
  }

  // Initialize the object properly
  object_setClass(zone, self);

  // Set up instance variables
  zone->_data = arena;
  zone->_size = alignedObjectSize + size; // Update _size to reflect arena size
  zone->_count = 0;
  zone->_retain = 1; // Initial retain count

  // Set the default zone if it hasn't been set yet
  if (defaultZone == nil) {
    defaultZone = zone;
  }

  return zone;
}

/*
 * Deallocate the zone, freeing the allocated memory.
 */
- (void)dealloc {
#ifdef DEBUG
  if (_count != 0) {
    panicf("[NXZone dealloc] called with %zu active allocations", _count);
  }
#endif
  // Clear the default zone if this is it
  if (defaultZone == self) {
    defaultZone = nil;
  }

  // Free the arena associated with this zone
  objc_arena_delete((objc_arena_t *)_data);

  // Call superclass dealloc
  [super dealloc];
}

#pragma mark - Class methods

+ (id)defaultZone {
  return defaultZone;
}

#pragma mark - Instance methods

- (void *)allocWithSize:(size_t)size {
  void *ptr = NULL;
  @synchronized(self) {
    // Allocate memory from the arena
    ptr = objc_arena_alloc_inner((objc_arena_t *)_data, size);
#ifdef DEBUG
    NXLog(@"  allocWithSize: size=%zu => @%p", size, ptr);
#endif
    // Increment the allocation count
    if (ptr != NULL) {
      _count++;
    }
  }
  return ptr;
}

- (void)free:(void *)ptr {
  @synchronized(self) {
    if (ptr != NULL) {
      _count--;
    }
#ifdef DEBUG
    NXLog(@"  free: @%p", ptr);
#endif
    // Free the memory back to the arena
    objc_arena_free_inner((objc_arena_t *)_data, ptr);
  }
}

@end
