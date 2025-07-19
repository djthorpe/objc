#include <NXFoundation/NXFoundation.h>
#include <objc/objc.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tests/tests.h>
#include <time.h>

typedef struct objc_arena {
  struct objc_arena *next;
  size_t size;
  struct objc_arena_alloc *head; // First allocation
  struct objc_arena_alloc *tail; // Last allocation
} objc_arena_t;

typedef struct objc_arena_alloc {
  struct objc_arena_alloc *next;
  size_t size;
  void *ptr;
} objc_arena_alloc_t;

#define __zone_malloc(size) malloc((size))
#define __zone_free(ptr) free((ptr))
#define objc_assert(cond)                                                      \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #cond,       \
              __FILE__, __LINE__);                                             \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

// Function declarations
static void *objc_arena_find_free_space(struct objc_arena *arena, size_t size);
static size_t objc_arena_calculate_free_space(struct objc_arena *arena);
static BOOL objc_arena_check_collision(struct objc_arena *arena, void *start,
                                       size_t size);

/**
 * @brief Create a new arena with the specified size.
 */
struct objc_arena *objc_arena_new(struct objc_arena *prev, size_t size) {
  objc_assert(size > 0);
  objc_assert(prev == NULL || prev->next == NULL);

  size_t total_size = sizeof(struct objc_arena) + size;
  struct objc_arena *arena = __zone_malloc(total_size);
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
void objc_arena_delete(struct objc_arena *arena) {
  objc_assert(arena);
  do {
    struct objc_arena *next = arena->next;
    __zone_free(arena);
    arena = next;
  } while (arena != NULL);
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
 * @brief Allocate memory from a specific arena by finding free space.
 */
void *objc_arena_alloc(struct objc_arena *arena, size_t size) {
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
 * @brief Free memory from a specific arena by removing it from the linked list.
 */
BOOL objc_arena_free(struct objc_arena *arena, void *ptr) {
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
 * @brief Calculate total free space by scanning the arena.
 */
static size_t objc_arena_calculate_free_space(struct objc_arena *arena) {
  objc_assert(arena);

  // Check for no allocations yet
  if (!arena->head) {
    return arena->size;
  }

  uintptr_t arena_start = (uintptr_t)(arena + 1);
  uintptr_t arena_end = arena_start + arena->size;
  size_t free_space = 0;
  size_t scan_granularity = sizeof(void *); // Pointer alignment

  // Scan through arena and count free space
  for (uintptr_t pos = arena_start; pos < arena_end; pos += scan_granularity) {
    size_t remaining = arena_end - pos;
    if (remaining < scan_granularity) {
      // Add remaining bytes at the end
      if (!objc_arena_check_collision(arena, (void *)pos, remaining)) {
        free_space += remaining;
      }
      break;
    }

    if (!objc_arena_check_collision(arena, (void *)pos, scan_granularity)) {
      free_space += scan_granularity;
    }
  }

  return free_space;
}

/**
 * @brief Dump arena state for debugging.
 */
void objc_arena_dump(struct objc_arena *arena) {
  objc_assert(arena);

  int arena_count = 0;
  do {
    struct objc_arena *next = arena->next;
    printf("Arena #%d @%p: size=%zu+%zu, next=@%p\n", arena_count++, arena,
           sizeof(struct objc_arena), arena->size, next);

    struct objc_arena_alloc *alloc = arena->head;
    int alloc_count = 0;
    size_t total_allocated = 0;

    while (alloc) {
      printf("  Allocation #%d @%p: size=%zu+%zu, ptr=@%p, next=@%p\n",
             alloc_count++, alloc, sizeof(struct objc_arena_alloc), alloc->size,
             alloc->ptr, alloc->next);
      total_allocated += alloc->size;
      alloc = alloc->next;
    }

    // Calculate free space by analyzing gaps
    size_t free_space = objc_arena_calculate_free_space(arena);
    size_t used_space = arena->size - free_space;

    printf("  Allocation stats: %d active allocations (%zu bytes)\n",
           alloc_count, total_allocated);
    printf("  Memory usage: %zu/%zu bytes used (%.1f%%), %zu bytes free\n",
           used_space, arena->size, (double)used_space / arena->size * 100.0,
           free_space);
    arena = next;
  } while (arena);
}

int main() {
  printf("Running NXFoundation_10 arena allocator tests...\n");

  // Example: Both struct and typedef approaches work
  struct objc_arena *arena1 = objc_arena_new(NULL, 1024); // Using struct
  objc_arena_t *arena2 = objc_arena_new(NULL, 512);       // Using typedef

  if (arena1) {
    printf("Created arena1 with size: %zu bytes (using struct)\n",
           arena1->size);
  }

  if (arena2) {
    printf("Created arena2 with size: %zu bytes (using typedef)\n",
           arena2->size);
    objc_arena_delete(arena2); // Clean up arena2 early
  }

  // Continue with main test using struct approach
  struct objc_arena *arena = arena1; // Use arena1 for main tests
  if (!arena) {
    printf("Failed to create arena!\n");
    return 1;
  }

  // Test allocation and freeing with random sizes and count
  printf("\n=== Testing Allocation ===\n");

  // Seed random number generator
  srand((unsigned int)time(NULL));

  // Random number of allocations between 3 and 10
  int num_allocs = 3 + rand() % 8;
  printf("Testing with %d allocations\n", num_allocs);

  void *ptrs[10]; // Maximum possible allocations
  size_t sizes[10];

  // Generate random sizes between 16 and 200 bytes
  for (int i = 0; i < num_allocs; i++) {
    sizes[i] = 16 + rand() % 185; // 16-200 bytes
    printf("Allocation #%d will be %zu bytes\n", i, sizes[i]);
  }

  for (int i = 0; i < num_allocs; i++) {
    printf("Allocating %zu bytes...", sizes[i]);
    ptrs[i] = objc_arena_alloc(arena, sizes[i]);
    if (ptrs[i]) {
      printf(" SUCCESS (ptr=@%p)\n", ptrs[i]);
      // Write test pattern
      memset(ptrs[i], 0xAA + i, sizes[i]);
    } else {
      printf(" FAILED\n");
    }
  }

  printf("\nArena state after allocations:\n");
  objc_arena_dump(arena);

  // Test freeing random allocation (middle-ish)
  printf("\n=== Testing Free (random allocation) ===\n");
  int free_index = num_allocs > 2 ? 1 + rand() % (num_allocs - 2)
                                  : 1; // Avoid first/last if possible
  if (ptrs[free_index]) {
    printf("Freeing allocation #%d (ptr=@%p, size=%zu)...", free_index,
           ptrs[free_index], sizes[free_index]);
    BOOL success = objc_arena_free(arena, ptrs[free_index]);
    printf(" %s\n", success ? "SUCCESS" : "FAILED");
    ptrs[free_index] = NULL;
  }

  printf("\nArena state after freeing random allocation:\n");
  objc_arena_dump(arena);

  // Test freeing another random allocation
  printf("\n=== Testing Free (another random allocation) ===\n");
  int free_index2;
  do {
    free_index2 = rand() % num_allocs;
  } while (ptrs[free_index2] == NULL ||
           free_index2 == free_index); // Find a different, non-freed allocation

  if (ptrs[free_index2]) {
    printf("Freeing allocation #%d (ptr=@%p, size=%zu)...", free_index2,
           ptrs[free_index2], sizes[free_index2]);
    BOOL success = objc_arena_free(arena, ptrs[free_index2]);
    printf(" %s\n", success ? "SUCCESS" : "FAILED");
    ptrs[free_index2] = NULL;
  }

  printf("\nArena state after freeing second allocation:\n");
  objc_arena_dump(arena);

  // Test allocating in freed space with random sizes
  printf("\n=== Testing Allocation in Freed Space ===\n");

  // Try a few random allocations to fill freed space
  int realloc_attempts = 2 + rand() % 3; // 2-4 attempts
  printf("Attempting %d reallocations in freed space\n", realloc_attempts);

  for (int i = 0; i < realloc_attempts; i++) {
    size_t realloc_size = 8 + rand() % 100; // 8-107 bytes
    void *new_ptr = objc_arena_alloc(arena, realloc_size);
    if (new_ptr) {
      printf("Reallocation #%d: Successfully allocated %zu bytes (ptr=@%p)\n",
             i, realloc_size, new_ptr);
      memset(new_ptr, 0xF0 + i, realloc_size);
    } else {
      printf("Reallocation #%d: Failed to allocate %zu bytes\n", i,
             realloc_size);
    }
  }

  printf("\nFinal arena state:\n");
  objc_arena_dump(arena);

  // Test invalid free
  printf("\n=== Testing Invalid Free ===\n");
  void *invalid_ptr = malloc(100);
  printf("Attempting to free invalid pointer...");
  BOOL result = objc_arena_free(arena, invalid_ptr);
  printf(" %s\n", result ? "UNEXPECTED SUCCESS" : "CORRECTLY FAILED");
  free(invalid_ptr);

  // Delete all arenas
  printf("\nCleaning up...\n");
  objc_arena_delete(arena);
  printf("Test completed successfully!\n");

  return 0;
}
