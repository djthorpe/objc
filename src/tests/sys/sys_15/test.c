#include "../../runtime-sys/all/hashtable.h"
#include <runtime-sys/sys.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <tests/tests.h>

// Test helpers
static bool test_key_equals(void *keyptr, void *other_keyptr) {
  char *key1 = (char *)keyptr;
  char *key2 = (char *)other_keyptr;
  if (key1 == NULL || key2 == NULL)
    return false;
  return strcmp(key1, key2) == 0;
}

static uintptr_t hash_string(const char *str) {
  uintptr_t hash = 5381;
  while (*str) {
    hash = ((hash << 5) + hash) + *str++;
  }
  return hash;
}

// Forward declarations
int test_sys_15(void);
int test_hashtable_basic_operations(void);
int test_hashtable_collisions(void);
int test_hashtable_chaining(void);
int test_hashtable_deletion(void);
int test_hashtable_iteration(void);
int test_hashtable_edge_cases(void);
int test_hashtable_count_capacity(void);

int test_sys_15(void) {
  sys_puts("Running hash table tests...\n");

  test_hashtable_basic_operations();
  test_hashtable_collisions();
  test_hashtable_chaining();
  test_hashtable_deletion();
  test_hashtable_iteration();
  test_hashtable_edge_cases();
  test_hashtable_count_capacity();

  sys_puts("All hash table tests passed!\n");
  return 0;
}

int test_hashtable_basic_operations(void) {
  sys_puts("Test 1: Basic hash table operations\n");

  // Initialize a small hash table
  sys_hashtable_t *table = sys_hashtable_init(4, test_key_equals);
  test_assert(table != NULL);

  // Test putting values
  char *key1 = "hello";
  char *key2 = "world";
  uintptr_t value1 = 100;
  uintptr_t value2 = 200;

  sys_hashtable_entry_t *entry1 =
      sys_hashtable_put(table, hash_string(key1), key1, NULL);
  test_assert(entry1 != NULL);
  entry1->value = value1;
  test_assert(entry1->value == value1);
  test_assert(entry1->keyptr == key1);

  sys_hashtable_entry_t *entry2 =
      sys_hashtable_put(table, hash_string(key2), key2, NULL);
  test_assert(entry2 != NULL);
  entry2->value = value2;
  test_assert(entry2->value == value2);
  test_assert(entry2->keyptr == key2);

  // Test getting values by key
  sys_hashtable_entry_t *found1 =
      sys_hashtable_get_key(table, hash_string(key1), key1);
  test_assert(found1 != NULL);
  test_assert(found1->value == value1);
  test_assert(found1 == entry1);

  sys_hashtable_entry_t *found2 =
      sys_hashtable_get_key(table, hash_string(key2), key2);
  test_assert(found2 != NULL);
  test_assert(found2->value == value2);
  test_assert(found2 == entry2);

  // Test getting values by value
  sys_hashtable_entry_t *found_by_value1 =
      sys_hashtable_get_value(table, value1);
  test_assert(found_by_value1 != NULL);
  test_assert(found_by_value1 == entry1);

  sys_hashtable_entry_t *found_by_value2 =
      sys_hashtable_get_value(table, value2);
  test_assert(found_by_value2 != NULL);
  test_assert(found_by_value2 == entry2);

  // Test non-existent key
  char *nonexistent_key = "nonexistent";
  sys_hashtable_entry_t *not_found = sys_hashtable_get_key(
      table, hash_string(nonexistent_key), nonexistent_key);
  test_assert(not_found == NULL);

  // Clean up
  sys_hashtable_finalize(table);
  return 0;
}

int test_hashtable_collisions(void) {
  sys_puts("Test 2: Hash collision handling\n");

  // Initialize with size 2 to force collisions
  sys_hashtable_t *table = sys_hashtable_init(2, test_key_equals);
  test_assert(table != NULL);

  // Create keys that will likely collide in a small table
  char *keys[] = {"a", "b", "c", "d"};
  uintptr_t values[] = {10, 20, 30, 40};

  // Insert multiple keys
  for (int i = 0; i < 4; i++) {
    sys_hashtable_entry_t *entry =
        sys_hashtable_put(table, hash_string(keys[i]), keys[i], NULL);
    test_assert(entry != NULL);
    entry->value = values[i];
    test_assert(entry->value == values[i]);
  }

  // Verify all keys can be found
  for (int i = 0; i < 4; i++) {
    sys_hashtable_entry_t *found =
        sys_hashtable_get_key(table, hash_string(keys[i]), keys[i]);
    test_assert(found != NULL);
    test_assert(found->value == values[i]);
  }

  // Clean up
  sys_hashtable_finalize(table);
  return 0;
}

int test_hashtable_chaining(void) {
  sys_puts("Test 3: Hash table chaining (growth)\n");

  // Initialize with tiny size to force chaining
  sys_hashtable_t *table = sys_hashtable_init(1, test_key_equals);
  test_assert(table != NULL);

  // Insert enough items to trigger chain creation
  char *keys[] = {"key1", "key2", "key3", "key4", "key5"};
  uintptr_t values[] = {100, 200, 300, 400, 500};

  for (int i = 0; i < 5; i++) {
    sys_hashtable_entry_t *entry =
        sys_hashtable_put(table, hash_string(keys[i]), keys[i], NULL);
    test_assert(entry != NULL);
    entry->value = values[i];
    test_assert(entry->value == values[i]);
  }

  // Verify all keys still accessible after chaining
  for (int i = 0; i < 5; i++) {
    sys_hashtable_entry_t *found =
        sys_hashtable_get_key(table, hash_string(keys[i]), keys[i]);
    test_assert(found != NULL);
    test_assert(found->value == values[i]);
  }

  // Note: We can't directly test chaining structure since hashtable is opaque,
  // but the fact that all 5 items can be stored and retrieved in a size-1 table
  // proves that chaining is working correctly

  // Clean up
  sys_hashtable_finalize(table);
  return 0;
}

int test_hashtable_deletion(void) {
  sys_puts("Test 4: Hash table deletion\n");

  sys_hashtable_t *table = sys_hashtable_init(8, test_key_equals);
  test_assert(table != NULL);

  // Insert some items
  char *keys[] = {"apple", "banana", "cherry", "date"};
  uintptr_t values[] = {1, 2, 3, 4};

  for (int i = 0; i < 4; i++) {
    sys_hashtable_entry_t *entry =
        sys_hashtable_put(table, hash_string(keys[i]), keys[i], NULL);
    test_assert(entry != NULL);
    entry->value = values[i];
  }

  // Test deletion by key
  sys_hashtable_entry_t *deleted =
      sys_hashtable_delete_key(table, hash_string(keys[1]), keys[1]);
  test_assert(deleted != NULL);
  test_assert(IS_DELETED(deleted) == true);

  // Verify deleted item cannot be found
  sys_hashtable_entry_t *not_found =
      sys_hashtable_get_key(table, hash_string(keys[1]), keys[1]);
  test_assert(not_found == NULL);

  // Test deletion by value
  sys_hashtable_entry_t *deleted_by_value =
      sys_hashtable_delete_value(table, values[2]);
  test_assert(deleted_by_value != NULL);
  test_assert(IS_DELETED(deleted_by_value) == true);

  // Verify deleted item cannot be found by value
  sys_hashtable_entry_t *not_found_by_value =
      sys_hashtable_get_value(table, values[2]);
  test_assert(not_found_by_value == NULL);

  // Verify remaining items still accessible
  sys_hashtable_entry_t *still_there1 =
      sys_hashtable_get_key(table, hash_string(keys[0]), keys[0]);
  test_assert(still_there1 != NULL);
  test_assert(still_there1->value == values[0]);

  sys_hashtable_entry_t *still_there2 =
      sys_hashtable_get_key(table, hash_string(keys[3]), keys[3]);
  test_assert(still_there2 != NULL);
  test_assert(still_there2->value == values[3]);

  // Test deleting non-existent key
  char *nonexistent = "nonexistent";
  sys_hashtable_entry_t *not_deleted =
      sys_hashtable_delete_key(table, hash_string(nonexistent), nonexistent);
  test_assert(not_deleted == NULL);

  // Clean up
  sys_hashtable_finalize(table);
  return 0;
}

int test_hashtable_iteration(void) {
  sys_puts("Test 5: Hash table iteration\n");

  sys_hashtable_t *table = sys_hashtable_init(4, test_key_equals);
  test_assert(table != NULL);

  // Insert some items
  char *keys[] = {"one", "two", "three"};
  uintptr_t values[] = {1, 2, 3};

  for (int i = 0; i < 3; i++) {
    sys_hashtable_entry_t *entry =
        sys_hashtable_put(table, hash_string(keys[i]), keys[i], NULL);
    test_assert(entry != NULL);
    entry->value = values[i];
  }

  // Test iteration
  sys_hashtable_iterator_t iterator = {0};
  sys_hashtable_iterator_t *iter = &iterator;

  int count = 0;
  bool found_values[3] = {false, false, false};

  sys_hashtable_entry_t *entry;
  while ((entry = sys_hashtable_iterator_next(table, &iter)) != NULL) {
    count++;

    // Mark which values we've seen
    for (int i = 0; i < 3; i++) {
      if (entry->value == values[i]) {
        found_values[i] = true;
      }
    }
  }

  // Verify we found all items
  test_assert(count == 3);
  for (int i = 0; i < 3; i++) {
    test_assert(found_values[i] == true);
  }

  // Test iteration after deletion
  sys_hashtable_delete_key(table, hash_string(keys[1]), keys[1]);

  // Reset iterator
  iterator = (sys_hashtable_iterator_t){0};
  iter = &iterator;

  count = 0;
  while ((entry = sys_hashtable_iterator_next(table, &iter)) != NULL) {
    count++;
    // Should not find the deleted value
    test_assert(entry->value != values[1]);
  }

  // Should find 2 remaining items
  test_assert(count == 2);

  // Clean up
  sys_hashtable_finalize(table);
  return 0;
}

int test_hashtable_edge_cases(void) {
  sys_puts("Test 6: Edge cases and bool *samekey parameter\n");

  sys_hashtable_t *table = sys_hashtable_init(4, test_key_equals);
  test_assert(table != NULL);

  // Test samekey parameter
  char *key = "test_key";
  uintptr_t value1 = 100;
  uintptr_t value2 = 200;

  // Insert initial value
  bool samekey1 = false;
  sys_hashtable_entry_t *entry1 =
      sys_hashtable_put(table, hash_string(key), key, &samekey1);
  test_assert(entry1 != NULL);
  test_assert(samekey1 == false); // New key
  entry1->value = value1;

  // Replace with same key
  bool samekey2 = false;
  sys_hashtable_entry_t *entry2 =
      sys_hashtable_put(table, hash_string(key), key, &samekey2);
  test_assert(entry2 != NULL);
  test_assert(entry2 == entry1); // Should be same slot
  test_assert(samekey2 == true); // Existing key
  uintptr_t old_value = entry2->value;
  test_assert(old_value == value1); // Should have old value
  entry2->value = value2;           // Update value

  // Test duplicate insertion with same value
  bool samekey3 = false;
  sys_hashtable_entry_t *entry3 =
      sys_hashtable_put(table, hash_string(key), key, &samekey3);
  test_assert(entry3 != NULL);
  test_assert(samekey3 == true); // Existing key

  // Test without key comparison function (hash-only comparison)
  sys_hashtable_t *hash_only_table = sys_hashtable_init(4, NULL);
  test_assert(hash_only_table != NULL);

  uintptr_t test_hash = 12345;
  uintptr_t test_value = 999;

  bool samekey_hash = false;
  sys_hashtable_entry_t *hash_entry =
      sys_hashtable_put(hash_only_table, test_hash, NULL, &samekey_hash);
  test_assert(hash_entry != NULL);
  test_assert(samekey_hash == false); // New entry
  hash_entry->value = test_value;

  sys_hashtable_entry_t *found_hash =
      sys_hashtable_get_key(hash_only_table, test_hash, NULL);
  test_assert(found_hash != NULL);
  test_assert(found_hash->value == test_value);

  // Clean up
  sys_hashtable_finalize(table);
  sys_hashtable_finalize(hash_only_table);
  return 0;
}

int test_hashtable_count_capacity(void) {
  sys_puts("Test 7: Hash table count and capacity\n");

  // Initialize with very small size to force chaining quickly
  sys_hashtable_t *table = sys_hashtable_init(1, test_key_equals);
  test_assert(table != NULL);

  // Test initial state
  test_assert(sys_hashtable_count(table) == 0);
  test_assert(sys_hashtable_capacity(table) == 1);

  // Add first entry
  char *key1 = "key1";
  uintptr_t value1 = 100;
  sys_hashtable_entry_t *entry1 =
      sys_hashtable_put(table, hash_string(key1), key1, NULL);
  test_assert(entry1 != NULL);
  entry1->value = value1;

  // Test count and capacity after one entry
  test_assert(sys_hashtable_count(table) == 1);
  test_assert(sys_hashtable_capacity(table) == 1);

  // Add second entry - this should force chaining since table size is 1
  char *key2 = "key2";
  uintptr_t value2 = 200;
  sys_hashtable_entry_t *entry2 =
      sys_hashtable_put(table, hash_string(key2), key2, NULL);
  test_assert(entry2 != NULL);
  entry2->value = value2;

  // Test count after adding second entry
  test_assert(sys_hashtable_count(table) == 2);

  // Capacity should have increased due to chaining (1 + new table size)
  size_t final_capacity = sys_hashtable_capacity(table);
  test_assert(final_capacity > 1);

  // Add more entries to test scaling
  char *keys[] = {"key3", "key4", "key5"};
  uintptr_t values[] = {300, 400, 500};

  for (int i = 0; i < 3; i++) {
    sys_hashtable_entry_t *entry =
        sys_hashtable_put(table, hash_string(keys[i]), keys[i], NULL);
    test_assert(entry != NULL);
    entry->value = values[i];
  }

  // Test final count
  test_assert(sys_hashtable_count(table) == 5);

  // Capacity should be at least 5 or more
  size_t total_capacity = sys_hashtable_capacity(table);
  test_assert(total_capacity >= 5);

  // Test deletion impact on count
  sys_hashtable_entry_t *deleted =
      sys_hashtable_delete_key(table, hash_string(key1), key1);
  test_assert(deleted != NULL);

  // Count should decrease, capacity should remain the same
  test_assert(sys_hashtable_count(table) == 4);
  test_assert(sys_hashtable_capacity(table) == total_capacity);

  // Test deletion by value
  sys_hashtable_entry_t *deleted_by_value =
      sys_hashtable_delete_value(table, value2);
  test_assert(deleted_by_value != NULL);

  // Count should decrease again
  test_assert(sys_hashtable_count(table) == 3);
  test_assert(sys_hashtable_capacity(table) == total_capacity);

  // Clean up
  sys_hashtable_finalize(table);
  return 0;
}
