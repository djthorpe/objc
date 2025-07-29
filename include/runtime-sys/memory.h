/**
 * @file memory.h
 * @brief Memory management.
 * @defgroup SystemMemory Memory Management
 * @ingroup System
 *
 * Managing and manipulating memory.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocates a block of memory.
 * @ingroup SystemMemory
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
extern void *sys_malloc(size_t size);

/**
 * @brief Frees a block of memory.
 * @ingroup SystemMemory
 * @param ptr A pointer to the memory block to be deallocated.
 */
extern void sys_free(void *ptr);

/**
 * @brief Set memory to a specific value
 * @ingroup SystemMemory
 * @param ptr Pointer to the memory block
 * @param value Value to set each byte to
 * @param size Number of bytes to set
 * @return Pointer to the memory block, or NULL on error
 */
extern void *sys_memset(void *ptr, uint8_t value, size_t size);

/**
 * @brief Copy memory from source to destination
 * @ingroup SystemMemory
 * @param dest Destination memory block
 * @param src Source memory block
 * @param size Number of bytes to copy
 * @return Pointer to destination, or NULL on error
 */
extern void *sys_memcpy(void *dest, const void *src, size_t size);

/**
 * @brief Move memory from source to destination (handles overlapping regions)
 * @ingroup SystemMemory
 * @param dest Destination memory block
 * @param src Source memory block
 * @param size Number of bytes to move
 * @return Pointer to destination, or NULL on error
 */
extern void *sys_memmove(void *dest, const void *src, size_t size);

/**
 * @brief Compare two memory blocks
 * @ingroup SystemMemory
 * @param ptr1 First memory block
 * @param ptr2 Second memory block
 * @param num Number of bytes to compare
 * @return 0 if equal, negative if ptr1 < ptr2, positive if ptr1 > ptr2
 */
extern int sys_memcmp(const void *ptr1, const void *ptr2, size_t num);

#ifdef __cplusplus
}
#endif
