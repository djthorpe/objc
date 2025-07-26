/**
 * @file malloc.h
 * @brief Memory allocation functions for the Objective-C runtime.
 */
#pragma once
#include <stddef.h>

/**
 * @brief Allocate memory for use by the Objective-C runtime.
 * @ingroup objc  
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory block, or NULL if allocation fails.
 */
void *objc_malloc(size_t size);

/**
 * @brief Free memory previously allocated by objc_malloc().
 * @ingroup objc  
 * @param ptr A pointer to the memory block to be deallocated.
 */
void objc_free(void *ptr);
