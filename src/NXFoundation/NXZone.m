#include <NXFoundation/NXFoundation.h>
#include <stddef.h>
#include <string.h>
#include <sys/sys.h>

// Define the first zone allocated as the default zone
static id defaultZone = nil;

/*
 * Implementation of memory allocation for NXZone.
 */
static inline void *__zone_malloc(size_t size) {
  void *ptr = sys_malloc(size);
#ifdef DEBUG
  NXLog(@"  __zone_malloc: size=%zu => @%p", size, ptr);
#endif
  return ptr;
}

/*
 * Implementation of memory free for NXZone.
 */
static inline void __zone_free(void *ptr) {
#ifdef DEBUG
  NXLog(@"  __zone_free => @%p", ptr);
#endif
  sys_free(ptr);
}

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
  // Calculate aligned size for the object
  size_t alignedObjectSize =
      (class_getInstanceSize(self) + 7) & ~7; // 8-byte alignment

  // Allocate memory for object + data block
  void *memory = __zone_malloc(alignedObjectSize + size);
  if (!memory) {
    return nil;
  } else {
    memset(memory, 0, alignedObjectSize + size);
  }

  // Initialize the object properly
  NXZone *zone = (NXZone *)memory;
  object_setClass(zone, self);

  // Set up instance variables
  zone->_size = size;
  zone->_data = size ? (uint8_t *)memory + alignedObjectSize : NULL;
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
  if (defaultZone == self) {
    defaultZone = nil; // Clear the default zone if this is it
  }
  __zone_free(self); // Free the allocated memory block
}

#pragma mark - Class methods

+ (id)defaultZone {
  return defaultZone;
}

#pragma mark - Instance methods

- (void *)allocWithSize:(size_t)size {
  void *ptr = NULL;
  if (_data == NULL) {
    // No arena - call malloc directly
    ptr = __zone_malloc(size);
  } else {
    panicf("Allocating memory in a zone with an arena is not supported");
  }
  @synchronized(self) {
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
  }
  if (_data == NULL) {
    // No arena - call free directly
    __zone_free(ptr);
  }
}

@end
