#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/** @brief Represents a hash table.
 */
struct sys_hashtable {
  struct sys_hashtable *next;
  size_t size;
  sys_hashtable_keyequals_t keyequals;
  /**
   * @brief Points to the flexible array member containing the hash table
   * buckets.
   */
  sys_hashtable_entry_t *entries;
#if defined(__LP64__) || defined(_WIN64)
} __attribute__((aligned(8))); // 64-bit systems: 8-byte alignment
#else
} __attribute__((aligned(4))); // 32-bit systems: 4-byte alignment
#endif

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/** @brief Initialize a new hash table
 */
static sys_hashtable_t *
_sys_hashtable_new(size_t size, sys_hashtable_t *prev,
                   sys_hashtable_keyequals_t keyequals) {
  sys_assert(size > 0);
  sys_assert(prev == NULL || prev->next == NULL);

  // Allocate memory for the hash table structure
  size_t total_size =
      sizeof(sys_hashtable_t) + size * sizeof(sys_hashtable_entry_t);
  sys_hashtable_t *table = sys_malloc(total_size);
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
  table->keyequals = keyequals;
  table->entries = (sys_hashtable_entry_t *)(table + 1);

  // Return the new hash table
  return table;
}

/** @brief Initialize a new chain of hash tables
 */
sys_hashtable_t *sys_hashtable_init(size_t size,
                                    sys_hashtable_keyequals_t keyequals) {
  sys_assert(size > 0);
  return _sys_hashtable_new(size, NULL, keyequals);
}

/** @brief Free all the hash tables in the chain
 */
void sys_hashtable_finalize(sys_hashtable_t *table) {
  sys_assert(table);

  // Free each hash table in the chain
  while (table) {
    sys_hashtable_t *next = table->next;
    sys_free(table);
    table = next;
  }
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/** @brief Search for an entry in a hash table using a hash key
 */
static sys_hashtable_entry_t *
_sys_hashtable_get_bykey_inner(sys_hashtable_t *table, uintptr_t hash,
                               void *keyptr) {
  sys_assert(table);

  // Calculate the starting index based on the key hash
  size_t start_index = hash % table->size;
  size_t index = start_index;
  size_t probes = 0;
  while (probes < table->size) {
    sys_hashtable_entry_t *entry = &table->entries[index];

    // Found the hash and it's not deleted
    if (entry->hash == hash && !entry->deleted) {
      if (table->keyequals == NULL) {
        // No key comparison function, just return if hashes match
        return entry;
      } else if (table->keyequals(keyptr, entry)) {
        // Exact match found using key comparison function
        return entry;
      }
    }

    // Empty slot found (never been used) - key definitely not in table
    if (entry->value == 0 && !entry->deleted) {
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
static sys_hashtable_entry_t *
_sys_hashtable_find_slot(sys_hashtable_t *table, uintptr_t hash, void *keyptr) {
  sys_assert(table != NULL);

  // Calculate the starting index based on the key hash
  size_t start_index = hash % table->size;
  size_t index = start_index;
  size_t probes = 0;
  sys_hashtable_entry_t *first_deleted = NULL;
  while (probes < table->size) {
    sys_hashtable_entry_t *entry = &table->entries[index];

    // Exact match found (for updates)
    if (entry->hash == hash && !entry->deleted) {
      if (table->keyequals == NULL) {
        return entry; // No key comparison function, just return if hashes match
      } else if (table->keyequals(keyptr, entry)) {
        return entry; // Exact match found using key comparison function
      }
    }

    // Remember first deleted slot for potential reuse
    if (entry->deleted && first_deleted == NULL) {
      first_deleted = entry;
    }

    // Empty slot found
    if (entry->value == 0 && !entry->deleted) {
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
// PUBLIC METHODS

/** @brief Search for an entry across all hash tables in the chain using a hash
 * key.
 */
sys_hashtable_entry_t *sys_hashtable_get_key(sys_hashtable_t *root,
                                             uintptr_t hash, void *keyptr) {
  sys_assert(root != NULL);

  // Traverse the chain of hash tables
  sys_hashtable_t *table = root;
  while (table) {
    sys_hashtable_entry_t *entry =
        _sys_hashtable_get_bykey_inner(table, hash, keyptr);
    if (entry) {
      return entry; // Found in this table
    }
    table = table->next; // Move to next table in chain
  }

  // Key not found in any table in the chain
  return NULL;
}

/** @brief Search for an entry in the hash table by value.
 */
sys_hashtable_entry_t *sys_hashtable_get_value(sys_hashtable_t *root,
                                               uintptr_t value) {
  sys_assert(root);
  sys_assert(value);

  // Traverse the chain of hash tables
  sys_hashtable_t *table = root;
  while (table) {
    // Linear search through all entries in current table
    for (size_t i = 0; i < table->size; i++) {
      sys_hashtable_entry_t *entry = &table->entries[i];

      // Check if entry matches value and is not deleted
      if (!entry->deleted && entry->value == value) {
        return entry; // Found matching value
      }
    }
    table = table->next; // Move to next table in chain
  }

  // Value not found in any table in the chain
  return NULL;
}

/** @brief Put an entry into the hash table, replacing any existing entry.
 */
sys_hashtable_entry_t *
sys_hashtable_put(sys_hashtable_t *root, uintptr_t hash, void *keyptr,
                  uintptr_t value,
                  void (*replaced)(sys_hashtable_entry_t *entry)) {
  sys_assert(root);
  sys_assert(value);

  // Try to find existing entry or available slot in current tables
  sys_hashtable_t *table = root;
  sys_hashtable_t *prev = NULL;
  while (table != NULL) {
    sys_hashtable_entry_t *slot = _sys_hashtable_find_slot(table, hash, keyptr);
    if (slot == NULL) {
      prev = table; // Keep track of the last table
      table = table->next;
      continue;
    }

    // Check if this is an existing entry with a different value
    if (slot->hash == hash && !slot->deleted) {
      // Existing entry found - replace value
      if (slot->value != value && replaced) {
        replaced(slot); // Call replacement callback
      }
    }

    // Set the new value
    slot->hash = hash;
    slot->value = value;
    slot->deleted = false;
    slot->keyptr = keyptr;

    // Return the slot
    return slot;
  }

  // All tables are full - create a new one with 1.5x growth
  size_t new_size = root->size + (root->size >> 1);
  table = _sys_hashtable_new(new_size, prev, root->keyequals);
  if (table == NULL) {
    return NULL; // Allocation failed
  }

  // Insert into the new table using linear probing
  sys_hashtable_entry_t *slot = _sys_hashtable_find_slot(table, hash, keyptr);
  sys_assert(slot);
  slot->hash = hash;
  slot->value = value;
  slot->deleted = false;
  slot->keyptr = keyptr;
  return slot;
}

/** @brief Delete an entry into the hash table by key.
 */
sys_hashtable_entry_t *sys_hashtable_delete_key(sys_hashtable_t *table,
                                                uintptr_t hash, void *keyptr) {
  sys_assert(table);
  sys_hashtable_entry_t *entry = sys_hashtable_get_key(table, hash, keyptr);
  if (entry == NULL) {
    return NULL; // Not found
  }
  entry->deleted = true; // Mark as deleted
  return entry;          // Return the deleted entry
}

/** @brief Delete an entry into the hash table by value.
 */
sys_hashtable_entry_t *sys_hashtable_delete_value(sys_hashtable_t *table,
                                                  uintptr_t value) {
  sys_assert(table);
  sys_assert(value);

  sys_hashtable_entry_t *entry = sys_hashtable_get_value(table, value);
  if (entry == NULL) {
    return NULL; // Not found
  }
  entry->deleted = true; // Mark as deleted
  return entry;          // Return the deleted entry
}

/** @brief Get the next entry from the iterator
 */
sys_hashtable_entry_t *
sys_hashtable_iterator_next(sys_hashtable_t *table,
                            sys_hashtable_iterator_t **iterator) {
  sys_assert(table != NULL);
  sys_assert(iterator != NULL);

  // Get the current iterator state
  sys_hashtable_iterator_t *iter = *iterator;
  sys_assert(iter != NULL);

  // If the iterator is not initialized, start from the first table
  if (iter->table == NULL) {
    iter->table = table;
    iter->index = 0; // Start from the first index
  }

  while (iter->table) {
    // Search for next valid entry in current table
    while (iter->index < iter->table->size) {
      sys_hashtable_entry_t *entry = &iter->table->entries[iter->index];
      iter->index++;

      // Return entries that are not deleted and have been used (non-zero value)
      if (!entry->deleted && entry->value != 0) {
        return entry;
      }
    }

    // Move to next table in chain
    iter->table = iter->table->next;
    iter->index = 0;
  }

  // Iteration complete - reset iterator for potential reuse
  iter->table = NULL;
  iter->index = 0;
  return NULL;
}

/** @brief Get the number of active entries in the hash table chain.
 */
size_t sys_hashtable_count(sys_hashtable_t *table) {
  sys_assert(table != NULL);

  size_t count = 0;
  sys_hashtable_t *current = table;

  // Traverse all tables in the chain
  while (current) {
    // Count active entries in current table
    for (size_t i = 0; i < current->size; i++) {
      sys_hashtable_entry_t *entry = &current->entries[i];

      // Count entries that are not deleted and have been used (non-zero value)
      if (!entry->deleted && entry->value != 0) {
        count++;
      }
    }
    current = current->next;
  }

  return count;
}

/** @brief Get the total capacity of all tables in the chain.
 */
size_t sys_hashtable_capacity(sys_hashtable_t *table) {
  sys_assert(table != NULL);

  size_t capacity = 0;
  sys_hashtable_t *current = table;

  // Traverse all tables in the chain and sum their sizes
  while (current) {
    capacity += current->size;
    current = current->next;
  }

  return capacity;
}
