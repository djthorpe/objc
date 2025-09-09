#include "api.h"
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////

/*
 * Dispatch table size and mask
 */
#define DTABLE_SIZE 64 // Should be power of 2
#define DTABLE_MASK (DTABLE_SIZE - 1)

///////////////////////////////////////////////////////////////////////////////

/*
 * Convert a pointer to a hash value in the range [0..DTABLE_SIZE-1]
 */
static inline uint32_t _objc_dtable_hash_ptr(void *sel_id) {
  uintptr_t p = (uintptr_t)sel_id;
  return (uint32_t)((p >> 3) ^ (p >> 12)) & DTABLE_MASK;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC API

/*
 * Create a new dispatch table for the class
 */
bool _objc_dtable_init(Class cls) {
  if (cls->dtable != NULL) {
    return false; // Dispatch table already exists
  }

  cls->dtable = sys_malloc(sizeof(void *) * DTABLE_SIZE);
  if (cls->dtable == NULL) {
    return false; // Memory allocation failed
  }
  for (int i = 0; i < DTABLE_SIZE; i++) {
    cls->dtable[i] = NULL;
  }

  // Return success
  return true;
}

/*
 * Check whether an IMP exists in the dispatch table
 */
bool _objc_dtable_exists(Class cls, void *sel_id) {
  if (cls->dtable == NULL) {
    return false;
  }

  uint32_t hash = _objc_dtable_hash_ptr(sel_id);
  for (int i = 0; i < DTABLE_SIZE; i++) {
    uint32_t idx = (hash + (uint32_t)i) & DTABLE_MASK;
    void *entry = cls->dtable[idx];

    if (entry == sel_id)
      return true;
    if (entry == NULL)
      return false; // Hit empty slot, not found
  }
  return false; // Table full but not found
}

/*
 * Add an IMP to the dispatch table (linear probing). No deletes supported.
 */
bool _objc_dtable_add(Class cls, void *sel_id) {
  if (cls->dtable == NULL) {
    return false;
  }

  uint32_t hash = _objc_dtable_hash_ptr(sel_id);
  for (int i = 0; i < DTABLE_SIZE; i++) {
    uint32_t idx = (hash + (uint32_t)i) & DTABLE_MASK;

    if (cls->dtable[idx] == NULL) {
      cls->dtable[idx] = sel_id;
      return true;
    }
    if (cls->dtable[idx] == sel_id) {
      return true; // Already exists
    }
  }
  // Should never reach here with proper load factor
  return false;
}
