/**
 * @file hashmap.h
 * @brief Methods for hash map operations, using NULL-terminated strings as the
 * key.
 * @ingroup SystemHashTable
 */
#pragma once
#include "hashtable.h"

/** @brief Initialize a new hash map for string keys.
 * @param size Number of entries in the hash map (must be > 0)
 * @return Pointer to new hash map, or NULL if allocation failed
 * @note Uses built-in string comparison function for keys
 */
sys_hashtable_t *sys_hashmap_init(size_t size);

/** @brief Search for an entry in the hash map by string key.
 * @param table The hash table to search in
 * @param key The string key to search for (null-terminated)
 * @return Pointer to the found entry, or NULL if not found
 */
sys_hashtable_entry_t *sys_hashmap_get_key(sys_hashtable_t *table,
                                           const char *key);

/** @brief Search for an entry in the hash map by value.
 * @param table The hash table to search in
 * @param value The value to search for
 * @return Pointer to the found entry, or NULL if not found
 */
sys_hashtable_entry_t *sys_hashmap_get_value(sys_hashtable_t *table,
                                             uintptr_t value);

/** @brief Insert or update an entry in the hash map with a string key.
 * @param table The hash table to insert into
 * @param key The string key (null-terminated) - will be copied/stored
 * @param value The value to associate with the key
 * @param replaced Optional pointer to store the previous value if key existed
 * @return true if operation succeeded, false on failure (e.g., memory
 * allocation)
 */
bool sys_hashmap_put(sys_hashtable_t *table, const char *key, uintptr_t value,
                     uintptr_t *replaced);

/** @brief Delete an entry from the hash map by string key.
 * @param table The hash table to delete from
 * @param key The string key to delete (null-terminated)
 * @param deleted Optional pointer to store the deleted value
 * @return true if entry was found and deleted, false if not found
 */
bool sys_hashmap_delete_key(sys_hashtable_t *table, const char *key,
                            uintptr_t *deleted);

/** @brief Delete an entry from the hash map by value.
 * @param table The hash table to delete from
 * @param value The value to delete (first matching entry will be removed)
 * @return true if entry was found and deleted, false if not found
 */
bool sys_hashmap_delete_value(sys_hashtable_t *table, uintptr_t value);
