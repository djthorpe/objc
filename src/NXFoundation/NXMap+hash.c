#include "NXMap+hash.h"
#include <objc/objc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

#define MAX_KEY_SIZE 16 // Maximum size for string keys

/** @brief Represents a hash table for pointer values.
 */
struct objc_hash_table {
  struct objc_hash_table *next;
  size_t size;
  struct objc_ptr_hash_entry *entries; ///< Pointer to the hash table entries
};

/** @brief Represents an entry in the pointer hash table.
 */
struct objc_ptr_hash_entry {
  void *value;    ///< Pointer value (largest alignment requirement first)
  uintptr_t hash; ///< Hash value for the key
  char key[MAX_KEY_SIZE]; ///< String key storage
  bool deleted; ///< Flag indicating if this entry has been deleted (tombstone)
#if defined(__LP64__) || defined(_WIN64)
} __attribute__((aligned(8))); // 64-bit systems: 8-byte alignment
#else
} __attribute__((aligned(4))); // 32-bit systems: 4-byte alignment
#endif

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

static struct objc_hash_table *
objc_hash_table_new(size_t size, struct objc_hash_table *prev) {
  objc_assert(size > 0);
  objc_assert(prev == NULL || (prev->size == size && prev->next == NULL));

  // Allocate memory for the hash table structure
  size_t total_size = sizeof(struct objc_hash_table) +
                      size * sizeof(struct objc_ptr_hash_entry);
  struct objc_hash_table *table = sys_malloc(total_size);
  if (table == NULL) {
    return NULL; // Allocation failed
  }

  // Set next pointer
  if (prev) {
    prev->next = table;
  }

  // Initialize the hash table
  sys_memset(table, 0, total_size);
  table->next = NULL;
  table->size = size;
  table->entries = (struct objc_ptr_hash_entry *)(table + 1);

  // Return the new hash table
  return table;
}

struct objc_hash_table *objc_hash_table_init(size_t size) {
  return objc_hash_table_new(size, NULL);
}

void objc_hash_table_free(objc_hash_table_t *table) {
  objc_assert(table != NULL);

  // Free each hash table in the chain
  while (table) {
    struct objc_hash_table *next = table->next;
    sys_free(table);
    table = next;
  }
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/** @brief djb2 hash function for strings
 */
static inline uintptr_t objc_hash_table_strhash(const char *str) {
  objc_assert(str != NULL); // Protect against null string input

  uintptr_t hash = 5381; // Standard djb2 initial value
  uintptr_t c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash;
}

/** @brief Search for an entry in a single hash table using linear probing.
 */
static struct objc_ptr_hash_entry *
objc_hash_table_get_entry_inner(struct objc_hash_table *table, uintptr_t key) {
  objc_assert(table != NULL);

  // Calculate the starting index based on the key hash
  size_t start_index = key % table->size;
  size_t index = start_index;
  size_t probes = 0;
  while (probes < table->size) {
    struct objc_ptr_hash_entry *entry = &table->entries[index];

    // Found the exact key and it's not deleted
    if (entry->hash == key && !entry->deleted) {
      return entry;
    }

    // Empty slot found (never been used) - key definitely not in table
    if (entry->value == NULL && !entry->deleted) {
      return NULL;
    }

    // Continue probing through deleted entries and collisions
    index = (index + 1) % table->size;
    probes++;
  }

  // Searched entire table without finding key or empty slot
  return NULL;
}

/** @brief Find the best slot for inserting a new key, preferring deleted slots.
 */
static struct objc_ptr_hash_entry *
objc_hash_table_find_slot(struct objc_hash_table *table, uintptr_t key) {
  objc_assert(table != NULL);

  // Calculate the starting index based on the key hash
  size_t start_index = key % table->size;
  size_t index = start_index;
  size_t probes = 0;
  struct objc_ptr_hash_entry *first_deleted = NULL;
  while (probes < table->size) {
    struct objc_ptr_hash_entry *entry = &table->entries[index];

    // Exact match found (for updates)
    if (entry->hash == key && !entry->deleted) {
      return entry;
    }

    // Remember first deleted slot for potential reuse
    if (entry->deleted && first_deleted == NULL) {
      first_deleted = entry;
    }

    // Empty slot found
    if (entry->value == NULL && !entry->deleted) {
      // Return deleted slot if we found one earlier (better for fragmentation)
      return first_deleted ? first_deleted : entry;
    }

    index = (index + 1) % table->size;
    probes++;
  }

  // Return deleted slot if available, NULL otherwise
  return first_deleted;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS - struct objc_ptr_hash_entry*

/** @brief Search for an entry across all hash tables in the chain.
 */
struct objc_ptr_hash_entry *
objc_hash_table_get_entry(struct objc_hash_table *root, uintptr_t key) {
  objc_assert(root != NULL);

  // Traverse the chain of hash tables
  struct objc_hash_table *table = root;
  while (table) {
    struct objc_ptr_hash_entry *entry =
        objc_hash_table_get_entry_inner(table, key);
    if (entry) {
      return entry; // Found in this table
    }
    table = table->next; // Move to next table in chain
  }

  // Key not found in any table in the chain
  return NULL;
}

/** @brief Insert or update an entry in the hash table chain.
 */
struct objc_ptr_hash_entry *
objc_hash_table_put_entry(struct objc_hash_table *root, uintptr_t key,
                          void *value, void **overwritten_value) {
  objc_assert(root != NULL);
  objc_assert(value !=
              NULL); // Values cannot be NULL (reserved for empty detection)
  objc_assert(overwritten_value != NULL);

  *overwritten_value = NULL; // Initialize output parameter

  // Try to find existing entry or available slot in current tables
  struct objc_hash_table *table = root;
  struct objc_hash_table *prev = NULL;
  while (table != NULL) {
    struct objc_ptr_hash_entry *slot = objc_hash_table_find_slot(table, key);
    if (slot) {
      // Found a slot (existing, deleted, or empty)
      // Check if this is an existing entry with a different value
      if (slot->hash == key && !slot->deleted &&
          slot->value != value) {
        *overwritten_value = slot->value;
      }
      slot->hash = key;
      slot->value = value;
      slot->deleted = false;
      return slot;
    }
    prev = table; // Keep track of the last table
    table = table->next;
  }

  // All tables are full - create a new one
  table = objc_hash_table_new(root->size, prev);
  if (table == NULL) {
    return NULL; // Allocation failed
  }

  // Insert into the new empty table (guaranteed to succeed)
  struct objc_ptr_hash_entry *slot = &table->entries[key % table->size];
  objc_assert(slot); // Should be empty slot
  slot->hash = key;
  slot->value = value;
  slot->deleted = false;
  return slot;
}

/** @brief Mark an entry as deleted
 */
void *objc_hash_table_delete_entry(struct objc_hash_table *root,
                                   uintptr_t key) {
  objc_assert(root != NULL);

  struct objc_ptr_hash_entry *entry = objc_hash_table_get_entry(root, key);
  if (entry) {
    void *old_value = entry->value;
    entry->deleted = true;
    entry->value = NULL;
    return old_value;
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS - const char*

void *objc_hash_lookup(objc_hash_table_t *table, const char *key) {
  objc_assert(table != NULL);
  objc_assert(key != NULL);

  uintptr_t hash = objc_hash_table_strhash(key);
  struct objc_ptr_hash_entry *entry = objc_hash_table_get_entry(table, hash);
  return entry ? entry->value : NULL;
}

bool objc_hash_set(objc_hash_table_t *table, const char *key, void *value,
                   void **overwritten_value) {
  objc_assert(table != NULL);
  objc_assert(key != NULL);
  objc_assert(value != NULL);
  objc_assert(overwritten_value != NULL);

  size_t key_len = strlen(key);
  objc_assert(key_len < MAX_KEY_SIZE); // Ensure key fits in buffer

  *overwritten_value = NULL; // Initialize output parameter

  // First, check if the key already exists and get its current value
  void *current_value = objc_hash_lookup(table, key);
  if (current_value != NULL) {
    // Key exists - always return the old value (even if it's the same)
    *overwritten_value = current_value;
  }

  uintptr_t hash = objc_hash_table_strhash(key);
  void *dummy_overwritten;
  struct objc_ptr_hash_entry *entry =
      objc_hash_table_put_entry(table, hash, value, &dummy_overwritten);
  if (entry) {
    // Copy the actual string key into the entry
    sys_memcpy(entry->key, key, key_len + 1); // +1 to include null terminator
    return true;
  }
  return false;
}

void *objc_hash_delete(objc_hash_table_t *table, const char *key) {
  objc_assert(table != NULL);
  objc_assert(key != NULL);

  uintptr_t hash = objc_hash_table_strhash(key);
  return objc_hash_table_delete_entry(table, hash);
}

///////////////////////////////////////////////////////////////////////////////
// ITERATION METHODS

/** @brief Get the next entry from the iterator (internal version).
 *
 * @param iterator The iterator state
 * @return Pointer to the next entry, or NULL if iteration is complete
 */
struct objc_ptr_hash_entry *
objc_hash_iterator_next_entry(objc_hash_iterator_t *iterator) {
  objc_assert(iterator != NULL);

  struct objc_hash_iterator *iter = (struct objc_hash_iterator *)iterator;

  while (iter->table) {
    // Search for next valid entry in current table
    while (iter->index < iter->table->size) {
      struct objc_ptr_hash_entry *entry = &iter->table->entries[iter->index];
      iter->index++;

      // Return non-deleted entries with values
      if (!entry->deleted && entry->value != NULL) {
        return entry;
      }
    }

    // Move to next table in chain
    iter->table = iter->table->next;
    iter->index = 0;
  }

  return NULL; // No more entries
}

bool objc_hash_iterator_next(objc_hash_table_t *table,
                             objc_hash_iterator_t *iterator, const char **key,
                             void **value) {
  objc_assert(table != NULL);
  objc_assert(iterator != NULL);
  objc_assert(key != NULL);
  objc_assert(value != NULL);

  struct objc_hash_iterator *iter = (struct objc_hash_iterator *)iterator;

  // Initialize iterator if not started (table is NULL)
  if (iter->table == NULL) {
    iter->table = table;
    iter->index = 0;
  }

  while (iter->table) {
    // Search for next valid entry in current table
    while (iter->index < iter->table->size) {
      struct objc_ptr_hash_entry *entry = &iter->table->entries[iter->index];
      iter->index++;

      // Return non-deleted entries with values
      if (!entry->deleted && entry->value != NULL) {
        *key = entry->key;
        *value = entry->value;
        return true;
      }
    }

    // Move to next table in chain
    iter->table = iter->table->next;
    iter->index = 0;
  }

  // Iteration complete - reset iterator for potential reuse
  iter->table = NULL;
  iter->index = 0;
  *key = NULL;
  *value = NULL;
  return false;
}
