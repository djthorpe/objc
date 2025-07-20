#include "NXZone+arena.h"
#include "NXZone+malloc.h"
#include <objc/objc.h>
#include <stddef.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// DECLARATIONS

static void *objc_arena_find_free_space(struct objc_arena *arena, size_t size);
static BOOL objc_arena_check_collision(struct objc_arena *arena, void *start,
                                       size_t total_size);

///////////////////////////////////////////////////////////////////////////////
// TYPES

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

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Create a new arena with the specified size.
 */
objc_arena_t *objc_arena_new(objc_arena_t *root, size_t size) {
  objc_assert(size > 0);

  // Go to the end of the arena chain
  objc_arena_t *prev = root;
  while (prev && prev->next) {
    prev = prev->next;
  }

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

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

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

/**
 * @brief Deallocate memory from arenas
 */
BOOL objc_arena_free(objc_arena_t *arena, void *ptr) {
  objc_assert(arena);
  if (ptr == NULL) {
    return NO; // Nothing to free
  }
  while (arena != NULL) {
    // Try to free in the current arena
    if (objc_arena_free_inner(arena, ptr)) {
      return YES; // Successfully freed
    }
    arena = arena->next; // Move to the next arena
  }
  return NO; // Pointer not found in any arena
}

/**
 * @brief Walk through an arena and perform an action on each allocation.
 */
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

/**
 * @brief Get the size of an allocation.
 */
size_t objc_arena_alloc_size(objc_arena_alloc_t *alloc, void **ptr) {
  // Where alloc is NULL, return 0 and set ptr to NULL
  if (alloc == NULL) {
    if (ptr != NULL) {
      *ptr = NULL;
    }
    return 0;
  }
  // Get the size from the allocation structure
  if (ptr != NULL) {
    *ptr = alloc->ptr;
  }
  return alloc->size;
}

objc_arena_t *objc_arena_next(objc_arena_t *arena) {
  objc_assert(arena);
  return arena->next; // Return the next arena in the linked list
}

/**
 * @brief Get the total size of an arena.
 */
size_t objc_arena_stats_size(objc_arena_t *arena) {
  objc_assert(arena);
  return sizeof(struct objc_arena) +
         arena->size; // Return the size, including the header
}

/**
 * @brief Get the amount of used space in an arena.
 */
size_t objc_arena_stats_used(objc_arena_t *arena) {
  objc_assert(arena);

  // Calculate used space by walking through active allocations
  size_t used = 0;
  struct objc_arena_alloc *current = arena->head;
  while (current) {
    used += sizeof(struct objc_arena_alloc) + current->size;
    current = current->next;
  }

  return used;
}

/**
 * @brief Get the amount of free space remaining in an arena.
 */
size_t objc_arena_stats_free(objc_arena_t *arena) {
  objc_assert(arena);

  // Return remaining space, accounting for potential fragmentation
  size_t used = objc_arena_stats_used(arena);
  return (arena->size > used) ? (arena->size - used) : 0;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

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

  for (intptr_t pos = (intptr_t)aligned_search_end;
       pos >= (intptr_t)arena_start; pos -= (intptr_t)alignment) {

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
