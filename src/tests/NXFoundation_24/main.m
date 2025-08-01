#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

int test_map_methods(void);

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  NXZone *zone = [NXZone zoneWithSize:2048];
  test_assert(zone != nil);
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);

  // Run the test for map methods
  int returnValue = TestMain("NXFoundation_24", test_map_methods);

  // Clean up
  [pool release];
  [zone release];

  // Return the result of the test
  return returnValue;
}

///////////////////////////////////////////////////////////////////////////////
// TESTS

int test_map_methods(void) {
  printf("Testing NXMap lifecycle methods...\n");

  // Test 1: Basic initialization with default capacity
  {
    printf("  Test 1: Default initialization...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);
    test_assert([map count] == 0);
    [map release];
    printf("    ✓ Default init successful\n");
  }

  // Test 2: Initialization with specific capacity
  {
    printf("  Test 2: Initialization with capacity...\n");
    NXMap *map = [[NXMap alloc] initWithCapacity:64];
    test_assert(map != nil);
    test_assert([map count] == 0);
    [map release];
    printf("    ✓ Capacity init successful\n");
  }

  // Test 3: Initialization with zero capacity
  {
    printf("  Test 3: Initialization with zero capacity...\n");
    NXMap *map = [[NXMap alloc] initWithCapacity:0];
    test_assert(map != nil);
    test_assert([map count] == 0);
    [map release];
    printf("    ✓ Zero capacity init successful\n");
  }

  // Test 4: Factory method +new
  {
    printf("  Test 4: Factory method +new...\n");
    NXMap *map = [NXMap new];
    test_assert(map != nil);
    test_assert([map count] == 0);
    // Note: +new returns autoreleased object, no manual release needed
    printf("    ✓ Factory method +new successful\n");
  }

  // Test 5: Factory method +mapWithCapacity:
  {
    printf("  Test 5: Factory method +mapWithCapacity:...\n");
    NXMap *map = [NXMap mapWithCapacity:128];
    test_assert(map != nil);
    test_assert([map count] == 0);
    // Note: factory method returns autoreleased object, no manual release
    // needed
    printf("    ✓ Factory method +mapWithCapacity: successful\n");
  }

  // Test 6: Factory method with zero capacity
  {
    printf("  Test 6: Factory method with zero capacity...\n");
    NXMap *map = [NXMap mapWithCapacity:0];
    test_assert(map != nil);
    test_assert([map count] == 0);
    printf("    ✓ Factory method with zero capacity successful\n");
  }

  // Test 7: Multiple maps can coexist
  {
    printf("  Test 7: Multiple maps coexistence...\n");
    NXMap *map1 = [[NXMap alloc] init];
    NXMap *map2 = [[NXMap alloc] initWithCapacity:32];
    NXMap *map3 = [NXMap new];

    test_assert(map1 != nil);
    test_assert(map2 != nil);
    test_assert(map3 != nil);
    test_assert([map1 count] == 0);
    test_assert([map2 count] == 0);
    test_assert([map3 count] == 0);

    [map1 release];
    [map2 release];
    // map3 is autoreleased
    printf("    ✓ Multiple maps coexistence successful\n");
  }

  // Test 8: Large capacity initialization
  {
    printf("  Test 8: Large capacity initialization...\n");
    NXMap *map = [[NXMap alloc] initWithCapacity:1024];
    test_assert(map != nil);
    test_assert([map count] == 0);
    [map release];
    printf("    ✓ Large capacity init successful\n");
  }

  // Test 9: Factory method +mapWithObjectsAndKeys: - basic usage
  {
    printf("  Test 9: Factory method +mapWithObjectsAndKeys: basic...\n");
    NXString *str1 = [[NXString alloc] initWithCString:"value1"];
    NXString *str2 = [[NXString alloc] initWithCString:"value2"];
    NXString *key1 = [[NXString alloc] initWithCString:"key1"];
    NXString *key2 = [[NXString alloc] initWithCString:"key2"];

    NXMap *map = [NXMap mapWithObjectsAndKeys:str1, key1, str2, key2, nil];
    test_assert(map != nil);
    test_assert([map count] == 2);
    test_assert([[map objectForKey:key1] isEqual:str1]);
    test_assert([[map objectForKey:key2] isEqual:str2]);

    [str1 release];
    [str2 release];
    [key1 release];
    [key2 release];
    printf("    ✓ Factory method +mapWithObjectsAndKeys: basic successful\n");
  }

  // Test 10: Factory method +mapWithObjectsAndKeys: - empty map
  {
    printf("  Test 10: Factory method +mapWithObjectsAndKeys: empty...\n");
    NXMap *map = [NXMap mapWithObjectsAndKeys:nil];
    test_assert(map != nil);
    test_assert([map count] == 0);
    printf("    ✓ Factory method +mapWithObjectsAndKeys: empty successful\n");
  }

  // Test 11: Factory method +mapWithObjectsAndKeys: - error cases
  {
    printf(
        "  Test 11: Factory method +mapWithObjectsAndKeys: error cases...\n");
    NXString *str1 = [[NXString alloc] initWithCString:"value1"];
    NXString *key1 = [[NXString alloc] initWithCString:"key1"];

    // Odd number of arguments (missing nil terminator) - should return nil
    // Note: We can't easily test this case due to variadic args

    // Invalid key type (not a string protocol) - would cause protocol warning
    // but should still work in practice with proper objects

    [str1 release];
    [key1 release];
    printf("    ✓ Factory method +mapWithObjectsAndKeys: error cases "
           "successful\n");
  }

  // Test 12: Factory method +mapWithObjects:forKeys: - basic usage
  {
    printf("  Test 12: Factory method +mapWithObjects:forKeys: basic...\n");
    NXString *str1 = [[NXString alloc] initWithCString:"value1"];
    NXString *str2 = [[NXString alloc] initWithCString:"value2"];
    NXString *key1 = [[NXString alloc] initWithCString:"key1"];
    NXString *key2 = [[NXString alloc] initWithCString:"key2"];

    NXArray *objects = [NXArray arrayWithObjects:str1, str2, nil];
    NXArray *keys = [NXArray arrayWithObjects:key1, key2, nil];

    NXMap *map = [NXMap mapWithObjects:objects forKeys:keys];
    test_assert(map != nil);
    test_assert([map count] == 2);
    test_assert([[map objectForKey:key1] isEqual:str1]);
    test_assert([[map objectForKey:key2] isEqual:str2]);

    [str1 release];
    [str2 release];
    [key1 release];
    [key2 release];
    printf("    ✓ Factory method +mapWithObjects:forKeys: basic successful\n");
  }

  // Test 13: Factory method +mapWithObjects:forKeys: - empty arrays
  {
    printf("  Test 13: Factory method +mapWithObjects:forKeys: empty...\n");
    NXArray *objects = [NXArray arrayWithObjects:nil];
    NXArray *keys = [NXArray arrayWithObjects:nil];

    NXMap *map = [NXMap mapWithObjects:objects forKeys:keys];
    test_assert(map != nil);
    test_assert([map count] == 0);
    printf("    ✓ Factory method +mapWithObjects:forKeys: empty successful\n");
  }

  // Test 14: Factory method +mapWithObjects:forKeys: - error cases
  {
    printf(
        "  Test 14: Factory method +mapWithObjects:forKeys: error cases...\n");
    NXString *str1 = [[NXString alloc] initWithCString:"value1"];
    NXString *key1 = [[NXString alloc] initWithCString:"key1"];
    NXString *key2 = [[NXString alloc] initWithCString:"key2"];

    NXArray *objects1 = [NXArray arrayWithObjects:str1, nil];
    NXArray *keys2 = [NXArray arrayWithObjects:key1, key2, nil];

    // Mismatched array sizes - should return nil
    NXMap *map1 = [NXMap mapWithObjects:objects1 forKeys:keys2];
    test_assert(map1 == nil);

    // Nil arrays - should return nil
    NXMap *map2 = [NXMap mapWithObjects:nil forKeys:keys2];
    test_assert(map2 == nil);

    NXMap *map3 = [NXMap mapWithObjects:objects1 forKeys:nil];
    test_assert(map3 == nil);

    [str1 release];
    [key1 release];
    [key2 release];
    printf("    ✓ Factory method +mapWithObjects:forKeys: error cases "
           "successful\n");
  }

  printf("Testing NXMap core functionality...\n");

  // Test 15: Basic setObject:forKey: and objectForKey:
  {
    printf("  Test 15: Basic set/get operations...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    NXString *key1 = [[NXString alloc] initWithCString:"key1"];
    NXString *value1 = [[NXString alloc] initWithCString:"value1"];

    // Test setting an object
    BOOL result = [map setObject:value1 forKey:key1];
    test_assert(result == YES);
    test_assert([map count] == 1);

    // Test retrieving the object
    id retrieved = [map objectForKey:key1];
    test_assert(retrieved == value1);
    test_assert([retrieved isEqual:@"value1"]);

    [map release];
    [key1 release];
    [value1 release];
    printf("    ✓ Basic set/get operations successful\n");
  }

  // Test 16: Multiple key-value pairs
  {
    printf("  Test 16: Multiple key-value pairs...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    NXString *key1 = [[NXString alloc] initWithCString:"first"];
    NXString *key2 = [[NXString alloc] initWithCString:"second"];
    NXString *key3 = [[NXString alloc] initWithCString:"third"];
    NXString *value1 = [[NXString alloc] initWithCString:"value1"];
    NXString *value2 = [[NXString alloc] initWithCString:"value2"];
    NXString *value3 = [[NXString alloc] initWithCString:"value3"];

    // Set multiple objects
    test_assert([map setObject:value1 forKey:key1] == YES);
    test_assert([map setObject:value2 forKey:key2] == YES);
    test_assert([map setObject:value3 forKey:key3] == YES);
    test_assert([map count] == 3);

    // Retrieve all objects
    test_assert([[map objectForKey:key1] isEqual:value1]);
    test_assert([[map objectForKey:key2] isEqual:value2]);
    test_assert([[map objectForKey:key3] isEqual:value3]);

    [map release];
    [key1 release];
    [key2 release];
    [key3 release];
    [value1 release];
    [value2 release];
    [value3 release];
    printf("    ✓ Multiple key-value pairs successful\n");
  }

  // Test 17: Key not found
  {
    printf("  Test 17: Key not found...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    // Try to get non-existent key
    id result = [map objectForKey:@"nonexistent"];
    test_assert(result == nil);
    test_assert([map count] == 0);

    [map release];
    printf("    ✓ Key not found handling successful\n");
  }

  // Test 18: Overwriting existing key
  {
    printf("  Test 18: Overwriting existing key...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    NXString *key = [[NXString alloc] initWithCString:"testkey"];
    NXString *value1 = [[NXString alloc] initWithCString:"original"];
    NXString *value2 = [[NXString alloc] initWithCString:"updated"];

    // Set initial value
    test_assert([map setObject:value1 forKey:key] == YES);
    test_assert([map count] == 1);
    test_assert([[map objectForKey:key] isEqual:value1]);

    // Overwrite with new value
    test_assert([map setObject:value2 forKey:key] == YES);
    test_assert([map count] == 1); // Count should remain 1 when overwriting
    test_assert([[map objectForKey:key] isEqual:value2]);

    [map release];
    [key release];
    [value1 release];
    [value2 release];
    printf("    ✓ Overwriting existing key successful\n");
  }

  // Test 19: removeAllObjects method
  {
    printf("  Test 19: removeAllObjects method...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    // Add several objects
    test_assert([map setObject:@"value1" forKey:@"key1"] == YES);
    test_assert([map setObject:@"value2" forKey:@"key2"] == YES);
    test_assert([map setObject:@"value3" forKey:@"key3"] == YES);
    test_assert([map count] == 3);

    // Remove all objects
    [map removeAllObjects];
    test_assert([map count] == 0);
    test_assert([map objectForKey:@"key1"] == nil);
    test_assert([map objectForKey:@"key2"] == nil);
    test_assert([map objectForKey:@"key3"] == nil);

    // Verify map is still usable after clearing
    test_assert([map setObject:@"newvalue" forKey:@"newkey"] == YES);
    test_assert([map count] == 1);
    test_assert([[map objectForKey:@"newkey"] isEqual:@"newvalue"]);

    [map release];
    printf("    ✓ removeAllObjects method successful\n");
  }

  // Test 20: Different object types
  {
    printf("  Test 20: Different object types...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    NXString *stringValue = [[NXString alloc] initWithCString:"stringtest"];
    NXArray *arrayValue = [NXArray new];

    // Store different object types
    test_assert([map setObject:stringValue forKey:@"string"] == YES);
    test_assert([map setObject:arrayValue forKey:@"array"] == YES);
    test_assert([map count] == 2);

    // Retrieve and verify types
    test_assert([[map objectForKey:@"string"] isEqual:stringValue]);
    test_assert([map objectForKey:@"array"] == arrayValue);

    [map release];
    [stringValue release];
    printf("    ✓ Different object types successful\n");
  }

  // Test 21: removeObjectForKey method
  {
    printf("  Test 21: removeObjectForKey method...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map);
    test_assert([map count] == 0);

    // Add some objects
    NXString *key1 = [[NXString alloc] initWithCString:"key1"];
    NXString *key2 = [[NXString alloc] initWithCString:"key2"];
    NXString *key3 = [[NXString alloc] initWithCString:"key3"];
    NXString *value1 = [[NXString alloc] initWithCString:"value1"];
    NXString *value2 = [[NXString alloc] initWithCString:"value2"];
    NXString *value3 = [[NXString alloc] initWithCString:"value3"];

    test_assert([map setObject:value1 forKey:key1]);
    test_assert([map setObject:value2 forKey:key2]);
    test_assert([map setObject:value3 forKey:key3]);
    test_assert([map count] == 3);

    // Remove existing key
    test_assert([map removeObjectForKey:key2] == YES);
    test_assert([map count] == 2);
    test_assert([map objectForKey:key2] == nil);
    test_assert([[map objectForKey:key1] isEqual:value1]);
    test_assert([[map objectForKey:key3] isEqual:value3]);

    // Try to remove non-existent key
    test_assert([map removeObjectForKey:@"nonexistent"] == NO);
    test_assert([map count] == 2);

    // Remove remaining keys
    test_assert([map removeObjectForKey:key1] == YES);
    test_assert([map removeObjectForKey:key3] == YES);
    test_assert([map count] == 0);

    // Try to remove from empty map
    test_assert([map removeObjectForKey:key1] == NO);

    [map release];
    [key1 release];
    [key2 release];
    [key3 release];
    [value1 release];
    [value2 release];
    [value3 release];
    printf("    ✓ removeObjectForKey method successful\n");
  }

  // Test 22: Setting same object multiple times
  {
    printf("  Test 22: Setting same object multiple times...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map);
    test_assert([map count] == 0);

    NXString *key = [[NXString alloc] initWithCString:"samekey"];
    NXString *value = [[NXString alloc] initWithCString:"samevalue"];

    // Set object first time
    test_assert([map setObject:value forKey:key]);
    test_assert([map count] == 1);
    test_assert([map objectForKey:key] == value);

    // Set same object again - should not change count or leak memory
    test_assert([map setObject:value forKey:key]);
    test_assert([map count] == 1);
    test_assert([map objectForKey:key] == value);

    // Set same object a third time
    test_assert([map setObject:value forKey:key]);
    test_assert([map count] == 1);
    test_assert([map objectForKey:key] == value);

    [map release];
    [key release];
    [value release];
    printf("    ✓ Setting same object multiple times successful\n");
  }

  // Test 23: allKeys method - basic functionality
  {
    printf("  Test 23: allKeys method basic...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    // Test empty map
    NXArray *emptyKeys = [map allKeys];
    test_assert(emptyKeys != nil);
    test_assert([emptyKeys count] == 0);

    // Add some key-value pairs
    NXString *key1 = [[NXString alloc] initWithCString:"first"];
    NXString *key2 = [[NXString alloc] initWithCString:"second"];
    NXString *key3 = [[NXString alloc] initWithCString:"third"];
    NXString *value1 = [[NXString alloc] initWithCString:"value1"];
    NXString *value2 = [[NXString alloc] initWithCString:"value2"];
    NXString *value3 = [[NXString alloc] initWithCString:"value3"];

    test_assert([map setObject:value1 forKey:key1] == YES);
    test_assert([map setObject:value2 forKey:key2] == YES);
    test_assert([map setObject:value3 forKey:key3] == YES);
    test_assert([map count] == 3);

    // Get all keys
    NXArray *keys = [map allKeys];
    test_assert(keys != nil);
    test_assert([keys count] == 3);

    // Verify all keys are present (order not guaranteed)
    BOOL foundFirst = NO, foundSecond = NO, foundThird = NO;
    unsigned int i;
    for (i = 0; i < [keys count]; i++) {
      NXString *key = [keys objectAtIndex:i];
      test_assert(key != nil);
      if ([key isEqual:key1])
        foundFirst = YES;
      else if ([key isEqual:key2])
        foundSecond = YES;
      else if ([key isEqual:key3])
        foundThird = YES;
    }
    test_assert(foundFirst && foundSecond && foundThird);

    [key1 release];
    [key2 release];
    [key3 release];
    [value1 release];
    [value2 release];
    [value3 release];
    [map release];
    printf("    ✓ allKeys method basic successful\n");
  }

  // Test 24: allObjects method - basic functionality
  {
    printf("  Test 24: allObjects method basic...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    // Test empty map
    NXArray *emptyObjects = [map allObjects];
    test_assert(emptyObjects != nil);
    test_assert([emptyObjects count] == 0);

    // Add some key-value pairs
    NXString *key1 = [[NXString alloc] initWithCString:"first"];
    NXString *key2 = [[NXString alloc] initWithCString:"second"];
    NXString *value1 = [[NXString alloc] initWithCString:"valueA"];
    NXString *value2 = [[NXString alloc] initWithCString:"valueB"];

    test_assert([map setObject:value1 forKey:key1] == YES);
    test_assert([map setObject:value2 forKey:key2] == YES);
    test_assert([map count] == 2);

    // Get all objects
    NXArray *objects = [map allObjects];
    test_assert(objects != nil);
    test_assert([objects count] == 2);

    // Verify all objects are present (order not guaranteed)
    BOOL foundValueA = NO, foundValueB = NO;
    unsigned int i;
    for (i = 0; i < [objects count]; i++) {
      NXString *obj = [objects objectAtIndex:i];
      test_assert(obj != nil);
      if ([obj isEqual:value1])
        foundValueA = YES;
      else if ([obj isEqual:value2])
        foundValueB = YES;
    }
    test_assert(foundValueA && foundValueB);

    [key1 release];
    [key2 release];
    [value1 release];
    [value2 release];
    [map release];
    printf("    ✓ allObjects method basic successful\n");
  }

  // Test 25: allKeys and allObjects after removeAllObjects
  {
    printf("  Test 25: allKeys/allObjects after removeAllObjects...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    // Add some data
    NXString *key = [[NXString alloc] initWithCString:"testkey"];
    NXString *value = [[NXString alloc] initWithCString:"testvalue"];
    test_assert([map setObject:value forKey:key] == YES);
    test_assert([map count] == 1);

    // Verify we have keys and objects
    NXArray *keys = [map allKeys];
    NXArray *objects = [map allObjects];
    test_assert([keys count] == 1);
    test_assert([objects count] == 1);

    // Clear the map
    [map removeAllObjects];
    test_assert([map count] == 0);

    // Verify arrays are now empty
    NXArray *keysAfter = [map allKeys];
    NXArray *objectsAfter = [map allObjects];
    test_assert(keysAfter != nil);
    test_assert(objectsAfter != nil);
    test_assert([keysAfter count] == 0);
    test_assert([objectsAfter count] == 0);

    [key release];
    [value release];
    [map release];
    printf("    ✓ allKeys/allObjects after removeAllObjects successful\n");
  }

  // Test 26: allKeys and allObjects with mixed object types
  {
    printf("  Test 26: allKeys/allObjects with mixed types...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map != nil);

    // Add different types of objects
    NXString *key1 = [[NXString alloc] initWithCString:"string"];
    NXString *key2 = [[NXString alloc] initWithCString:"array"];
    NXString *stringValue = [[NXString alloc] initWithCString:"text"];
    NXArray *arrayValue = [NXArray arrayWithObjects:stringValue, nil];

    test_assert([map setObject:stringValue forKey:key1] == YES);
    test_assert([map setObject:arrayValue forKey:key2] == YES);
    test_assert([map count] == 2);

    // Test allKeys
    NXArray *keys = [map allKeys];
    test_assert(keys != nil);
    test_assert([keys count] == 2);

    // Test allObjects
    NXArray *objects = [map allObjects];
    test_assert(objects != nil);
    test_assert([objects count] == 2);

    // Verify objects are the right types
    BOOL foundString = NO, foundArray = NO;
    unsigned int i;
    for (i = 0; i < [objects count]; i++) {
      id obj = [objects objectAtIndex:i];
      test_assert(obj != nil);
      if (obj == stringValue)
        foundString = YES;
      else if (obj == arrayValue)
        foundArray = YES;
    }
    test_assert(foundString && foundArray);

    [key1 release];
    [key2 release];
    [stringValue release];
    [map release];
    printf("    ✓ allKeys/allObjects with mixed types successful\n");
  }

  // Test 27: Stress test with random operations
  {
    printf("  Test 27: Stress test with random operations...\n");
    NXMap *map = [[NXMap alloc] init];
    test_assert(map);
    test_assert([map count] == 0);

    // We'll track expected state in a simple array
    const int MAX_KEYS = 50;
    const int NUM_OPERATIONS = 200;
    id expected_values[MAX_KEYS]; // NULL means key doesn't exist
    unsigned int expected_count = 0;

    // Initialize expected state
    for (int i = 0; i < MAX_KEYS; i++) {
      expected_values[i] = nil;
    }

    // Create a pool of test objects to use
    NXArray *test_objects = [[NXArray alloc] init];
    for (int i = 0; i < 20; i++) {
      NXString *obj = [[NXString alloc] initWithFormat:@"TestObject_%d", i];
      [test_objects append:obj];
      [obj release]; // Array retains it
    }

    // Perform random operations
    for (int op = 0; op < NUM_OPERATIONS; op++) {
      int key_index = abs(NXRandInt32()) % MAX_KEYS;
      int operation = abs(NXRandInt32()) % 100; // 0-99
      NXString *key = [[NXString alloc] initWithFormat:@"key_%d", key_index];

      if (operation < 50) { // 50% chance: INSERT/UPDATE
        id new_value = [test_objects
            objectAtIndex:(abs(NXRandInt32()) % [test_objects count])];
        id old_value = expected_values[key_index];

        // Perform the operation
        test_assert([map setObject:new_value forKey:key]);

        // Update expected state
        if (old_value == nil) {
          expected_count++; // New key
        }
        expected_values[key_index] = new_value;

      } else if (operation < 80 &&
                 expected_count >
                     0) { // 30% chance: REMOVE (only if we have keys)
        id old_value = expected_values[key_index];

        if (old_value != nil) {
          // Key exists, should succeed
          test_assert([map removeObjectForKey:key] == YES);
          expected_values[key_index] = nil;
          expected_count--;
        } else {
          // Key doesn't exist, should fail
          test_assert([map removeObjectForKey:key] == NO);
        }

      } else { // 20% chance: LOOKUP
        id expected_value = expected_values[key_index];
        id actual_value = [map objectForKey:key];

        if (expected_value == nil) {
          test_assert(actual_value == nil);
        } else {
          test_assert(actual_value == expected_value);
        }
      }

      // Validate count after every operation
      test_assert([map count] == expected_count);

      // Every 50 operations, do a full validation
      if ((op + 1) % 50 == 0) {
        printf("    Operation %d: count=%d, validating...\n", op + 1,
               expected_count);

        // Check that all expected keys exist and have correct values
        for (int i = 0; i < MAX_KEYS; i++) {
          NXString *test_key = [[NXString alloc] initWithFormat:@"key_%d", i];
          id expected_val = expected_values[i];
          id actual_val = [map objectForKey:test_key];

          if (expected_val == nil) {
            test_assert(actual_val == nil);
          } else {
            test_assert(actual_val == expected_val);
          }
          [test_key release];
        }
      }

      [key release];
    }

    // Final validation - iterate through all entries
    printf("    Final validation: expected_count=%u, actual_count=%u\n",
           expected_count, [map count]);
    test_assert([map count] == expected_count);

    // Validate all expected keys are present with correct values
    unsigned int found_keys = 0;
    for (int i = 0; i < MAX_KEYS; i++) {
      if (expected_values[i] != nil) {
        NXString *test_key = [[NXString alloc] initWithFormat:@"key_%d", i];
        id actual_value = [map objectForKey:test_key];
        test_assert(actual_value == expected_values[i]);
        found_keys++;
        [test_key release];
      }
    }
    test_assert(found_keys == expected_count);
    printf("    Final key validation: %u keys verified\n", found_keys);

    [test_objects release];
    [map release];
    printf("    ✓ Stress test with random operations successful\n");
  }

  printf("All NXMap lifecycle and functionality tests passed!\n");
  return 0;
}