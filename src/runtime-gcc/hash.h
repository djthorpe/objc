#pragma once
#include "class.h"

/*
 * Objective-C runtime hash table for selectors.
 * This structure is used to store and manage selectors in the Objective-C
 * runtime.
 */
struct objc_hashitem {
  objc_class_t *cls;  // Pointer to the class that owns this selector
  const char *method; // Key for the selector, typically the selector name
  const char *types;  // Types encoding for the selector
  IMP imp;            // Implementation pointer for the selector
};

/*
 * Initializes the hash table for the Objective-C runtime.
 * This function should be called before using the hash table to ensure it is
 * ready for use.
 */
void __objc_hash_init();

/*
 * Register a new selector in the hash table. This function adds a new method to
 * the hash table, allowing it to be looked up later. Returns nil on failure, or
 * a pointer to the newly registered hash item on success.
 */
struct objc_hashitem *__objc_hash_register(objc_class_t *cls,
                                           const char *method,
                                           const char *types, IMP imp);

/*
 * Lookup an implementation for a method in the hash table
 */
struct objc_hashitem *__objc_hash_lookup(objc_class_t *cls, const char *method,
                                         const char *types);