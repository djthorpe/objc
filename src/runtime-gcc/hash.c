#include "hash.h"
#include <objc/objc.h>
#include <string.h>
#include <runtime-sys/sys.h>

#define HASH_TABLE_SIZE 512
struct objc_hashitem hash_table[HASH_TABLE_SIZE + 1];

static size_t __objc_hash_compute(objc_class_t *cls, const char *method,
                                  const char *types);

static BOOL __objc_hash_match(struct objc_hashitem *item, objc_class_t *cls,
                              const char *method, const char *types);

///////////////////////////////////////////////////////////////////////////////

void __objc_hash_init() {
  static BOOL init = NO;
  if (init) {
    return; // Already initialized
  }
  init = YES;

  // Initialize the hash table
  for (int i = 0; i <= HASH_TABLE_SIZE; i++) {
    hash_table[i].cls = NULL;
  }
}

/*
 * Register a new method in the hash table
 */
struct objc_hashitem *__objc_hash_register(objc_class_t *cls,
                                           const char *method,
                                           const char *types, IMP imp) {
  size_t hash = __objc_hash_compute(cls, method, types);
  size_t index = hash;
  while (hash_table[hash].cls != NULL) {
    // It we have matched the item exactly, then replace it
    if (__objc_hash_match(&hash_table[hash], cls, method, types)) {
      // Replace the existing item with the new implementation
      struct objc_hashitem *item = &hash_table[hash];
      item->imp = imp; // Update the implementation pointer
      return item;     // Return the updated item
    }

    // Collision detected, find the next available slot
    hash = (hash + 1) % HASH_TABLE_SIZE;
    if (hash == index) {
      return NULL;
    }
  }

  // Insert the new hash item
  struct objc_hashitem *item = &hash_table[hash];
  item->cls = cls;
  item->method = method;
  item->types = types;
  item->imp = imp;

  // Return the newly registered item
  return item;
}

/*
 * Return YES if the hash item matches the class, method, and types
 */
inline static BOOL __objc_hash_match(struct objc_hashitem *item,
                                     objc_class_t *cls, const char *method,
                                     const char *types) {
  // Basic class and method name matching
  if (item->cls != cls || item->method == NULL || method == NULL ||
      strcmp(item->method, method) != 0) {
    return NO;
  }
  
  // If both types are NULL, consider it a match (for selectors without type info)
  if (item->types == NULL && types == NULL) {
    return YES;
  }
  
  // If one is NULL but not the other, it's a partial match for @selector() lookups
  // Allow matching when types is NULL (from @selector) but item->types is not
  if (types == NULL && item->types != NULL) {
    return YES; // Allow @selector() to match methods with type encoding
  }
  
  // If both are non-NULL, they must match exactly
  if (item->types != NULL && types != NULL) {
    return strcmp(item->types, types) == 0;
  }
  
  return NO;
}

/*
 * Lookup an implementation for a method in the hash table
 */
struct objc_hashitem *__objc_hash_lookup(objc_class_t *cls, const char *method,
                                         const char *types) {
  size_t hash = __objc_hash_compute(cls, method, types);
  size_t index = hash;
  while (hash_table[hash].cls != NULL) {
    // Check if the class, method, and types match
    if (__objc_hash_match(&hash_table[hash], cls, method, types)) {
      // Found a match
      return &hash_table[hash];
    }
    // Increment the hash to check the next item
    hash = (hash + 1) % HASH_TABLE_SIZE;
    if (hash == index) {
      return NULL;
    }
  }

  // If we exited the loop because we found an empty slot, method not found
  return NULL;
}

static size_t __objc_hash_compute(objc_class_t *cls, const char *method,
                                  const char *types) {
  size_t hash = 0;
  for (const char *p = cls->name; *p; p++) {
    hash = (hash * 31) + *p;
  }
  if (cls->info & objc_class_flag_meta) {
    hash += 0x10000; // Meta class flag
  }
  if (method != NULL) {
    for (const char *p = method; *p; p++) {
      hash = (hash * 31) + *p;
    }
  }
  if (types != NULL) {
    for (const char *p = types; *p; p++) {
      hash = (hash * 31) + *p;
    }
  }
  return hash % HASH_TABLE_SIZE;
}
