#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <string.h>
#include <tests/tests.h>

// Removed forward declaration of NXNumberBool to avoid tight coupling.

int main() {
  printf("Starting NXNumber boolean value tests...\n");

  // First create a zone for allocations
  NXZone *zone = [NXZone zoneWithSize:2048];
  test_assert(zone != nil);

  // Create an autorelease pool for memory management
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);

  // Test 1: Basic boolean number creation
  printf("\nTest 1: Testing basic boolean number creation...\n");
  NXNumber *trueNum = [NXNumber trueValue];
  NXNumber *falseNum = [NXNumber falseValue];

  test_assert(trueNum != nil);
  test_assert(falseNum != nil);
  printf("✓ Both trueValue and falseValue created successfully\n");

  // Test 2: Factory method numberWithBool:
  printf("\nTest 2: Testing numberWithBool: factory method...\n");
  NXNumber *boolYes = [NXNumber numberWithBool:YES];
  NXNumber *boolNo = [NXNumber numberWithBool:NO];

  test_assert(boolYes != nil);
  test_assert(boolNo != nil);
  printf("✓ numberWithBool: creates valid objects for YES and NO\n");

  // Test 3: Singleton behavior
  printf("\nTest 3: Testing singleton behavior...\n");
  test_assert(trueNum == boolYes); // Should be the same instance
  test_assert(falseNum == boolNo); // Should be the same instance
  printf("✓ Singleton pattern working: same instances for same values\n");

  // Test 4: Different values are different instances
  printf("\nTest 4: Testing different values are different instances...\n");
  test_assert(trueNum != falseNum);
  printf("✓ True and false values are different instances\n");

  // Test 5: Class type verification
  printf("\nTest 5: Testing class types...\n");
  test_assert([trueNum isKindOfClass:[NXNumber class]]);
  test_assert([falseNum isKindOfClass:[NXNumber class]]);
  test_assert([boolYes isKindOfClass:[NXNumber class]]);
  test_assert([boolNo isKindOfClass:[NXNumber class]]);
  printf("✓ All objects are instances of NXNumber\n");

  // Test 6: Boolean value extraction
  printf("\nTest 6: Testing boolean value extraction...\n");
  // Directly test boolValue method without respondsToSelector check
  NXNumberBool *trueBool = (NXNumberBool *)trueNum;
  NXNumberBool *falseBool = (NXNumberBool *)falseNum;
  NXNumberBool *yesBool = (NXNumberBool *)boolYes;
  NXNumberBool *noBool = (NXNumberBool *)boolNo;

  test_assert([trueBool boolValue] == YES);
  test_assert([falseBool boolValue] == NO);
  test_assert([yesBool boolValue] == YES);
  test_assert([noBool boolValue] == NO);
  printf("✓ Boolean values extracted correctly\n");

  // Test 7: String representation
  printf("\nTest 7: Testing string representation...\n");
  // Directly test description method without respondsToSelector check
  NXString *trueDesc = [trueNum description];
  NXString *falseDesc = [falseNum description];

  test_assert(trueDesc != nil);
  test_assert(falseDesc != nil);

  const char *trueStr = [trueDesc cStr];
  const char *falseStr = [falseDesc cStr];

  printf("True number description: '%s'\n", trueStr);
  printf("False number description: '%s'\n", falseStr);

  // Check that descriptions are different
  test_assert(strcmp(trueStr, falseStr) != 0);
  printf("✓ String representations are different for true/false\n");

  // Test 8: Multiple calls to factory methods
  printf("\nTest 8: Testing consistency of multiple factory method calls...\n");
  NXNumber *true1 = [NXNumber trueValue];
  NXNumber *true2 = [NXNumber trueValue];
  NXNumber *false1 = [NXNumber falseValue];
  NXNumber *false2 = [NXNumber falseValue];

  test_assert(true1 == true2);   // Same singleton instance
  test_assert(false1 == false2); // Same singleton instance
  test_assert(true1 != false1);  // Different instances for different values
  printf("✓ Factory methods consistently return same singleton instances\n");

  // Test 9: Boolean expressions with values
  printf("\nTest 9: Testing boolean expressions...\n");
  NXNumberBool *testTrue = (NXNumberBool *)[NXNumber numberWithBool:YES];
  NXNumberBool *testFalse = (NXNumberBool *)[NXNumber numberWithBool:NO];

  BOOL trueResult = [testTrue boolValue];
  BOOL falseResult = [testFalse boolValue];

  test_assert(trueResult && !falseResult);    // True is true, false is false
  test_assert(!(!trueResult || falseResult)); // Logical operations work
  printf("✓ Boolean expressions work correctly\n");

  // Test 10: Memory management verification
  printf("\nTest 10: Testing memory management...\n");
  // Verify we can create multiple references without issues
  NXNumber *ref1 = [NXNumber trueValue];
  NXNumber *ref2 = [NXNumber trueValue];
  NXNumber *ref3 = [NXNumber falseValue];

  test_assert(ref1 == ref2); // Same singleton
  test_assert(ref1 != ref3); // Different singletons

  // Try retaining and releasing (should be safe for singletons)
  [ref1 retain];
  [ref1 release];

  // Verify singleton still works after retain/release
  NXNumber *ref4 = [NXNumber trueValue];
  test_assert(ref1 == ref4);
  printf("✓ Memory management works correctly with singletons\n");

  printf("\n=== All NXNumber boolean tests passed! ===\n");

  // Clean up
  [pool release];
  [zone release];

  return 0;
}
