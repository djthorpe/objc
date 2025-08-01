#include <runtime-sys/sys.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS FOR STRING HASHTABLE

/**
 * @brief Key comparison function for string keys
 */
static bool _string_keyequals(void *keyptr, sys_hashtable_entry_t *entry) {
  return strcmp((const char *)keyptr, (const char *)entry->keyptr) == 0;
}

/**
 * @brief djb2 hash function for strings
 */
static uintptr_t _string_hash(const char *str) {
  uintptr_t hash = 5381;
  uintptr_t c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

/**
 * @brief Helper to create a string hashtable
 */
static sys_hashtable_t *create_string_hashtable(size_t size) {
  return sys_hashtable_init(size, _string_keyequals);
}

/**
 * @brief Helper to put a string key-value pair (defaults to copying)
 */
static bool put_string(sys_hashtable_t *table, const char *key, uintptr_t value, uintptr_t *replaced) {
  return put_string_copy(table, key, value, true, replaced);
}

/**
 * @brief Helper to put a string key-value pair with copy control
 */
static bool put_string_copy(sys_hashtable_t *table, const char *key, uintptr_t value, bool copy, uintptr_t *replaced) {
  bool samekey = false;
  void *key_to_use = (void *)key;
  
  if (copy) {
    // Check if key already exists first
    sys_hashtable_entry_t *existing = sys_hashtable_get_key(table, _string_hash(key), (void *)key);
    if (existing) {
      // Key exists, just update value
      if (replaced) *replaced = existing->value;
      existing->value = value;
      return true;
    }
    
    // Key doesn't exist, need to copy it
    size_t len = strlen(key) + 1;
    char *key_copy = malloc(len);
    if (!key_copy) return false;
    strcpy(key_copy, key);
    key_to_use = key_copy;
  }
  
  sys_hashtable_entry_t *entry = sys_hashtable_put(table, _string_hash(key), key_to_use, &samekey);
  if (!entry) {
    if (copy && key_to_use != key) {
      free(key_to_use);
    }
    return false;
  }
  
  if (samekey && replaced) {
    *replaced = entry->value;
  } else if (replaced) {
    *replaced = 0;
  }
  
  entry->value = value;
  return true;
}

/**
 * @brief Helper to get a value by string key
 */
static uintptr_t get_string(sys_hashtable_t *table, const char *key) {
  sys_hashtable_entry_t *entry = sys_hashtable_get_key(table, _string_hash(key), (void *)key);
  return entry ? entry->value : 0;
}

/**
 * @brief Helper to delete by string key
 */
static uintptr_t delete_string(sys_hashtable_t *table, const char *key) {
  sys_hashtable_entry_t *entry = sys_hashtable_delete_key(table, _string_hash(key), (void *)key);
  return entry ? entry->value : 0;
}

// Forward declarations
int test_sys_16(void);
int test_hashmap_basic_operations(void);
int test_hashmap_long_keys(void);
int test_hashmap_key_copying(void);
int test_hashmap_collisions(void);
int test_hashmap_overwriting(void);
int test_hashmap_deletion(void);
int test_hashmap_iteration(void);
int test_hashmap_memory_management(void);
int test_hashmap_edge_cases(void);

int main(void) { return TestMain("test_sys_16", test_sys_16); }

int test_sys_16(void) {
  sys_puts("Running comprehensive hashmap tests...\n");

  test_hashmap_basic_operations();
  test_hashmap_long_keys();
  test_hashmap_key_copying();
  test_hashmap_collisions();
  test_hashmap_overwriting();
  test_hashmap_deletion();
  test_hashmap_iteration();
  test_hashmap_memory_management();
  test_hashmap_edge_cases();

  sys_puts("All hashmap tests passed!\n");
  return 0;
}

int test_hashmap_basic_operations(void) {
  sys_puts("Test 1: Basic hashmap operations\n");

  sys_hashtable_t *map = create_string_hashtable(8);
  test_assert(map != NULL);

  // Test basic put and get
  uintptr_t replaced = 0;
  bool success = put_string(map, "key1", 100, &replaced);
  test_assert(success == true);
  test_assert(replaced == 0); // No previous value

  uintptr_t value = get_string(map, "key1");
  test_assert(value == 100);

  // Test multiple keys
  test_assert(put_string(map, "key2", 200, NULL));
  test_assert(put_string(map, "key3", 300, NULL));

  test_assert(get_string(map, "key1") == 100);
  test_assert(get_string(map, "key2") == 200);
  test_assert(get_string(map, "key3") == 300);

  // Test non-existent key
  test_assert(get_string(map, "nonexistent") == 0);

  // Test reverse lookup (by value) - we'll need to iterate
  sys_hashtable_iterator_t iter = {0};
  sys_hashtable_iterator_t *iterptr = &iter;
  sys_hashtable_entry_t *entry;
  const char *found_key = NULL;
  while ((entry = sys_hashtable_iterator_next(map, &iterptr))) {
    if (entry->value == 200) {
      found_key = (const char *)entry->keyptr;
      break;
    }
  }
  test_assert(found_key != NULL);
  test_assert(strcmp(found_key, "key2") == 0);

  // Test non-existent value
  iter = (sys_hashtable_iterator_t){0};
  iterptr = &iter;
  found_key = NULL;
  while ((entry = sys_hashtable_iterator_next(map, &iterptr))) {
    if (entry->value == 999) {
      found_key = (const char *)entry->keyptr;
      break;
    }
  }
  test_assert(found_key == NULL);

  sys_hashtable_finalize(map);
  return 0;
}

int test_hashmap_long_keys(void) {
  sys_puts("Test 2: Long keys (testing embedded vs malloc'd storage)\n");

  sys_hashtable_t *map = create_string_hashtable(4);
  test_assert(map != NULL);

  // Short key that fits in keybuf (SYS_HASHTABLE_KEY_SIZE = 24)
  char short_key[] = "short";
  test_assert(strlen(short_key) < 24);
  test_assert(put_string_copy(map, short_key, 100, true, NULL));
  test_assert(get_string(map, short_key) == 100);

  // Long key that requires malloc
  char long_key[] = "this_is_a_very_long_key_that_exceeds_the_buffer_size_and_"
                    "requires_malloc";
  test_assert(strlen(long_key) > 24);
  test_assert(put_string_copy(map, long_key, 200, true, NULL));
  test_assert(get_string(map, long_key) == 200);

  // Very long key
  char very_long_key[200];
  memset(very_long_key, 'x', 199);
  very_long_key[199] = '\0';
  test_assert(put_string_copy(map, very_long_key, 300, true, NULL));
  test_assert(get_string(map, very_long_key) == 300);

  // Test all keys still work
  test_assert(get_string(map, short_key) == 100);
  test_assert(get_string(map, long_key) == 200);
  test_assert(get_string(map, very_long_key) == 300);

  sys_hashtable_finalize(map);
  return 0;
}

int test_hashmap_key_copying(void) {
  sys_puts("Test 3: Key copying vs reference\n");

  sys_hashtable_t *map = create_string_hashtable(4);
  test_assert(map != NULL);

  // Test copy=false (use reference)
  char stack_key[] = "stack_key";
  test_assert(put_string_copy(map, stack_key, 100, false, NULL));
  test_assert(get_string(map, stack_key) == 100);

  // Test copy=true (copy key)
  char original_key[] = "original_key";
  test_assert(put_string_copy(map, original_key, 200, true, NULL));
  test_assert(get_string(map, original_key) == 200);

  // Modify original - copied key should still work
  original_key[0] = 'X'; // Change first character
  test_assert(get_string(map, "original_key") == 200); // Still works
  test_assert(get_string(map, original_key) == 0); // Modified key not found

  // Test long key copying
  char long_original[] =
      "this_is_a_long_key_that_gets_copied_and_requires_malloc_storage";
  test_assert(put_string_copy(map, long_original, 300, true, NULL));
  test_assert(get_string(map, long_original) == 300);

  // Modify original long key
  long_original[0] = 'X';
  test_assert(
      get_string(
          map,
          "this_is_a_long_key_that_gets_copied_and_requires_malloc_storage") ==
      300);
  test_assert(get_string(map, long_original) == 0);

  sys_hashtable_finalize(map);
  return 0;
}

int test_hashmap_collisions(void) {
  sys_puts("Test 4: Hash collisions and chaining\n");

  // Use small table to force collisions
  sys_hashtable_t *map = create_string_hashtable(2);
  test_assert(map != NULL);

  // Add many keys to force collisions
  char keys[10][20];
  for (int i = 0; i < 10; i++) {
    snprintf(keys[i], sizeof(keys[i]), "collision_key_%d", i);
    test_assert(
        put_string_copy(map, keys[i], (uintptr_t)(100 + i), true, NULL));
  }

  // Verify all keys can be retrieved
  for (int i = 0; i < 10; i++) {
    uintptr_t value = get_string(map, keys[i]);
    test_assert(value == (uintptr_t)(100 + i));
  }

  // Test that hash collisions are handled properly
  test_assert(sys_hashtable_count(map) == 10);

  sys_hashtable_finalize(map);
  return 0;
}

int test_hashmap_overwriting(void) {
  sys_puts("Test 5: Overwriting existing keys\n");

  sys_hashtable_t *map = create_string_hashtable(4);
  test_assert(map != NULL);

  // Initial insert
  uintptr_t replaced = 0;
  test_assert(put_string_copy(map, "test_key", 100, true, &replaced));
  test_assert(replaced == 0); // No previous value
  test_assert(get_string(map, "test_key") == 100);

  // Overwrite with same key
  test_assert(put_string_copy(map, "test_key", 200, true, &replaced));
  test_assert(replaced == 100); // Previous value returned
  test_assert(get_string(map, "test_key") == 200);

  // Overwrite again
  test_assert(put_string_copy(map, "test_key", 300, true, &replaced));
  test_assert(replaced == 200);
  test_assert(get_string(map, "test_key") == 300);

  // Test overwriting without capturing replaced value
  test_assert(put_string_copy(map, "test_key", 400, true, NULL));
  test_assert(get_string(map, "test_key") == 400);

  // Test overwriting with long key
  sys_puts("Testing long key insertion...\n");
  char long_key[] =
      "this_is_a_long_key_for_overwrite_testing_that_requires_malloc";
  test_assert(put_string_copy(map, long_key, 500, true, NULL));
  test_assert(get_string(map, long_key) == 500);

  sys_puts("Testing long key overwrite...\n");
  test_assert(put_string_copy(map, long_key, 600, true, &replaced));
  sys_puts("Long key overwrite completed.\n");
  test_assert(replaced == 500);
  test_assert(get_string(map, long_key) == 600);

  sys_puts("About to finalize map...\n");
  sys_hashtable_finalize(map);
  sys_puts("Map finalized successfully.\n");
  return 0;
}

int test_hashmap_deletion(void) {
  sys_puts("Test 6: Key deletion\n");

  sys_hashtable_t *map = sys_hashmap_init(4);
  test_assert(map != NULL);

  // Add some keys
  test_assert(sys_hashmap_put(map, "key1", true, 100, NULL));
  test_assert(sys_hashmap_put(map, "key2", true, 200, NULL));
  test_assert(sys_hashmap_put(map, "key3", true, 300, NULL));

  char long_key[] =
      "this_is_a_long_key_for_deletion_testing_with_malloc_storage";
  test_assert(sys_hashmap_put(map, long_key, true, 400, NULL));

  // Verify all keys exist
  test_assert(sys_hashmap_get_value(map, "key1") == 100);
  test_assert(sys_hashmap_get_value(map, "key2") == 200);
  test_assert(sys_hashmap_get_value(map, "key3") == 300);
  test_assert(sys_hashmap_get_value(map, long_key) == 400);

  // Delete a key
  uintptr_t deleted_value = sys_hashmap_delete(map, "key2");
  test_assert(deleted_value == 200);
  test_assert(sys_hashmap_get_value(map, "key2") == 0); // No longer found

  // Delete long key
  deleted_value = sys_hashmap_delete(map, long_key);
  test_assert(deleted_value == 400);
  test_assert(sys_hashmap_get_value(map, long_key) == 0);

  // Try to delete non-existent key
  deleted_value = sys_hashmap_delete(map, "nonexistent");
  test_assert(deleted_value == 0);

  // Verify remaining keys still work
  test_assert(sys_hashmap_get_value(map, "key1") == 100);
  test_assert(sys_hashmap_get_value(map, "key3") == 300);

  // Delete remaining keys
  test_assert(sys_hashmap_delete(map, "key1") == 100);
  test_assert(sys_hashmap_delete(map, "key3") == 300);
  test_assert(sys_hashmap_get_value(map, "key1") == 0);
  test_assert(sys_hashmap_get_value(map, "key3") == 0);

  sys_hashmap_finalize(map);
  return 0;
}

int test_hashmap_iteration(void) {
  sys_puts("Test 7: Hashmap iteration\n");

  sys_hashtable_t *map = sys_hashmap_init(4);
  test_assert(map != NULL);

  // Add test data
  const char *keys[] = {"apple", "banana", "cherry", "date"};
  uintptr_t values[] = {1, 2, 3, 4};
  int num_items = 4;

  for (int i = 0; i < num_items; i++) {
    test_assert(sys_hashmap_put(map, keys[i], true, values[i], NULL));
  }

  // Test iteration using hashtable iterator
  sys_hashtable_iterator_t iterator = {0};
  sys_hashtable_iterator_t *iter = &iterator;

  bool found_keys[4] = {false, false, false, false};
  int count = 0;

  sys_hashtable_entry_t *entry;
  while ((entry = sys_hashtable_iterator_next(map, &iter)) != NULL) {
    count++;

    // Find which key this is
    for (int i = 0; i < num_items; i++) {
      if (strcmp((const char *)entry->keyptr, keys[i]) == 0) {
        test_assert(entry->value == values[i]);
        test_assert(found_keys[i] == false); // Not seen before
        found_keys[i] = true;
        break;
      }
    }
  }

  // Verify we found all keys
  test_assert(count == num_items);
  for (int i = 0; i < num_items; i++) {
    test_assert(found_keys[i] == true);
  }

  sys_hashmap_finalize(map);
  return 0;
}

int test_hashmap_memory_management(void) {
  sys_puts("Test 8: Memory management and edge cases\n");

  sys_hashtable_t *map = sys_hashmap_init(2);
  test_assert(map != NULL);

  // Test transition from non-copied to copied key
  char test_key[] = "transition_key";
  test_assert(sys_hashmap_put(map, test_key, false, 100, NULL)); // Reference
  test_assert(sys_hashmap_get_value(map, test_key) == 100);

  uintptr_t replaced = 0;
  test_assert(sys_hashmap_put(map, test_key, true, 200, &replaced)); // Copy
  test_assert(replaced == 100);
  test_assert(sys_hashmap_get_value(map, test_key) == 200);

  // Test transition from short copied to long copied key
  char short_key[] = "short";
  test_assert(
      sys_hashmap_put(map, short_key, true, 300, NULL)); // Embedded copy
  test_assert(sys_hashmap_get_value(map, short_key) == 300);

  char long_replacement[] =
      "this_replacement_key_is_very_long_and_requires_malloc_storage_instead";
  test_assert(
      sys_hashmap_put(map, "short", true, 400,
                      &replaced)); // This should succeed - keys are the same
  test_assert(replaced == 300);    // Previous value should be returned
  test_assert(sys_hashmap_get_value(map, short_key) == 400); // Updated value

  // Add the long key separately
  test_assert(sys_hashmap_put(map, long_replacement, true, 450, NULL));
  test_assert(sys_hashmap_get_value(map, long_replacement) == 450);

  // Test overwriting long key with another long key
  test_assert(sys_hashmap_put(map, long_replacement, true, 500, &replaced));
  test_assert(replaced == 450);
  test_assert(sys_hashmap_get_value(map, long_replacement) == 500);

  sys_hashmap_finalize(map);
  return 0;
}

int test_hashmap_edge_cases(void) {
  sys_puts("Test 9: Edge cases and error conditions\n");

  sys_hashtable_t *map = sys_hashmap_init(4);
  test_assert(map != NULL);

  // Test empty string key
  test_assert(sys_hashmap_put(map, "", true, 100, NULL));
  test_assert(sys_hashmap_get_value(map, "") == 100);

  // Test single character key
  test_assert(sys_hashmap_put(map, "x", true, 200, NULL));
  test_assert(sys_hashmap_get_value(map, "x") == 200);

  // Test key with special characters
  test_assert(sys_hashmap_put(map, "key with spaces", true, 300, NULL));
  test_assert(sys_hashmap_get_value(map, "key with spaces") == 300);

  test_assert(sys_hashmap_put(map, "key@#$%^&*()", true, 400, NULL));
  test_assert(sys_hashmap_get_value(map, "key@#$%^&*()") == 400);

  // Test maximum value
  test_assert(sys_hashmap_put(map, "max_value", true, UINTPTR_MAX, NULL));
  test_assert(sys_hashmap_get_value(map, "max_value") == UINTPTR_MAX);

  // Test deleting and re-adding
  test_assert(sys_hashmap_delete(map, "x") == 200);
  test_assert(sys_hashmap_get_value(map, "x") == 0);
  test_assert(sys_hashmap_put(map, "x", true, 250, NULL));
  test_assert(sys_hashmap_get_value(map, "x") == 250);

  // Test count and capacity
  size_t initial_count = sys_hashtable_count(map);
  test_assert(initial_count > 0);

  size_t capacity = sys_hashtable_capacity(map);
  test_assert(capacity >= initial_count);

  sys_hashmap_finalize(map);
  return 0;
}
