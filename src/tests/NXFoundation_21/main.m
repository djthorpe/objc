#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

int test_array_methods(void);

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  NXZone *zone = [NXZone zoneWithSize:2048];
  test_assert(zone != nil);
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];
  test_assert(pool != nil);

  // Run the test for array methods
  int returnValue = TestMain("NXFoundation_21", test_array_methods);

  // Clean up
  [pool release];
  [zone release];

  // Return the result of the test
  return returnValue;
}

///////////////////////////////////////////////////////////////////////////////
// TESTS

int test_array_methods(void) {
  printf("\n=== Testing NXArray Functionality ===\n");

  // Test 1: Array creation and basic properties
  printf("Test 1: Testing array creation...\n");

  // Create empty array
  NXArray *emptyArray = [NXArray new];
  test_assert(emptyArray != nil);
  test_assert([emptyArray count] == 0);
  printf("✓ Empty array creation\n");

  // Create array with capacity
  NXArray *capacityArray = [NXArray arrayWithCapacity:10];
  test_assert(capacityArray != nil);
  test_assert([capacityArray count] == 0);
  test_assert([capacityArray capacity] >= 10);
  printf("✓ Array creation with capacity\n");

  // Test 2: Object access on empty arrays
  printf("\nTest 2: Testing empty array access...\n");

  test_assert([emptyArray firstObject] == nil);
  test_assert([emptyArray lastObject] == nil);
  // Note: objectAtIndex on empty array will assert - this is expected behavior
  printf("✓ Empty array access returns nil\n");

  // Test 3: Basic array properties
  printf("\nTest 3: Testing basic array properties...\n");

  // Test that empty arrays have correct properties
  test_assert([emptyArray count] == 0);
  test_assert([capacityArray count] == 0);
  test_assert([capacityArray capacity] >= 10);
  printf("✓ Array properties validation\n");

  // Test 4: Array class and protocol conformance
  printf("\nTest 4: Testing array class properties...\n");

  // Test that arrays are properly created and released
  test_assert(emptyArray != nil);
  test_assert(capacityArray != nil);

  // Test protocol conformance
  test_assert([emptyArray conformsTo:@protocol(JSONProtocol)]);
  printf("✓ Array class and protocol conformance\n");

  // Test 5: Array creation with objects
  printf("\nTest 5: Testing arrayWithObjects and initWithObjects...\n");

  // Test arrayWithObjects with multiple objects
  NXString *obj1 = [NXString stringWithCString:"First"];
  NXString *obj2 = [NXString stringWithCString:"Second"];
  NXString *obj3 = [NXString stringWithCString:"Third"];

  NXArray *objectArray = [NXArray arrayWithObjects:obj1, obj2, obj3, nil];
  test_assert(objectArray != nil);
  test_assert([objectArray count] == 3);
  test_assert([objectArray firstObject] == obj1);
  test_assert([objectArray lastObject] == obj3);
  test_assert([objectArray objectAtIndex:0] == obj1);
  test_assert([objectArray objectAtIndex:1] == obj2);
  test_assert([objectArray objectAtIndex:2] == obj3);
  printf("✓ Array creation with objects via arrayWithObjects\n");

  // Test initWithObjects directly
  NXArray *initArray = [[NXArray alloc] initWithObjects:obj3, obj1, obj2, nil];
  test_assert(initArray != nil);
  test_assert([initArray count] == 3);
  test_assert([initArray firstObject] == obj3);
  test_assert([initArray lastObject] == obj2);
  test_assert([initArray objectAtIndex:0] == obj3);
  test_assert([initArray objectAtIndex:1] == obj1);
  test_assert([initArray objectAtIndex:2] == obj2);
  [initArray release];
  printf("✓ Array creation with objects via initWithObjects\n");

  // Test empty object creation (nil as first argument)
  NXArray *emptyObjectArray = [NXArray arrayWithObjects:nil];
  test_assert(emptyObjectArray != nil);
  test_assert([emptyObjectArray count] == 0);
  test_assert([emptyObjectArray firstObject] == nil);
  test_assert([emptyObjectArray lastObject] == nil);
  printf("✓ Array creation with nil first object\n");

  // Test single object creation
  NXArray *singleObjectArray = [NXArray arrayWithObjects:obj1, nil];
  test_assert(singleObjectArray != nil);
  test_assert([singleObjectArray count] == 1);
  test_assert([singleObjectArray firstObject] == obj1);
  test_assert([singleObjectArray lastObject] == obj1);
  test_assert([singleObjectArray objectAtIndex:0] == obj1);
  printf("✓ Array creation with single object\n");

  // Test 6: JSON functionality for arrays
  printf("\nTest 6: Testing NXArray JSON functionality...\n");

  // Test empty array JSON
  NXString *emptyArrayJson = [emptyArray JSONString];
  test_assert(emptyArrayJson != nil);
  test_cstrings_equal([emptyArrayJson cStr], "[]");
  printf("✓ Empty array JSON output\n");

  // Test single object array JSON
  NXString *singleArrayJson = [singleObjectArray JSONString];
  test_assert(singleArrayJson != nil);
  test_cstrings_equal([singleArrayJson cStr], "[\"First\"]");
  printf("✓ Single object array JSON output\n");

  // Test multiple object array JSON
  NXString *multiArrayJson = [objectArray JSONString];
  test_assert(multiArrayJson != nil);
  test_cstrings_equal([multiArrayJson cStr],
                      "[\"First\", \"Second\", \"Third\"]");
  printf("✓ Multiple object array JSON output\n");

  // Test array with different object types
  NXNumber *num1 = [NXNumber numberWithInt32:42];
  NXNumber *boolVal = [NXNumber numberWithBool:YES];
  NXNull *nullVal = [NXNull nullValue];

  NXArray *mixedArray =
      [NXArray arrayWithObjects:obj1, num1, boolVal, nullVal, nil];
  NXString *mixedArrayJson = [mixedArray JSONString];
  test_assert(mixedArrayJson != nil);
  test_cstrings_equal([mixedArrayJson cStr], "[\"First\", 42, true, null]");
  printf("✓ Mixed type array JSON output\n");

  // Test nested array (array containing another array) - if supported
  // Note: This would require implementing NXArray as a JSONProtocol conforming
  // object
  printf("✓ Basic array JSON formatting verified\n");

  // Test 7: JSON capacity estimation
  printf("\nTest 7: Testing JSONBytes capacity estimation...\n");

  // Test empty array capacity
  size_t emptyCapacity = [emptyArray JSONBytes];
  test_assert(emptyCapacity >= 2); // Should at least account for "[]"
  printf("✓ Empty array JSONBytes: %zu\n", emptyCapacity);

  // Test single object capacity
  size_t singleCapacity = [singleObjectArray JSONBytes];
  test_assert(singleCapacity >= [singleArrayJson length]);
  printf("✓ Single object array JSONBytes: %zu (actual length: %u)\n",
         singleCapacity, [singleArrayJson length]);

  // Test multiple object capacity
  size_t multiCapacity = [objectArray JSONBytes];
  test_assert(multiCapacity >= [multiArrayJson length]);
  printf("✓ Multiple object array JSONBytes: %zu (actual length: %u)\n",
         multiCapacity, [multiArrayJson length]);

  // Test mixed type capacity
  size_t mixedCapacity = [mixedArray JSONBytes];
  test_assert(mixedCapacity >= [mixedArrayJson length]);
  printf("✓ Mixed type array JSONBytes: %zu (actual length: %u)\n",
         mixedCapacity, [mixedArrayJson length]);

  // Test 8: JSON standard compliance for arrays
  printf("\nTest 8: Testing JSON standard compliance...\n");

  // Verify array JSON starts and ends with brackets
  const char *arrayJsonStr = [multiArrayJson cStr];
  test_assert(arrayJsonStr[0] == '[');
  test_assert(arrayJsonStr[strlen(arrayJsonStr) - 1] == ']');
  printf("✓ Array JSON properly bracketed\n");

  // Verify proper comma separation (no trailing comma)
  const char *commaTest = [[objectArray JSONString] cStr];
  test_assert(strstr(commaTest, ", ") !=
              NULL); // Should have comma-space separators
  test_assert(strstr(commaTest, ",]") ==
              NULL); // Should not have trailing comma
  printf("✓ Array JSON comma separation correct\n");

  // Verify that JSONBytes estimation is reasonable (not too small)
  test_assert([objectArray JSONBytes] >=
              [objectArray count] * 2); // Minimum reasonable estimate
  printf("✓ JSONBytes estimation is reasonable\n");

  // Test 9: Array JSON with special characters
  printf("\nTest 9: Testing array JSON with special characters...\n");

  NXString *specialStr1 = [NXString stringWithCString:"String with \"quotes\""];
  NXString *specialStr2 = [NXString stringWithCString:"String\nwith\nlines"];
  NXArray *specialArray =
      [NXArray arrayWithObjects:specialStr1, specialStr2, nil];

  NXString *specialArrayJson = [specialArray JSONString];
  test_assert(specialArrayJson != nil);

  // Verify the strings are properly escaped in the array context
  const char *specialJsonStr = [specialArrayJson cStr];
  test_assert(strstr(specialJsonStr, "\\\"") !=
              NULL); // Should have escaped quotes
  test_assert(strstr(specialJsonStr, "\\n") !=
              NULL); // Should have escaped newlines
  printf("✓ Array JSON with special characters properly escaped\n");

  // Test 10: Array JSON with non-JSON-compliant objects
  printf("\nTest 10: Testing array JSON with non-JSON-compliant objects...\n");

  // Create a basic NXObject that doesn't conform to JSONProtocol
  NXObject *basicObject = [[NXObject alloc] init];
  test_assert(basicObject != nil);

  // Verify the object doesn't conform to JSONProtocol
  test_assert(![basicObject conformsTo:@protocol(JSONProtocol)]);
  printf("✓ Basic NXObject doesn't conform to JSONProtocol\n");

  // Test array with non-JSON-compliant object mixed with compliant ones
  NXString *normalStr = [NXString stringWithCString:"normal"];
  NXNumber *normalNum = [NXNumber numberWithInt32:123];
  NXArray *mixedComplianceArray =
      [NXArray arrayWithObjects:normalStr, basicObject, normalNum, nil];

  NXString *mixedComplianceJson = [mixedComplianceArray JSONString];
  test_assert(mixedComplianceJson != nil);

  // Verify the JSON is valid and contains the object's description
  const char *mixedJsonStr = [mixedComplianceJson cStr];
  test_assert(mixedJsonStr[0] == '[');
  test_assert(mixedJsonStr[strlen(mixedJsonStr) - 1] == ']');

  // Should contain the normal string and number
  test_assert(strstr(mixedJsonStr, "\"normal\"") != NULL);
  test_assert(strstr(mixedJsonStr, "123") != NULL);

  // Should contain the object's description (which will be JSON-escaped)
  NXString *objectDesc = [basicObject description];
  printf("  → Basic object description: %s\n", [objectDesc cStr]);

  // The description should be properly JSON-escaped in the output
  test_assert(strstr(mixedJsonStr, [objectDesc cStr]) != NULL ||
              strstr(mixedJsonStr, "NXObject") !=
                  NULL); // Should contain object info
  printf("✓ Non-JSON-compliant object handled via description\n");

  // Test array with only non-JSON-compliant objects
  NXObject *object1 = [[NXObject alloc] init];
  NXObject *object2 = [[NXObject alloc] init];
  NXArray *nonCompliantArray = [NXArray arrayWithObjects:object1, object2, nil];

  NXString *nonCompliantJson = [nonCompliantArray JSONString];
  test_assert(nonCompliantJson != nil);

  // Verify it's still valid JSON structure
  const char *nonCompliantJsonStr = [nonCompliantJson cStr];
  test_assert(nonCompliantJsonStr[0] == '[');
  test_assert(nonCompliantJsonStr[strlen(nonCompliantJsonStr) - 1] == ']');
  test_assert(strstr(nonCompliantJsonStr, ", ") !=
              NULL); // Should have comma separation
  printf(
      "✓ Array of non-JSON-compliant objects produces valid JSON structure\n");

  // Test JSONBytes calculation with non-compliant objects
  size_t nonCompliantCapacity = [nonCompliantArray JSONBytes];
  test_assert(nonCompliantCapacity >= [nonCompliantJson length]);
  test_assert(nonCompliantCapacity >=
              4); // At minimum should account for "[]" + content
  printf("✓ JSONBytes estimation works with non-JSON-compliant objects\n");

  // Test 11: Testing containsObject method
  printf("\nTest 11: Testing containsObject method...\n");

  // Create test objects (these are autoreleased)
  NXString *testStr1 = [NXString stringWithCString:"hello"];
  NXString *testStr2 = [NXString stringWithCString:"world"];
  NXString *testStr3 = [NXString stringWithCString:"missing"];

  // Create array with some objects (this is autoreleased)
  NXArray *testArray = [NXArray arrayWithObjects:testStr1, testStr2, nil];

  // Test: Object that exists
  test_assert([testArray containsObject:testStr1]);
  test_assert([testArray containsObject:testStr2]);
  printf("✓ Contains existing objects\n");

  // Test: Object that doesn't exist
  test_assert(![testArray containsObject:testStr3]);
  printf("✓ Correctly returns NO for missing objects\n");

  // Test: Nil object (should return NO, not crash)
  test_assert(![testArray containsObject:nil]);
  printf("✓ Handles nil objects gracefully\n");

  // Test: Empty array
  NXArray *emptyTestArray =
      [NXArray new]; // This is autoreleased, don't manually release
  test_assert(![emptyTestArray containsObject:testStr1]);
  printf("✓ Empty array correctly returns NO\n");

  // Test: Nested array (recursive search)
  NXArray *nestedArray = [NXArray arrayWithObjects:testStr3, nil];
  NXArray *outerArray = [NXArray arrayWithObjects:testStr1, nestedArray, nil];
  test_assert(
      [outerArray containsObject:testStr3]); // Should find it recursively
  test_assert([outerArray
      containsObject:nestedArray]); // Should find the nested array itself
  printf("✓ Recursive search in nested collections works\n");

  // Test 12: Testing append method
  printf("\nTest 12: Testing append method...\n");

  // Create a new empty array
  NXArray *appendArray = [NXArray new];
  test_assert([appendArray count] == 0);
  printf("✓ Created empty array\n");

  // Test appending first object (should grow from capacity 0 to 1)
  BOOL result1 = [appendArray append:testStr1];
  test_assert(result1 == YES);
  test_assert([appendArray count] == 1);
  test_assert([appendArray capacity] >= 1);
  test_assert([appendArray firstObject] == testStr1);
  printf("✓ First append successful, capacity: %zu\n", [appendArray capacity]);

  // Test appending second object (should grow capacity from 1 to 2)
  BOOL result2 = [appendArray append:testStr2];
  test_assert(result2 == YES);
  test_assert([appendArray count] == 2);
  test_assert([appendArray capacity] >= 2);
  test_assert([appendArray objectAtIndex:1] == testStr2);
  printf("✓ Second append successful, capacity: %zu\n", [appendArray capacity]);

  // Test appending third object (should grow capacity from 2 to 3 with 1.5x
  // growth)
  BOOL result3 = [appendArray append:testStr3];
  test_assert(result3 == YES);
  test_assert([appendArray count] == 3);
  test_assert([appendArray capacity] >= 3);
  test_assert([appendArray lastObject] == testStr3);
  printf("✓ Third append successful, capacity: %zu\n", [appendArray capacity]);

  // Test circular reference prevention
  BOOL circularResult = [appendArray append:appendArray];
  test_assert(circularResult == NO);
  test_assert([appendArray count] == 3); // Should not have increased
  printf("✓ Circular reference prevention works\n");

  // Test JSON output of appended array
  NXString *appendedJson = [appendArray JSONString];
  test_assert(appendedJson != nil);
  test_cstrings_equal([appendedJson cStr],
                      "[\"hello\", \"world\", \"missing\"]");
  printf("✓ Appended array JSON output correct\n");
  printf("✓ All append operations successful\n");

  // Test 13: Testing insert:atIndex: method
  printf("\nTest 13: Testing insert:atIndex: method...\n");

  // Create array for insertion testing
  NXArray *insertArray = [NXArray new];
  NXString *first = [NXString stringWithCString:"first"];
  NXString *last = [NXString stringWithCString:"last"];
  NXString *middle = [NXString stringWithCString:"middle"];
  NXString *beginning = [NXString stringWithCString:"beginning"];

  // Test inserting into empty array at index 0
  BOOL insertResult1 = [insertArray insert:first atIndex:0];
  test_assert(insertResult1 == YES);
  test_assert([insertArray count] == 1);
  printf("✓ Insert into empty array at index 0\n");

  // Test inserting at the end (should delegate to append)
  BOOL insertResult2 = [insertArray insert:last atIndex:1];
  test_assert(insertResult2 == YES);
  test_assert([insertArray count] == 2);
  printf("✓ Insert at end (delegates to append)\n");

  // Test inserting in the middle
  BOOL insertResult3 = [insertArray insert:middle atIndex:1];
  test_assert(insertResult3 == YES);
  test_assert([insertArray count] == 3);
  printf("✓ Insert in middle\n");

  // Test inserting at the beginning
  BOOL insertResult4 = [insertArray insert:beginning atIndex:0];
  test_assert(insertResult4 == YES);
  test_assert([insertArray count] == 4);
  printf("✓ Insert at beginning\n");

  // Verify final array order: ["beginning", "first", "middle", "last"]
  test_assert([insertArray count] == 4);

  // Check order
  NXString *elem0 = [insertArray objectAtIndex:0];
  NXString *elem1 = [insertArray objectAtIndex:1];
  NXString *elem2 = [insertArray objectAtIndex:2];
  NXString *elem3 = [insertArray objectAtIndex:3];

  test_cstrings_equal([elem0 cStr], "beginning");
  test_cstrings_equal([elem1 cStr], "first");
  test_cstrings_equal([elem2 cStr], "middle");
  test_cstrings_equal([elem3 cStr], "last");
  printf("✓ Array elements in correct order\n");

  // Test capacity growth during insertion
  size_t capacityBefore = [insertArray capacity];
  NXString *extra1 = [NXString stringWithCString:"extra1"];
  NXString *extra2 = [NXString stringWithCString:"extra2"];

  // Insert multiple elements to trigger capacity growth
  [insertArray insert:extra1 atIndex:2];
  [insertArray insert:extra2 atIndex:2];

  size_t capacityAfter = [insertArray capacity];
  test_assert(capacityAfter > capacityBefore);
  printf("✓ Capacity growth during insertion: %zu → %zu\n", capacityBefore,
         capacityAfter);

  // Test circular reference prevention
  BOOL circularInsert = [insertArray insert:insertArray atIndex:0];
  test_assert(circularInsert == NO);
  printf("✓ Circular reference prevention works\n");

  // Test JSON output with inserted elements
  NXString *insertedJSON = [insertArray JSONString];
  test_assert(insertedJSON != nil);
  printf("JSON after insertions: %s\n", [insertedJSON cStr]);

  // Verify JSON contains all elements
  test_assert(
      [insertedJSON containsString:[NXString stringWithCString:"beginning"]]);
  test_assert(
      [insertedJSON containsString:[NXString stringWithCString:"first"]]);
  test_assert(
      [insertedJSON containsString:[NXString stringWithCString:"middle"]]);
  test_assert(
      [insertedJSON containsString:[NXString stringWithCString:"last"]]);
  test_assert(
      [insertedJSON containsString:[NXString stringWithCString:"extra1"]]);
  test_assert(
      [insertedJSON containsString:[NXString stringWithCString:"extra2"]]);
  printf("✓ JSON output contains all inserted elements\n");
  printf("✓ All insert operations successful\n");

  // Test 14: Testing indexForObject: method
  printf("\nTest 14: Testing indexForObject: method...\n");

  // Create test strings
  NXString *findStr1 = [NXString stringWithCString:"find1"];
  NXString *findStr2 = [NXString stringWithCString:"find2"];
  NXString *findStr3 = [NXString stringWithCString:"find3"];
  NXString *notFoundStr = [NXString stringWithCString:"notfound"];

  // Create array with test objects
  NXArray *findArray =
      [NXArray arrayWithObjects:findStr1, findStr2, findStr3, nil];
  test_assert([findArray count] == 3);

  // Test finding objects at various positions
  unsigned int index1 = [findArray indexForObject:findStr1];
  unsigned int index2 = [findArray indexForObject:findStr2];
  unsigned int index3 = [findArray indexForObject:findStr3];
  unsigned int indexNotFound = [findArray indexForObject:notFoundStr];

  test_assert(index1 == 0);
  test_assert(index2 == 1);
  test_assert(index3 == 2);
  test_assert(indexNotFound == NXNotFound);
  printf("✓ Found objects at correct indices: 0, 1, 2\n");
  printf("✓ Missing object returns NXNotFound (%u)\n", NXNotFound);

  // Test with empty array
  unsigned int emptyIndex = [emptyArray indexForObject:findStr1];
  test_assert(emptyIndex == NXNotFound);
  printf("✓ Empty array returns NXNotFound for any object\n");

  // Test with object that exists multiple times (should return first
  // occurrence)
  NXArray *duplicateArray =
      [NXArray arrayWithObjects:findStr1, findStr2, findStr1, nil];
  unsigned int firstOccurrence = [duplicateArray indexForObject:findStr1];
  test_assert(firstOccurrence == 0);
  printf("✓ Returns index of first occurrence when object appears multiple "
         "times\n");

  printf("✓ All indexForObject: tests successful\n");

  // Note: testStr1, testStr2, testStr3, testArray, nestedArray, outerArray,
  // emptyTestArray, appendArray are autoreleased No manual releases needed for
  // these objects

  // Clean up allocated objects
  [basicObject release];
  [object1 release];
  [object2 release];

  printf("\n⚠️  Note: NXArray implementation now supports all major array "
         "operations\n");
  printf("    ✅ Implemented: initWithObjects:, arrayWithObjects:, JSONString, "
         "JSONBytes, containsObject:, append:, insert:atIndex:, "
         "indexForObject:\n");
  printf("    ✅ Handles non-JSON-compliant objects via description method\n");
  printf("    ✅ Supports recursive collection search in containsObject:\n");
  printf(
      "    ✅ Dynamic array growth with append: and insert:atIndex: methods\n");
  printf("    ✅ Circular reference prevention for mutation operations\n");
  printf("    ⚠️  objectAtIndex: uses assertions for bounds checking\n");

  printf("\n✅ All implemented NXArray methods tested successfully!\n");
  return 0;
}
