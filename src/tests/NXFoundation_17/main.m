#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  printf("Starting NXNull tests...\n");

  // First create a zone for allocations
  NXZone *zone = [NXZone zoneWithSize:2048];
  test_assert(zone != nil);

  // Create an autorelease pool for memory management
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);

  // Test 1: Basic null value creation
  printf("\nTest 1: Testing basic null value creation...\n");
  NXNull *nullValue1 = [NXNull nullValue];
  test_assert(nullValue1 != nil);
  printf("✓ nullValue created successfully\n");

  // Test 2: Singleton behavior
  printf("\nTest 2: Testing singleton behavior...\n");
  NXNull *nullValue2 = [NXNull nullValue];
  test_assert(nullValue2 != nil);
  test_assert(nullValue1 == nullValue2); // Should be the same instance
  printf("✓ Singleton pattern working: same instance returned\n");

  // Test 3: Multiple calls return same instance
  printf("\nTest 3: Testing consistency of multiple calls...\n");
  NXNull *null1 = [NXNull nullValue];
  NXNull *null2 = [NXNull nullValue];
  NXNull *null3 = [NXNull nullValue];

  test_assert(null1 == null2);
  test_assert(null2 == null3);
  test_assert(null1 == null3);
  printf("✓ Multiple calls consistently return same singleton instance\n");

  // Test 4: Class type verification
  printf("\nTest 4: Testing class types...\n");
  test_assert([nullValue1 isKindOfClass:[NXNull class]]);
  test_assert([nullValue1 isKindOfClass:[NXObject class]]);
  printf("✓ NXNull is instance of correct classes\n");

  // Test 5: String representation (description)
  printf("\nTest 5: Testing string representation...\n");
  NXString *desc = [nullValue1 description];
  test_assert(desc != nil);

  const char *descStr = [desc cStr];
  printf("NXNull description: '%s'\n", descStr);

  // Should return "null" as per implementation
  test_assert(strcmp(descStr, "null") == 0);
  printf("✓ String representation is correct\n");

  // Test 6: Memory management with retain/release
  printf("\nTest 6: Testing memory management...\n");
  NXNull *ref1 = [NXNull nullValue];

  // Retain and release should be safe for singletons
  [ref1 retain];
  [ref1 release];

  // Verify singleton still works after retain/release
  NXNull *ref2 = [NXNull nullValue];
  test_assert(ref1 == ref2);
  printf("✓ Memory management works correctly with singleton\n");

  // Test 7: Equality comparison
  printf("\nTest 7: Testing equality comparisons...\n");
  NXNull *nullA = [NXNull nullValue];
  NXNull *nullB = [NXNull nullValue];

  // Pointer equality (since it's a singleton)
  test_assert(nullA == nullB);

  // Should be equal to itself
  test_assert([nullA isEqual:nullA]);
  printf("✓ Equality comparisons work correctly\n");

  // Test 8: Hash consistency
  printf("\nTest 8: Testing object identity...\n");
  NXNull *nullX = [NXNull nullValue];
  NXNull *nullY = [NXNull nullValue];

  // Same object should have same identity
  test_assert(nullX == nullY);
  printf("✓ Object identity is consistent for singleton\n");

  // Test 9: Autorelease behavior
  printf("\nTest 9: Testing autorelease behavior...\n");
  // The singleton should be in the autorelease pool as per implementation
  NXNull *autoreleaseNull = [NXNull nullValue];
  test_assert(autoreleaseNull != nil);

  // Create a new pool and verify singleton still works
  NXAutoreleasePool *newPool = [[NXAutoreleasePool alloc] init];
  NXNull *nullInNewPool = [NXNull nullValue];
  test_assert(nullInNewPool == autoreleaseNull); // Should be same singleton
  [newPool release];

  printf("✓ Autorelease behavior works correctly\n");

  // Test 10: Concurrent access safety
  printf("\nTest 10: Testing thread safety basics...\n");
  // Test multiple rapid calls (simulates concurrent access)
  NXNull *concurrentRefs[10];
  for (int i = 0; i < 10; i++) {
    concurrentRefs[i] = [NXNull nullValue];
  }

  // All should be the same instance
  for (int i = 1; i < 10; i++) {
    test_assert(concurrentRefs[0] == concurrentRefs[i]);
  }
  printf("✓ Rapid access returns consistent singleton instances\n");

  // Test 11: Description persistence
  printf("\nTest 11: Testing description method consistency...\n");
  NXString *desc1 = [nullValue1 description];
  NXString *desc2 = [nullValue2 description];

  // Both should return valid strings
  test_assert(desc1 != nil);
  test_assert(desc2 != nil);

  // Content should be the same
  test_assert(strcmp([desc1 cStr], [desc2 cStr]) == 0);
  printf("✓ Description method returns consistent results\n");

  // Test 12: Null value semantics
  printf("\nTest 12: Testing null value semantics...\n");
  NXNull *semanticNull = [NXNull nullValue];

  // Should not be nil (it's an actual object representing null)
  test_assert(semanticNull != nil);

  // But represents a null/empty value conceptually
  NXString *nullDesc = [semanticNull description];
  test_assert(strcmp([nullDesc cStr], "null") == 0);
  printf("✓ Null value semantics are correct\n");

  printf("\n=== All NXNull tests passed! ===\n");

  // Clean up
  [pool release];
  [zone release];

  return 0;
}
