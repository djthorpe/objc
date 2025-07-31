#pragma once

#include <stdbool.h>
#include <stddef.h>

// Opaque types
typedef struct objc_hash_table objc_hash_table_t;

/** @brief Iterator state for hash table traversal.
 */
typedef struct objc_hash_iterator {
  struct objc_hash_table *table; ///< Current table being iterated
  size_t index;                  ///< Current index within table
} objc_hash_iterator_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/** @brief Initialize a new hash table
 * @param size Number of entries in the hash table (must be > 0)
 * @return Pointer to new hash table, or NULL if allocation failed
 */
objc_hash_table_t *objc_hash_table_init(size_t size);

/** @brief Free all the hash tables in the chain
 * @param table First hash table in the chain to free
 */
void objc_hash_table_free(objc_hash_table_t *table);

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS - String Key API

/** @brief Get a void* value by string key from the hash table chain.
 * @param table The hash table to search
 * @param key String key to look up (must be non-NULL and < 16 chars)
 * @return Pointer to the value, or NULL if key not found
 */
void *objc_hash_lookup(objc_hash_table_t *table, const char *key);

/** @brief Set a void* value by string key
 * @param table The hash table to insert into
 * @param key String key (must be non-NULL and < 16 chars)
 * @param value Pointer value to store (must be non-NULL)
 * @param overwritten_value Output parameter set to the previous value if key
 * existed, otherwise set to NULL (even if previous value equals new value)
 * @return true if successful, false if allocation failed
 */
bool objc_hash_set(objc_hash_table_t *table, const char *key, void *value,
                   void **overwritten_value);

/** @brief Delete an entry by string key
 * @param table The hash table to delete from
 * @param key String key to delete (must be non-NULL)
 * @return Pointer to the deleted value, or NULL if key not found
 */
void *objc_hash_delete(objc_hash_table_t *table, const char *key);

///////////////////////////////////////////////////////////////////////////////
// ITERATION METHODS

/** @brief Get the next entry from the iterator.
 *
 * This function handles the complete iterator lifecycle:
 * - Auto-initializes the iterator on first call (when table is NULL)
 * - Returns each key-value pair in the hash table chain
 * - Auto-resets the iterator when iteration is complete
 * - Iterator can be reused by calling again after completion
 *
 * Usage:
 * @code
 * objc_hash_iterator_t iter = {0};  // Zero-initialize on stack
 * const char *key;
 * void *value;
 *
 * while (objc_hash_iterator_next(table, &iter, &key, &value)) {
 *     printf("Key: %s, Value: %p\n", key, value);
 * }
 * // iter is automatically reset and can be reused
 * @endcode
 *
 * @param table The hash table to iterate over (used for initialization)
 * @param iterator Pointer to iterator state (initialized if table is
 * NULL, reset when done)
 * @param key Output parameter for the key string (set to NULL when done)
 * @param value Output parameter for the value pointer (set to NULL when done)
 * @return true if an entry was found, false if iteration is complete
 */
bool objc_hash_iterator_next(objc_hash_table_t *table,
                             objc_hash_iterator_t *iterator, const char **key,
                             void **value);
