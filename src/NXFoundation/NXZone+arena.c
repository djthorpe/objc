#include "NXZone+arena.h"
#include "NXZone+malloc.h"
#include <objc/objc.h>
#include <stddef.h>
#include <stdint.h>

#pragma mark Declarations

static void *objc_arena_find_free_space(struct objc_arena *arena, size_t size);
static BOOL objc_arena_check_collision(struct objc_arena *arena, void *start,
                                       size_t total_size);

#pragma mark Types

/**
 * @brief Represents an arena for memory allocation.
 */
struct objc_arena {
  struct objc_arena *next;
  size_t size;
  struct objc_arena_alloc *head; // First allocation
  struct objc_arena_alloc *tail; // Last allocation
};

/**
 * @brief Represents an allocation in an arena
 */
struct objc_arena_alloc {
  struct objc_arena_alloc *next;
  size_t size;
  void *ptr;
};

// Convenient typedefs
typedef struct objc_arena objc_arena_t;
typedef struct objc_arena_alloc objc_arena_alloc_t;

#pragma mark Lifecycle

/**
 * @brief Create a new arena with the specified size.
 */
objc_arena_t *objc_arena_new(objc_arena_t *prev, size_t size) {
  objc_assert(size > 0);
  objc_assert(prev == NULL || prev->next == NULL);

  size_t total_size = sizeof(struct objc_arena) + size;
  objc_arena_t *arena = __zone_malloc(total_size);
  if (arena) {
    if (prev) {
      prev->next = arena;
    }
    arena->next = NULL;
    arena->size = size;
    arena->head = NULL;
    arena->tail = NULL;
  }
  return arena;
}

/**
 * @brief Free all arenas
 */
void objc_arena_delete(objc_arena_t *arena) {
  objc_assert(arena);
  do {
    objc_arena_t *next = arena->next;
    __zone_free(arena);
    arena = next;
  } while (arena != NULL);
}

#pragma mark Public Functions

/**
 * @brief Allocate memory from an arena by finding free space.
 */
void *objc_arena_alloc_inner(objc_arena_t *arena, size_t size) {
  objc_assert(arena);
  objc_assert(size > 0);

  // Find available space for this allocation
  void *free_location = objc_arena_find_free_space(arena, size);
  if (free_location == NULL) {
    return NULL; // No space available
  }

  // Create the allocation header at the found location
  struct objc_arena_alloc *new_alloc = (struct objc_arena_alloc *)free_location;
  new_alloc->size = size;
  new_alloc->ptr = (void *)(new_alloc + 1);
  new_alloc->next = NULL;

  // Add to the linked list
  if (!arena->head) {
    // First allocation
    arena->head = new_alloc;
    arena->tail = new_alloc;
  } else {
    // Append to the end
    arena->tail->next = new_alloc;
    arena->tail = new_alloc;
  }

  // Return the pointer to the allocated memory
  return new_alloc->ptr;
}

/**
 * @brief Deallocate memory from an arena by removing it from the linked list.
 */
BOOL objc_arena_free_inner(objc_arena_t *arena, void *ptr) {
  objc_assert(arena);

  // NULL pointer, nothing to free
  if (ptr == NULL) {
    return NO;
  }

  // Find the allocation to free by traversing the chain
  struct objc_arena_alloc *current = arena->head;
  struct objc_arena_alloc *prev = NULL;

  while (current) {
    if (current->ptr == ptr) {
      // Remove from linked list
      if (prev) {
        prev->next = current->next;
      } else {
        // This was the head
        arena->head = current->next;
      }

      // Update tail if this was the last allocation
      if (current == arena->tail) {
        arena->tail = prev;
      }

      // The allocation block becomes free space (gap in the list)
      return YES;
    }

    prev = current;
    current = current->next;
  }

  // Allocation not found
  return NO;
}

#pragma mark Private Functions

/**
 * @brief Find free space in the arena that can accommodate the requested size.
 */
static void *objc_arena_find_free_space(struct objc_arena *arena, size_t size) {
  objc_assert(arena);
  objc_assert(size > 0);

  uintptr_t arena_start = (uintptr_t)(arena + 1);
  uintptr_t arena_end = arena_start + arena->size;
  size_t needed = sizeof(struct objc_arena_alloc) + size;
  size_t alignment = sizeof(void *); // Pointer alignment

  // Start from the end and work backwards
  uintptr_t search_end = arena_end - needed;
  uintptr_t aligned_search_end = (search_end / alignment) * alignment;

  for (uintptr_t pos = aligned_search_end; pos >= arena_start;
       pos -= alignment) {

    // Check if this position would collide with any existing allocation
    if (objc_arena_check_collision(arena, (void *)pos, needed) == NO) {
      // Return suitable space
      return (void *)pos;
    }
  }

  return NULL; // No suitable space found
}

/**
 * @brief Check if a memory range collides with any existing allocation.
 */
static BOOL objc_arena_check_collision(struct objc_arena *arena, void *start,
                                       size_t total_size) {
  objc_assert(arena);

  // No allocations, no collision
  if (arena->head == NULL) {
    return NO;
  }

  struct objc_arena_alloc *current = arena->head;
  uintptr_t range_start = (uintptr_t)start;
  uintptr_t range_end = range_start + total_size;

  while (current) {
    uintptr_t alloc_start = (uintptr_t)current;
    uintptr_t alloc_end =
        alloc_start + sizeof(struct objc_arena_alloc) + current->size;

    // Check if ranges overlap
    if (range_start < alloc_end && range_end > alloc_start) {
      return YES; // Collision detected
    }

    current = current->next;
  }

  return NO; // No collision
}

void objc_arena_walk_inner(objc_arena_t *arena, objc_arena_alloc_t **alloc) {
  objc_assert(arena);
  objc_assert(alloc);

  // Start from the head of the arena
  if (arena->head == NULL) {
    *alloc = NULL; // No allocations to walk
    return;
  }

  // If alloc is NULL, return the first allocation
  if (*alloc == NULL) {
    *alloc = arena->head;
  } else {
    *alloc = (*alloc)->next;
  }
}