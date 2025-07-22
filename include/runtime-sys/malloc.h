/**
 * @file malloc.h
 * @brief Defines memory management functions.
 *
 * This file declares various system methods for managing memory.
 */
#pragma once
#include <stddef.h>

/**
 * @brief Allocates a block of memory.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or `NULL` if the allocation fails.
 */
extern void *sys_malloc(size_t size);

/**
 * @brief Frees a block of memory.
 * @param ptr A pointer to the memory block to be deallocated.
 */
extern void sys_free(void *ptr);
