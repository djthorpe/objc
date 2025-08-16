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
 * @brief Return next arena in the linked list.
 * @details This function returns the next arena in the linked list of arenas.
 * @param arena Pointer to the current arena.
 * @return Pointer to the next arena, or NULL if there are no more arenas.
 */
objc_arena_t *objc_arena_next(objc_arena_t *arena);

/**
 * @brief Get the total size of an arena.
 * @param arena Pointer to the arena to query.
 * @return The total size of the arena in bytes.
 */
size_t objc_arena_stats_size(objc_arena_t *arena);

/**
 * @brief Get the amount of memory currently used in an arena.
 * @param arena Pointer to the arena to query.
 * @return The number of bytes currently allocated from the arena.
 *
 * This function calculates and returns the number of bytes currently
 * allocated and in use within the memory arena. This includes both
 * the user data and the allocation header overhead.
 */
size_t objc_arena_stats_used(objc_arena_t *arena);

/**
 * @brief Get the amount of free space remaining in an arena.
 * @param arena Pointer to the arena to query.
 * @return The number of bytes available for allocation in the arena.
 *
 * This function calculates and returns the number of bytes available
 * for allocation in the specified arena. Due to fragmentation, it may
 * not mean an allocation of this size can be created.
 */
size_t objc_arena_stats_free(objc_arena_t *arena);

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
 * @brief Deallocate memory from a chain of arenas.
 * @param arena Pointer to the first arena in the chain to search.
 * @param ptr Pointer to the memory to deallocate.
 * @return YES if the memory was successfully freed, NO if the pointer was
 *         not found in any arena in the chain.
 *
 *  This function searches through a linked chain of arenas to find
 *  and deallocate the specified memory pointer. It will traverse
 *  all linked arenas until the pointer is found and freed.
 */
BOOL objc_arena_free(objc_arena_t *arena, void *ptr);

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

/**
 * @brief Get the size of an allocation.
 * @param alloc Pointer to the allocation to get the size of.
 * @param ptr Pointer to a pointer that will be updated with the user data
 * pointer, if not NULL.
 * @return The size of the allocation in bytes.
 *
 * This function returns the size of the specified allocation.
 */
size_t objc_arena_alloc_size(objc_arena_alloc_t *alloc, void **ptr);
