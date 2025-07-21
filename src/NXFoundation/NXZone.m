#include "NXZone+arena.h"
#include <NXFoundation/NXFoundation.h>
#include <string.h>
#include <sys/sys.h>

// Define the first zone allocated as the default zone
static id defaultZone = nil;

@implementation NXZone

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @note Cannot use [[alloc] init] with NXZone.
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
  zone->_root = zone->_cur = arena;
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
    sys_panicf("[NXZone dealloc] called with %zu active allocations", _count);
  }
#endif
  // Clear the default zone if this is it
  if (defaultZone == self) {
    defaultZone = nil;
  }

  // Free the linked list of arenas associated with this zone
  objc_arena_delete((objc_arena_t *)_root);

  // Call superclass dealloc
  [super dealloc];
}

///////////////////////////////////////////////////////////////////////////////
// CLASS METHODS

+ (id)defaultZone {
  return defaultZone;
}

///////////////////////////////////////////////////////////////////////////////
// INSTANCE METHODS

- (void *)allocWithSize:(size_t)size {
  void *ptr = NULL;
  @synchronized(self) {
    // Allocate memory from the current arena
    ptr = objc_arena_alloc_inner((objc_arena_t *)_cur, size);
    if (ptr == NULL) {
      // If allocation fails, attempt to create a new arena
      objc_arena_t *new =
          objc_arena_new((objc_arena_t *)_cur, size > _size ? size : _size);
      if (new) {
        _cur = new; // Update current arena to the new one
        ptr = objc_arena_alloc_inner(new, size);
      }
    }
#ifdef DEBUG
    NXLog(@"  allocWithSize: size=%zu => @%p", size, ptr);
#endif
    // Increment the allocation count
    if (ptr != NULL) {
      NXLog(@"  COUNT++");
      if (_count == SIZE_MAX) {
        sys_panicf("[NXZone allocWithSize] count overflow");
      } else {
        // Increment the allocation count
        _count++;
      }
    }
    return ptr;
  }
}

- (void)free:(void *)ptr {
  if (ptr == NULL) {
    return;
  }
  @synchronized(self) {
#ifdef DEBUG
    NXLog(@"  free: @%p", ptr);
#endif
    // Free the memory back to the arena
    BOOL success = objc_arena_free((objc_arena_t *)_root, ptr);
    if (success) {
      NXLog(@"  COUNT--");
      _count--;
    } else {
      sys_panicf("[NXZone free] failed to free memory @%p", ptr);
    }
  }
}

- (void)dump {
  objc_arena_t *cur = (objc_arena_t *)_root;
  objc_arena_alloc_t *alloc = NULL;
  void *ptr = NULL;
  NXLog(@"Zone dump for -[NXZone] @%p:", self);
  size_t size = 0;
  size_t used = 0;
  size_t free = 0;
  while (cur != NULL) {
    size += objc_arena_stats_size(cur);
    used += objc_arena_stats_used(cur);
    free += objc_arena_stats_free(cur);
    NXLog(@"  arena @%p size=%zu used=%zu free=%zu:", cur,
          objc_arena_stats_size(cur), objc_arena_stats_used(cur),
          objc_arena_stats_free(cur));
    do {
      objc_arena_walk_inner(cur, &alloc);
      if (alloc != NULL) {
        size_t size = objc_arena_alloc_size(alloc, &ptr);
        NXLog(@"    alloc: @%p, size=%zu", ptr, size);
      }
    } while (alloc != NULL);
    cur = objc_arena_next(cur); // Move to the next arena
  }
  NXLog(@"Total size: %zu bytes, used: %zu bytes, free: %zu bytes", size, used,
        free);
}

/**
 * @brief Returns the total size of the memory zone.
 * @return The total size of the zone in bytes.
 *
 * This method returns the total capacity of the memory zone in bytes. Note that
 * this includes both used and free memory, plus the size of the NXZone
 * object itself.
 */
- (size_t)bytesTotal {
  objc_arena_t *cur = (objc_arena_t *)_root;
  size_t size = 0;
  while (cur != NULL) {
    size += objc_arena_stats_size(cur);
    cur = objc_arena_next(cur); // Move to the next arena
  }
  return size;
}

/**
 * @brief Returns the used size of the memory zone.
 * @return The used size of the zone in bytes.
 *
 * This method returns the number of used bytes in the memory zone.
 */
- (size_t)bytesUsed {
  objc_arena_t *cur = (objc_arena_t *)_root;
  size_t used = 0;
  while (cur != NULL) {
    used += objc_arena_stats_used(cur);
    cur = objc_arena_next(cur); // Move to the next arena
  }
  return used;
}

/**
 * @brief Returns the free size of the memory zone.
 * @return The free size of the zone in bytes.
 *
 * This method returns the number of free bytes in the memory zone.
 */
- (size_t)bytesFree {
  objc_arena_t *cur = (objc_arena_t *)_root;
  size_t free = 0;
  while (cur != NULL) {
    free += objc_arena_stats_free(cur);
    cur = objc_arena_next(cur); // Move to the next arena
  }
  return free;
}

@end
