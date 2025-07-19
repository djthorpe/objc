#pragma once
#include <objc/objc.h>
#include <stddef.h>

/**
 * @brief Represents an arena for memory allocation.
 */
typedef struct objc_arena objc_arena_t;

/**
 * @brief Represents an arena allocation
 */
typedef struct objc_arena_alloc objc_arena_alloc_t;

/**
 * @brief Create a new arena with the specified size.
 * @param prev Pointer to the previous arena, or NULL for the first arena.
 * @param size Size of the arena in bytes.
 * @return Pointer to the newly created arena, or NULL on failure.
 */
objc_arena_t *objc_arena_new(objc_arena_t *prev, size_t size);

/**
 * @brief Free all arenas
 * @param arena Pointer to the first arena to free.
 */
void objc_arena_delete(objc_arena_t *arena);

/**
 * @brief Allocate memory from an arena by finding free space.
 * @param arena Pointer to the arena from which to allocate memory.
 * @param size Size of the memory to allocate in bytes.
 * @return Pointer to the allocated memory, or NULL if allocation fails.
 *
 * This function searches the arena for a free block of memory that can
 * accommodate the requested size. If no suitable block is found, it returns
 * NULL.
 *
 * @note It does not search other linked arenas.
 */
void *objc_arena_alloc_inner(objc_arena_t *arena, size_t size);

/**
 * @brief Deallocate memory from an arena by removing it from the linked list.
 * @param arena Pointer to the arena from which to free memory.
 * @param ptr Pointer to the memory to free.
 * @return TRUE if the memory was successfully freed, FALSE if the pointer was
 *         not found in the arena.
 *
 * This function searches the linked list of allocations in the arena for the
 * specified pointer. If found, it removes the allocation from the list and
 * returns TRUE. If the pointer is not found, it returns FALSE.
 *
 * @note It does not search other linked arenas.
 */
BOOL objc_arena_free_inner(objc_arena_t *arena, void *ptr);

/**
 * @brief Walk through an arena and perform an action on each allocation.
 * @param arena Pointer to the arena to walk through
 * @param alloc Pointer to a pointer that will be updated with the next
 * allocation
 *
 * This function iterates through the linked list of allocations in the arena,
 * calling the provided action on each allocation. The action can modify the
 * allocation or perform any other operation. The `alloc` pointer is updated
 * to point to the next allocation in the list. If there are no more
 * allocations, `alloc` will be set to NULL.
 */
void objc_arena_walk_inner(objc_arena_t *arena, objc_arena_alloc_t **alloc);
