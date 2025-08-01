/**
 * @file hashmap.h
 * @brief Methods for hash map operations, using NULL-terminated strings as the
 * key.
 * @ingroup SystemHashTable
 */
#pragma once
#include "hashtable.h"

/**
 * @brief Initialize a new hash map for string keys.
 * @ingroup SystemHashTable
 * @param size Number of entries in the hash map (must be > 0)
 * @return Pointer to new hash map, or NULL if allocation failed
 *
 * Create a new hash map, which can store uintptr_t values
 * against NULL-terminated string keys of arbitrary length.
 *
 * The hash map uses a djb2 hash function and built-in string
 * comparison function for keys. If the length of the key is
 * larger than some pre-defined limit, it might allocate
 * memory to copy the key otherwise the key could be copied
 * directly into the hash map, to reduce the number of memory
 * allocations.
 *
 * You should call sys_hashmap_finalize() to free the
 * hash map when done.
 */
sys_hashtable_t *sys_hashmap_init(size_t size);

/**
 * @brief Finalize a hash map, freeing all malloc'd keys and the structure.
 * @ingroup SystemHashTable
 * @param table The hash map to finalize
 *
 * This function properly frees any malloc'd string keys before
 * freeing the underlying hash table structure.
 */
void sys_hashmap_finalize(sys_hashtable_t *table);

/**
 * @brief Search for an entry in the hash map by string key, and return the
 * value.
 * @ingroup SystemHashTable
 * @param table The hash table to search in
 * @param key The string key to search for (null-terminated)
 * @return The value associated with the key, or 0 if not found
 */
uintptr_t sys_hashmap_get_value(sys_hashtable_t *table, const char *key);

/**
 * @brief Search for an entry in the hash map by value, and return the key.
 * @ingroup SystemHashTable
 * @param table The hash table to search in
 * @param value The value to search for
 * @return The returned key if found, or NULL otherwise.
 *
 * This function searches for the first entry with the specified value
 * and returns the associated key. If no entry is found, it returns NULL.
 * This is useful for reverse lookups in the hash map, but does a linear
 * search through the entries, so it is not efficient for large hash maps.
 */
const char *sys_hashmap_get_key(sys_hashtable_t *table, uintptr_t value);

/**
 * @brief Insert or update an entry in the hash map with a string key.
 * @ingroup SystemHashTable
 * @param table The hash table to insert into
 * @param key The string key (null-terminated)
 * @param copy Copy the string key if true, or use the pointer directly if false
 * @param value The value to associate with the key
 * @param replaced Optional pointer to store the previous value if key already
 * existed, and was different from the new value.
 * @return true if operation succeeded, false on failure (e.g., memory
 * allocation)
 */
bool sys_hashmap_put(sys_hashtable_t *table, const char *key, bool copy,
                     uintptr_t value, uintptr_t *replaced);

/**
 * @brief Delete an entry from the hash map by string key.
 * @ingroup SystemHashTable
 * @param table The hash table to delete from
 * @param key The string key to delete (null-terminated)
 * @return the value associated with the deleted key, or 0 if not found.
 */
uintptr_t sys_hashmap_delete(sys_hashtable_t *table, const char *key);
