#pragma once
#include <stddef.h>

/*
 ** External memory management function to allocate memory
 */
extern void* objc_malloc(size_t size);

/*
 ** External memory management functions to free allocated memory
 */
extern void objc_free(void* ptr);
