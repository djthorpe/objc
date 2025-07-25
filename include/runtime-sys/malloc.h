/**
 * @file malloc.h
 * @brief Defines memory management functions.
 *
 * This file declares various system methods for managing memory.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Allocates a block of memory.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
extern void *sys_malloc(size_t size);

/**
 * @brief Frees a block of memory.
 * @param ptr A pointer to the memory block to be deallocated.
 */
extern void sys_free(void *ptr);

/**
 * @brief Set memory to a specific value
 * @param ptr Pointer to the memory block
 * @param value Value to set each byte to
 * @param size Number of bytes to set
 * @return Pointer to the memory block, or NULL on error
 */
extern void *sys_memset(void *ptr, uint8_t value, size_t size);

/**
 * @brief Copy memory from source to destination
 * @param dest Destination memory block
 * @param src Source memory block
 * @param size Number of bytes to copy
 * @return Pointer to destination, or NULL on error
 */
extern void *sys_memcpy(void *dest, const void *src, size_t size);
