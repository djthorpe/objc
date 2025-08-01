#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static bool _sys_keyequals(void *keyptr, sys_hashtable_entry_t *entry);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/** @brief Initialize a new hash map for string keys.
 */
sys_hashtable_t *sys_hashmap_init(size_t size) {
  sys_assert(size > 0);
  return sys_hashtable_init(size, _sys_keyequals);
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

static bool _sys_keyequals(void *keyptr, sys_hashtable_entry_t *entry) {
  sys_assert(keyptr != NULL);
  sys_assert(entry != NULL);
  return strcmp((const char *)keyptr, entry->key) == 0;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief djb2 hash function for strings
 */
uintptr_t sys_hash_djb2(const char *str) {
  sys_assert(str != NULL);

  uintptr_t hash = 5381; // Standard djb2 initial value
  uintptr_t c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash;
}

/** @brief Search for an entry in the hash map by string key.
 */
sys_hashtable_entry_t *sys_hashmap_get_key(sys_hashtable_t *table,
                                           const char *key) {
  sys_assert(table);
  sys_assert(key);

  // Compute the hash of the key, then search using the hash
  return sys_hashtable_get_key(table, sys_hash_djb2(key), (void *)key);
}

/** @brief Search for an entry in the hash map by value.
 */
sys_hashtable_entry_t *sys_hashmap_get_value(sys_hashtable_t *table,
                                             uintptr_t value) {
  sys_assert(table);
  sys_assert(value);
  // Search using the value directly
  return sys_hashtable_get_value(table, value);
}

/** @brief Insert or update an entry in the hash map with a string key.
 */
bool sys_hashmap_put(sys_hashtable_t *table, const char *key, uintptr_t value,
                     uintptr_t *replaced) {
  sys_assert(table);
  sys_assert(key);
  sys_assert(value);
  // TODO
}

/** @brief Delete an entry from the hash map by string key.
 */
bool sys_hashmap_delete_key(sys_hashtable_t *table, const char *key,
                            uintptr_t *deleted) {
  // TODO
}

/** @brief Delete an entry from the hash map by value.
 */
bool sys_hashmap_delete_value(sys_hashtable_t *table, uintptr_t value) {
  // TODO
}
