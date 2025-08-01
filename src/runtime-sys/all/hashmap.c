#include "hashtable_private.h"
#include <runtime-sys/sys.h>
#include <string.h>

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

/** @brief Finalize a hash map, freeing all malloc'd keys.
 */
void sys_hashmap_finalize(sys_hashtable_t *table) {
  sys_assert(table != NULL);

  // Traverse all tables in the chain
  sys_hashtable_t *current = table;
  while (current) {
    // Free malloc'd keys in current table
    for (size_t i = 0; i < current->size; i++) {
      sys_hashtable_entry_t *entry = &current->entries[i];

      // Free malloc'd key if entry is active and has malloc'd flag
      if (!IS_DELETED(entry) && entry->value != 0 && IS_MALLOCED(entry)) {
        sys_free(entry->keyptr);
      }
    }
    current = current->next;
  }

  // Now free the hashtable structure itself
  sys_hashtable_finalize(table);
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

static bool _sys_keyequals(void *keyptr, sys_hashtable_entry_t *entry) {
  sys_assert(keyptr != NULL);
  sys_assert(entry != NULL);
  return strcmp((const char *)keyptr, (const char *)entry->keyptr) == 0;
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

/** @brief Search for an entry in the hash map by string key, and return the
 * value.
 */
uintptr_t sys_hashmap_get_value(sys_hashtable_t *table, const char *key) {
  sys_assert(table);
  sys_assert(key);

  // Compute the hash of the key, then search using the hash
  sys_hashtable_entry_t *entry =
      sys_hashtable_get_key(table, sys_hash_djb2(key), (void *)key);
  return entry ? entry->value : 0;
}

/** @brief Search for an entry in the hash map by value, and return the key.
 */
const char *sys_hashmap_get_key(sys_hashtable_t *table, uintptr_t value) {
  sys_assert(table);
  sys_assert(value);

  sys_hashtable_entry_t *entry = sys_hashtable_get_value(table, value);
  return entry ? entry->keyptr : 0;
}

/** @brief Insert or update an entry in the hash map with a string key.
 */
bool sys_hashmap_put(sys_hashtable_t *table, const char *key, bool copy,
                     uintptr_t value, uintptr_t *replaced) {
  sys_assert(table);
  sys_assert(key);
  sys_assert(value);

  // Obtain the entry in which to put the value
  bool samekey = false;
  sys_hashtable_entry_t *entry =
      sys_hashtable_put(table, sys_hash_djb2(key), (void *)key, &samekey);
  if (entry == NULL) {
    return false;
  }

  // If samekey is true, we set the replaced value
  if (samekey && replaced) {
    *replaced = entry->value;
  }

  // Now we can safely set the value
  entry->value = value;

  // If this is an existing key, we need to handle key replacement
  if (samekey) {
    // For existing keys, free malloc'd memory if necessary
    if (IS_MALLOCED(entry)) {
      sys_free(entry->keyptr);
      CLEAR_MALLOCED(entry);
    }
  }

  // Handle key storage based on copy parameter
  if (copy) {
    // Copy the key into our managed storage
    size_t key_length = strlen(key) + 1; // +1 for null terminator
    if (key_length <= sizeof(entry->keybuf)) {
      // Key fits in embedded buffer - copy directly
      memcpy(entry->keybuf, key, key_length);
      entry->keyptr = entry->keybuf;
      // No malloc flag needed for embedded keys
    } else {
      // Key too long for embedded buffer - allocate memory
      char *key_copy = sys_malloc(key_length);
      if (!key_copy) {
        SET_DELETED(entry);   // Mark as deleted if allocation failed
        entry->keyptr = NULL; // Clear key pointer
        if (replaced) {
          *replaced = 0; // No value to return if allocation failed
        }
        return false; // Memory allocation failed
      }
      memcpy(key_copy, key, key_length);
      entry->keyptr = key_copy;
      SET_MALLOCED(entry); // Mark that this key was malloc'd
    }
  } else {
    // Don't copy - just reference the external key
    // WARNING: Caller must ensure key remains valid for lifetime of entry
    entry->keyptr = (void *)key;
    // No malloc flag for external references
  }

  return true;
}

/** @brief Delete an entry from the hash map by string key.
 */
uintptr_t sys_hashmap_delete(sys_hashtable_t *table, const char *key) {
  sys_assert(table);
  sys_assert(key);

  sys_hashtable_entry_t *entry =
      sys_hashtable_delete_key(table, sys_hash_djb2(key), (void *)key);
  if (entry == NULL) {
    return 0; // Key not found
  }

  uintptr_t deleted_value = entry->value;

  // Free malloc'd key if the malloc flag is set
  if (IS_MALLOCED(entry)) {
    sys_free(entry->keyptr);
    CLEAR_MALLOCED(entry);
  }

  return deleted_value;
}
