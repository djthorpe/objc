/**
 * @file malloc.h
 * @brief Defines external memory management functions.
 * @details This file declares the `objc_malloc` and `objc_free` functions, which
 * are used for memory allocation and deallocation within the Objective-C runtime.
 */
#pragma once
#include <stddef.h>

/**
 * @brief Allocates a block of memory.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or `NULL` if the allocation fails.
 */
extern void* objc_malloc(size_t size);

/**
 * @brief Frees a block of memory.
 * @param ptr A pointer to the memory block to be deallocated.
 */
extern void objc_free(void* ptr);
