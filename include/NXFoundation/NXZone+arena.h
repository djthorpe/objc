/**
 * @file NXZone+arena.h
 * @brief Defines NXZone arenas for memory management.
 */
#pragma once

/**
 * @brief Arena memory structure for bulk allocation management.
 *
 * An arena represents a contiguous block of memory that can be used
 * for allocations. The arena structure itself is stored at the
 * beginning of the allocated block, followed immediately by the usable memory
 * area.
 */
typedef struct objc_arena objc_arena_t;

/**
 * @brief Creates a new arena with the specified size.
 * @param size The size in bytes of usable memory to allocate (must be > 0)
 * @return Pointer to the new arena structure on success, NULL on failure
 *
 * Allocates a contiguous block of memory consisting of the arena
 * structure followed by the requested amount of usable memory.
 */
objc_arena_t *objc_arena_new(size_t size);

/**
 * @brief Frees an arena and all its associated memory.
 */
void objc_arena_free(objc_arena_t *arena);
