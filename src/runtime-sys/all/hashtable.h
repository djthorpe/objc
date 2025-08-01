/**
 * @file hashtable_private.h
 * @brief Private header for hashtable implementation details
 *
 * This file contains internal implementation details for the hashtable
 * and hashmap modules, including helper macros for flag manipulation.
 * This header should only be included by hashtable.c and hashmap.c.
 */
#pragma once

#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PRIVATE MACROS

// Entry flags (private implementation details)
#define SYS_HASHTABLE_ENTRY_DELETED 0x01 ///< Entry has been deleted (tombstone)

// Helper macros for entry flags
#define IS_DELETED(entry) ((entry)->flags & SYS_HASHTABLE_ENTRY_DELETED)
#define SET_DELETED(entry) ((entry)->flags |= SYS_HASHTABLE_ENTRY_DELETED)
#define CLEAR_DELETED(entry) ((entry)->flags &= ~SYS_HASHTABLE_ENTRY_DELETED)

///////////////////////////////////////////////////////////////////////////////
// PRIVATE TYPES

/** @brief Represents a hash table.
 */
struct sys_hashtable {
  struct sys_hashtable *next;
  size_t size;
  sys_hashtable_keyequals_t keyequals;
  /**
   * @brief Points to memory allocated after the struct, which contains the
   * hash table buckets.
   */
  sys_hashtable_entry_t *entries;
#if defined(__LP64__) || defined(_WIN64)
} __attribute__((aligned(8))); // 64-bit systems: 8-byte alignment
#else
} __attribute__((aligned(4))); // 32-bit systems: 4-byte alignment
#endif
