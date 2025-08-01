/**
 * @file hashtable.h
 * @brief A hash table implementation.
 * @defgroup SystemHashTable Hash Tables
 * @ingroup System
 *
 * Methods for hash table creation, insertion, deletion, and lookup.
 *
 * This file declares types and functions for creating and managing hash tables.
 * Hash tables are a data structure that implements an associative array
 * abstract data type, a structure that can map keys to values.
 */
#pragma once

#define SYS_HASHTABLE_KEY_SIZE 24 ///< Maximum key size in bytes

/**
 * @brief Opaque type for hash table
 * @ingroup SystemHashTable
 *
 * This type represents a hash table that can store key-value pairs.
 */
typedef struct sys_hashtable sys_hashtable_t;

/**
 * @brief Iterator state for hash table traversal.
 * @ingroup SystemHashTable
 *
 * This structure is used to maintain the state of an iterator while traversing
 * a hash table.
 * To initialize an iterator, set the `table` field to NULL.
 */
typedef struct sys_hashtable_iterator {
  sys_hashtable_t *table; ///< Current table being iterated
  size_t index;           ///< Current index within table
} sys_hashtable_iterator_t;

/**
 * @brief Hash table entry.
 * @ingroup SystemHashTable
 *
 * This structure represents a single entry in the hash table, containing
 * a hash key, value, and pointer to the concrete key implementation.
 * The key can be stored directly in the entry or as a pointer to an external
 * key buffer.
 */
typedef struct {
  uintptr_t hash;  ///< Hash key for the entry
  uintptr_t value; ///< Value associated with the entry
  bool deleted; ///< Flag indicating if this entry has been deleted (tombstone)
  void *keyptr; ///< Pointer to key data
  char keybuf[SYS_HASHTABLE_KEY_SIZE]; ///< Fixed-size key buffer, which can be
                                       ///< used to store keys directly in the
                                       ///< entry
} sys_hashtable_entry_t;

/**
 * @brief Compare entry with a key.
 * @ingroup SystemHashTable
 *
 * This callback function is used to compare a key with an entry in the hash
 * table. It should return true if the key matches the entry, false otherwise.
 */
typedef bool (*sys_hashtable_keyequals_t)(void *keyptr,
                                          sys_hashtable_entry_t *entry);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a new hash table
 * @ingroup SystemHashTable
 * @param size Number of entries in the hash table (must be > 0)
 * @param equals Function to compare a key pointer to an entry for equality
 * @return Pointer to new hash table, or NULL if allocation failed
 */
sys_hashtable_t *sys_hashtable_init(size_t size,
                                    sys_hashtable_keyequals_t keyequals);

/**
 * @brief Free all the hash tables in the chain
 * @ingroup SystemHashTable
 * @param table First hash table in the chain to free
 */
void sys_hashtable_finalize(sys_hashtable_t *table);

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Search for an entry in the hash table by hash key.
 * @ingroup SystemHashTable
 * @param table The hash table to search in
 * @param hash The hash key for the entry
 * @param keyptr Pointer to the key data for comparison
 * @return Pointer to the found entry, or NULL if not found
 */
sys_hashtable_entry_t *sys_hashtable_get_key(sys_hashtable_t *table,
                                             uintptr_t hash, void *keyptr);

/**
 * @brief Search for an entry in the hash table by hash value.
 * @ingroup SystemHashTable
 * @param table The hash table to search in
 * @param value The value to search for
 * @return Pointer to the found entry, or NULL if not found
 */
sys_hashtable_entry_t *sys_hashtable_get_value(sys_hashtable_t *table,
                                               uintptr_t value);

/**
 * @brief Put an entry into the hash table, replacing any existing entry.
 * @ingroup SystemHashTable
 * @param table The hash table to insert into
 * @param hash The hash key for the entry
 * @param keyptr Pointer to the key data
 * @param value The value to insert
 * @param replaced Callback for the replaced entry if it was replaced
 * @return Pointer to the inserted/updated entry, or NULL if allocation failed
 */
sys_hashtable_entry_t *
sys_hashtable_put(sys_hashtable_t *table, uintptr_t hash, void *keyptr,
                  uintptr_t value,
                  void (*replaced)(sys_hashtable_entry_t *entry));

/**
 * @brief Delete an entry into the hash table by key.
 * @ingroup SystemHashTable
 * @param table The hash table to delete from
 * @param hash The hash key for the entry
 * @param keyptr Pointer to the key data for comparison
 * @return Pointer to the deleted entry, or NULL if not found
 */
sys_hashtable_entry_t *sys_hashtable_delete_key(sys_hashtable_t *table,
                                                uintptr_t hash, void *keyptr);

/** @brief Delete an entry from the hash table by value.
 * @ingroup SystemHashTable
 * @param table The hash table to delete from
 * @param value The value to delete
 * @return The deleted entry, or NULL if not found
 */
sys_hashtable_entry_t *sys_hashtable_delete_value(sys_hashtable_t *table,
                                                  uintptr_t value);

/** @brief Get the next entry from the iterator
 * @ingroup SystemHashTable
 * @param table The hash table being iterated
 * @param iterator Pointer to the iterator state (will be updated)
 * @return Pointer to the next entry, or NULL if no more entries
 */
sys_hashtable_entry_t *
sys_hashtable_iterator_next(sys_hashtable_t *table,
                            sys_hashtable_iterator_t **iterator);

/** @brief Get the number of active entries in the hash table chain.
 * @ingroup SystemHashTable
 * @param table The hash table to count entries in
 * @return The total number of active (non-deleted) entries across all tables in
 * the chain
 */
size_t sys_hashtable_count(sys_hashtable_t *table);

/** @brief Get the total capacity of all tables in the chain.
 * @ingroup SystemHashTable
 * @param table The hash table to get capacity for
 * @return The total capacity (size) of all tables in the chain
 */
size_t sys_hashtable_capacity(sys_hashtable_t *table);
