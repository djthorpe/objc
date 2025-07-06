#pragma once
#include <stddef.h>

/*
 * Implementation of memory allocation for NXZone.
 */
void* __zone_malloc(size_t size);

/*
 * Implementation of memory free for NXZone.
 */
void __zone_free(void* ptr);
