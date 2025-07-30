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

  // Test 15: Testing removeObjectAtIndex: method
  printf("\nTest 15: Testing removeObjectAtIndex: method...\n");

  // Create array for removal testing
  NXString *removeStr1 = [NXString stringWithCString:"remove1"];
  NXString *removeStr2 = [NXString stringWithCString:"remove2"];
  NXString *removeStr3 = [NXString stringWithCString:"remove3"];
  NXString *removeStr4 = [NXString stringWithCString:"remove4"];

  NXArray *removeArray = [NXArray
      arrayWithObjects:removeStr1, removeStr2, removeStr3, removeStr4, nil];
  test_assert([removeArray count] == 4);
  printf("✓ Created array with 4 elements\n");

  // Test removing from middle (index 1)
  BOOL removeResult1 = [removeArray removeObjectAtIndex:1];
  test_assert(removeResult1 == YES);
  test_assert([removeArray count] == 3);
  test_assert([removeArray objectAtIndex:0] == removeStr1);
  test_assert([removeArray objectAtIndex:1] ==
              removeStr3); // removeStr2 was removed
  test_assert([removeArray objectAtIndex:2] == removeStr4);
  printf("✓ Removed middle element (index 1), elements shifted correctly\n");

  // Test removing from beginning (index 0)
  BOOL removeResult2 = [removeArray removeObjectAtIndex:0];
  test_assert(removeResult2 == YES);
  test_assert([removeArray count] == 2);
  test_assert([removeArray objectAtIndex:0] ==
              removeStr3); // removeStr1 was removed
  test_assert([removeArray objectAtIndex:1] == removeStr4);
  printf("✓ Removed first element (index 0), elements shifted correctly\n");

  // Test removing from end (last index)
  BOOL removeResult3 = [removeArray removeObjectAtIndex:1];
  test_assert(removeResult3 == YES);
  test_assert([removeArray count] == 1);
  test_assert([removeArray objectAtIndex:0] == removeStr3);
  printf("✓ Removed last element, array reduced to 1 element\n");

  // Test removing the final element
  BOOL removeResult4 = [removeArray removeObjectAtIndex:0];
  test_assert(removeResult4 == YES);
  test_assert([removeArray count] == 0);
  test_assert([removeArray firstObject] == nil);
  test_assert([removeArray lastObject] == nil);
  printf("✓ Removed final element, array is now empty\n");

  printf("✓ All removeObjectAtIndex: tests successful\n");

  // Test 16: Testing remove: method
  printf("\nTest 16: Testing remove: method...\n");

  // Create array for object removal testing
  NXString *objRemove1 = [NXString stringWithCString:"obj1"];
  NXString *objRemove2 = [NXString stringWithCString:"obj2"];
  NXString *objRemove3 = [NXString stringWithCString:"obj3"];
  NXString *objNotInArray = [NXString stringWithCString:"notfound"];

  NXArray *objRemoveArray = [NXArray
      arrayWithObjects:objRemove1, objRemove2, objRemove3, objRemove1, nil];
  test_assert([objRemoveArray count] == 4);
  printf("✓ Created array with 4 elements (including duplicate)\n");

  // Test removing existing object (should remove first occurrence)
  BOOL objRemoveResult1 = [objRemoveArray remove:objRemove1];
  test_assert(objRemoveResult1 == YES);
  test_assert([objRemoveArray count] == 3);
  test_assert([objRemoveArray objectAtIndex:0] == objRemove2);
  test_assert([objRemoveArray objectAtIndex:1] == objRemove3);
  test_assert([objRemoveArray objectAtIndex:2] ==
              objRemove1); // Second occurrence remains
  printf("✓ Removed first occurrence of duplicate object\n");

  // Test removing middle object
  BOOL objRemoveResult2 = [objRemoveArray remove:objRemove3];
  test_assert(objRemoveResult2 == YES);
  test_assert([objRemoveArray count] == 2);
  test_assert([objRemoveArray objectAtIndex:0] == objRemove2);
  test_assert([objRemoveArray objectAtIndex:1] == objRemove1);
  printf("✓ Removed middle object successfully\n");

  // Test removing object that doesn't exist
  BOOL objRemoveResult3 = [objRemoveArray remove:objNotInArray];
  test_assert(objRemoveResult3 == NO);
  test_assert([objRemoveArray count] == 2); // Array unchanged
  printf("✓ Correctly returned NO for non-existent object\n");

  // Test removing remaining objects
  BOOL objRemoveResult4 = [objRemoveArray remove:objRemove2];
  test_assert(objRemoveResult4 == YES);
  test_assert([objRemoveArray count] == 1);

  BOOL objRemoveResult5 = [objRemoveArray remove:objRemove1];
  test_assert(objRemoveResult5 == YES);
  test_assert([objRemoveArray count] == 0);
  printf("✓ Removed all remaining objects, array is empty\n");

  // Test removing from empty array
  BOOL objRemoveResult6 = [objRemoveArray remove:objRemove1];
  test_assert(objRemoveResult6 == NO);
  test_assert([objRemoveArray count] == 0);
  printf("✓ Correctly returned NO when removing from empty array\n");

  printf("✓ All remove: tests successful\n");

  // Test 17: Comprehensive capacity and memory management edge cases
  printf("\nTest 17: Testing capacity management and memory edge cases...\n");

  // Test capacity growth progression
  NXArray *growthArray = [NXArray new];
  test_assert([growthArray count] == 0);
  test_assert([growthArray capacity] == 0);
  printf("✓ Initial empty array has zero capacity\n");

  // Test growth from 0 to 1
  NXString *growthStr1 = [NXString stringWithCString:"item1"];
  [growthArray append:growthStr1];
  test_assert([growthArray count] == 1);
  test_assert([growthArray capacity] >= 1);
  size_t cap1 = [growthArray capacity];
  printf("✓ Capacity after first append: %zu\n", cap1);

  // Test growth from 1 to 2 (should become 2)
  NXString *growthStr2 = [NXString stringWithCString:"item2"];
  [growthArray append:growthStr2];
  test_assert([growthArray count] == 2);
  test_assert([growthArray capacity] >= 2);
  size_t cap2 = [growthArray capacity];
  printf("✓ Capacity after second append: %zu\n", cap2);

  // Test growth progression (1.5x growth)
  for (int i = 3; i <= 10; i++) {
    NXString *item = [NXString stringWithCString:"itemN"];
    size_t prevCap = [growthArray capacity];
    [growthArray append:item];
    size_t newCap = [growthArray capacity];

    if (newCap > prevCap) {
      // Verify 1.5x growth strategy
      test_assert(newCap >= prevCap * 3 / 2 || newCap == prevCap + 1);
      printf("✓ Capacity growth %zu → %zu (growth triggered at count %d)\n",
             prevCap, newCap, i);
    }
  }

  // Test large capacity allocation
  NXArray *largeCapacityArray = [NXArray arrayWithCapacity:1000];
  test_assert([largeCapacityArray capacity] >= 1000);
  test_assert([largeCapacityArray count] == 0);
  printf("✓ Large capacity allocation successful\n");

  // Test zero capacity edge case
  NXArray *zeroCapArray = [NXArray arrayWithCapacity:0];
  test_assert([zeroCapArray capacity] == 0);
  test_assert([zeroCapArray count] == 0);
  printf("✓ Zero capacity array creation\n");

  // Test 18: Boundary condition testing
  printf("\nTest 18: Testing boundary conditions and limits...\n");

  // Test inserting at exact length boundary
  NXArray *boundaryArray =
      [NXArray arrayWithObjects:growthStr1, growthStr2, nil];
  test_assert([boundaryArray count] == 2);

  // Insert at exact end (should work)
  BOOL boundaryInsert1 = [boundaryArray insert:growthStr1 atIndex:2];
  test_assert(boundaryInsert1 == YES);
  test_assert([boundaryArray count] == 3);
  printf("✓ Insert at exact array length succeeds\n");

  // Test edge case: array with maximum reasonable size
  NXArray *maxArray = [NXArray new];
  for (int i = 0; i < 100; i++) {
    NXString *maxItem = [NXString stringWithCString:"max"];
    BOOL appendResult = [maxArray append:maxItem];
    test_assert(appendResult == YES);
  }
  test_assert([maxArray count] == 100);
  printf("✓ Array handles 100 elements successfully\n");

  // Test removing all elements one by one
  while ([maxArray count] > 0) {
    [maxArray removeObjectAtIndex:0];
  }
  test_assert([maxArray count] == 0);
  test_assert([maxArray firstObject] == nil);
  test_assert([maxArray lastObject] == nil);
  printf("✓ Array emptied by successive removals\n");

  // Test 19: Complex object interaction edge cases
  printf("\nTest 19: Testing complex object interactions...\n");

  // Test arrays containing other arrays (nested structures)
  NXArray *innerArray1 = [NXArray arrayWithObjects:growthStr1, nil];
  NXArray *innerArray2 = [NXArray arrayWithObjects:growthStr2, nil];
  NXArray *outerComplexArray =
      [NXArray arrayWithObjects:innerArray1, innerArray2, nil];

  test_assert([outerComplexArray count] == 2);
  test_assert([outerComplexArray objectAtIndex:0] == innerArray1);
  test_assert([outerComplexArray objectAtIndex:1] == innerArray2);
  printf("✓ Array can contain other arrays\n");

  // Test containsObject with nested arrays
  test_assert([outerComplexArray containsObject:innerArray1]);
  test_assert(
      [outerComplexArray containsObject:growthStr1]); // Should find recursively
  test_assert(![outerComplexArray
      containsObject:[NXString stringWithCString:"notfound"]]);
  printf("✓ Recursive containsObject works with nested arrays\n");

  // Test JSON output with nested arrays
  NXString *nestedJson = [outerComplexArray JSONString];
  test_assert(nestedJson != nil);
  test_assert([nestedJson containsString:[NXString stringWithCString:"["]]);
  test_assert([nestedJson containsString:[NXString stringWithCString:"]"]]);
  printf("✓ JSON serialization works with nested arrays\n");

  // Test mixed object types in array
  NXNumber *mixedNum = [NXNumber numberWithInt32:42];
  NXNull *mixedNull = [NXNull nullValue];
  NXArray *complexMixedArray = [NXArray
      arrayWithObjects:growthStr1, mixedNum, mixedNull, innerArray1, nil];

  test_assert([complexMixedArray count] == 4);
  test_assert([complexMixedArray indexForObject:growthStr1] == 0);
  test_assert([complexMixedArray indexForObject:mixedNum] == 1);
  test_assert([complexMixedArray indexForObject:mixedNull] == 2);
  test_assert([complexMixedArray indexForObject:innerArray1] == 3);
  printf("✓ Array handles mixed object types correctly\n");

  // Test 20: Edge cases in search and comparison operations
  printf("\nTest 20: Testing search and comparison edge cases...\n");

  // Test indexForObject with objects that have same string content but
  // different instances
  NXString *searchStr1 = [NXString stringWithCString:"duplicate"];
  NXString *searchStr2 = [NXString stringWithCString:"duplicate"];
  NXArray *searchArray = [NXArray arrayWithObjects:searchStr1, searchStr2, nil];

  // Should find first occurrence based on isEqual:, not pointer equality
  unsigned int foundIndex1 = [searchArray indexForObject:searchStr1];
  unsigned int foundIndex2 = [searchArray indexForObject:searchStr2];
  test_assert(foundIndex1 == 0); // First instance
  test_assert(foundIndex2 ==
              0); // Same because isEqual: returns YES for equal strings
  printf("✓ indexForObject uses isEqual: correctly\n");

  // Test containsObject with equivalent but different instances
  test_assert([searchArray containsObject:searchStr1]);
  test_assert([searchArray containsObject:searchStr2]);
  printf("✓ containsObject handles equivalent instances\n");

  // Test removing equivalent but different instances
  BOOL removeEquivalent = [searchArray remove:searchStr2];
  test_assert(removeEquivalent == YES);
  test_assert([searchArray count] == 1); // Should have removed first occurrence
  printf("✓ remove: removes first equivalent object\n");

  // Test 21: JSON edge cases and special formatting
  printf("\nTest 21: Testing JSON edge cases and special formatting...\n");

  // Test JSONBytes accuracy with various content
  NXArray *jsonTestArray = [NXArray new];
  size_t emptyJsonBytes = [jsonTestArray JSONBytes];
  test_assert(emptyJsonBytes >= 2); // Should be at least "[]"

  NXString *jsonActual = [jsonTestArray JSONString];
  test_assert([jsonActual length] <= emptyJsonBytes);
  printf("✓ Empty array JSONBytes estimation accurate\n");

  // Test JSONBytes with complex content
  [jsonTestArray append:[NXString stringWithCString:"test\"with\\quotes"]];
  [jsonTestArray append:[NXNumber numberWithInt32:12345]];
  [jsonTestArray append:[NXNull nullValue]];

  size_t complexJsonBytes = [jsonTestArray JSONBytes];
  NXString *complexJsonActual = [jsonTestArray JSONString];
  test_assert([complexJsonActual length] <= complexJsonBytes);
  printf("✓ Complex array JSONBytes estimation accurate\n");

  // Verify JSON contains expected elements
  test_assert([complexJsonActual
      containsString:[NXString stringWithCString:"test\\\"with\\\\quotes"]]);
  test_assert(
      [complexJsonActual containsString:[NXString stringWithCString:"12345"]]);
  test_assert(
      [complexJsonActual containsString:[NXString stringWithCString:"null"]]);
  printf("✓ JSON contains all expected escaped content\n");

  // Test 22: Error recovery and robustness
  printf("\nTest 22: Testing error recovery and robustness...\n");

  // Test operations on arrays after heavy modification
  NXArray *robustArray = [NXArray new];

  // Fill, empty, refill cycle
  for (int cycle = 0; cycle < 3; cycle++) {
    // Fill
    for (int i = 0; i < 10; i++) {
      [robustArray append:[NXString stringWithCString:"cycle"]];
    }
    test_assert([robustArray count] == 10);

    // Empty
    while ([robustArray count] > 0) {
      [robustArray removeObjectAtIndex:0];
    }
    test_assert([robustArray count] == 0);
    printf("✓ Fill/empty cycle %d completed\n", cycle + 1);
  }

  // Test mixed operations sequence
  [robustArray append:[NXString stringWithCString:"base"]];
  [robustArray insert:[NXString stringWithCString:"insert1"] atIndex:0];
  [robustArray insert:[NXString stringWithCString:"insert2"] atIndex:1];
  [robustArray append:[NXString stringWithCString:"append1"]];
  [robustArray removeObjectAtIndex:1];
  [robustArray remove:[NXString stringWithCString:"base"]];

  test_assert([robustArray count] == 2);
  printf("✓ Complex operation sequence completed successfully\n");

  // Test that array maintains integrity after operations
  NXString *finalJson = [robustArray JSONString];
  test_assert(finalJson != nil);
  test_assert([finalJson length] > 2); // More than just "[]"
  printf("✓ Array maintains integrity after complex operations\n");

  // Test 23: Protocol conformance and polymorphic behavior
  printf(
      "\nTest 23: Testing protocol conformance and polymorphic behavior...\n");

  // Test that NXArray properly conforms to all expected protocols
  test_assert([NXArray conformsTo:@protocol(JSONProtocol)]);
  test_assert([NXArray conformsTo:@protocol(CollectionProtocol)]);
  printf("✓ NXArray class conforms to expected protocols\n");

  // Test instance protocol conformance
  NXArray *protocolTestArray = [NXArray new];
  test_assert([protocolTestArray conformsTo:@protocol(JSONProtocol)]);
  test_assert([protocolTestArray conformsTo:@protocol(CollectionProtocol)]);
  printf("✓ NXArray instances conform to expected protocols\n");

  // Test polymorphic behavior - using array as id<JSONProtocol>
  id<JSONProtocol> jsonObject = protocolTestArray;
  NXString *polymorphicJson = [jsonObject JSONString];
  test_assert(polymorphicJson != nil);
  test_cstrings_equal([polymorphicJson cStr], "[]");
  printf("✓ Polymorphic JSON behavior works correctly\n");

  // Test polymorphic behavior - using array as id<CollectionProtocol>
  id<CollectionProtocol> collectionObject = protocolTestArray;
  test_assert([collectionObject
                  containsObject:[NXString stringWithCString:"notfound"]] ==
              NO);
  printf("✓ Polymorphic collection behavior works correctly\n");

  // Test 24: Performance and stress testing
  printf("\nTest 24: Testing performance characteristics...\n");

  // Test insertion performance doesn't degrade severely with size
  NXArray *performanceArray = [NXArray new];

  // Measure append performance
  for (int i = 0; i < 50; i++) {
    BOOL appendResult =
        [performanceArray append:[NXString stringWithCString:"perf"]];
    test_assert(appendResult == YES);
  }
  test_assert([performanceArray count] == 50);
  printf("✓ Append performance: 50 elements added successfully\n");

  // Test that capacity grows reasonably (not excessive reallocation)
  size_t finalCapacity = [performanceArray capacity];
  test_assert(finalCapacity >= 50);
  test_assert(finalCapacity < 200); // Shouldn't be excessively large
  printf(
      "✓ Capacity management reasonable: final capacity %zu for 50 elements\n",
      finalCapacity);

  // Test insertion in middle doesn't break with larger arrays
  BOOL midInsert =
      [performanceArray insert:[NXString stringWithCString:"middle"]
                       atIndex:25];
  test_assert(midInsert == YES);
  test_assert([performanceArray count] == 51);
  test_assert([[performanceArray objectAtIndex:25]
      isEqual:[NXString stringWithCString:"middle"]]);
  printf("✓ Mid-array insertion works with larger arrays\n");

  // Test removal performance
  for (int i = 0; i < 25; i++) {
    [performanceArray removeObjectAtIndex:0]; // Always remove first element
  }
  test_assert([performanceArray count] == 26);
  printf("✓ Removal performance: 25 elements removed successfully\n");

  // Test removeAllObjects method
  [performanceArray removeAllObjects];
  test_assert([performanceArray count] == 0);
  test_assert([performanceArray firstObject] == nil);
  test_assert([performanceArray lastObject] == nil);
  test_assert([performanceArray capacity] >=
              26); // Capacity should be preserved
  printf("✓ removeAllObjects: cleared array while preserving capacity\n");

  // Test 25: Comprehensive circular reference prevention
  printf("\nTest 25: Testing comprehensive circular reference prevention...\n");

  // Test 1: Direct self-reference prevention
  NXArray *selfRefArray = [NXArray new];
  BOOL selfAppendResult = [selfRefArray append:selfRefArray];
  test_assert(selfAppendResult == NO);
  test_assert([selfRefArray count] == 0);
  printf("✓ Direct self-reference prevention works (append)\n");

  BOOL selfInsertResult = [selfRefArray insert:selfRefArray atIndex:0];
  test_assert(selfInsertResult == NO);
  test_assert([selfRefArray count] == 0);
  printf("✓ Direct self-reference prevention works (insert)\n");

  // Test 2: Indirect circular reference prevention
  NXArray *arrayA = [NXArray new];
  NXArray *arrayB = [NXArray new];

  // Add arrayB to arrayA first
  BOOL addB_to_A = [arrayA append:arrayB];
  test_assert(addB_to_A == YES);
  test_assert([arrayA count] == 1);

  // Now try to add arrayA to arrayB (should be prevented due to circular
  // reference)
  BOOL addA_to_B = [arrayB append:arrayA];
  test_assert(addA_to_B == NO);
  test_assert([arrayB count] == 0);
  printf("✓ Indirect circular reference prevention works\n");

  // Test 3: Deep circular reference prevention (A -> B -> C, then try C -> A)
  NXArray *arrayC = [NXArray new];

  // Build chain: A contains B, B contains C
  [arrayB append:arrayC]; // B -> C
  test_assert([arrayB count] == 1);

  // Now arrayA contains arrayB which contains arrayC
  // Try to add arrayA to arrayC (should be prevented)
  BOOL addA_to_C = [arrayC append:arrayA];
  test_assert(addA_to_C == NO);
  test_assert([arrayC count] == 0);
  printf("✓ Deep circular reference prevention works\n");

  // Test 4: Complex nested structure with mixed objects
  NXString *safeStr = [NXString stringWithCString:"safe"];
  NXArray *nestedSafeArray = [NXArray arrayWithObjects:safeStr, nil];

  // Add safe nested array to main array
  BOOL addNestedSafe = [arrayA append:nestedSafeArray];
  test_assert(addNestedSafe == YES);
  test_assert([arrayA count] == 2); // arrayB + nestedSafeArray

  // Try to add arrayA to the nested array (should be prevented)
  BOOL addA_to_Nested = [nestedSafeArray append:arrayA];
  test_assert(addA_to_Nested == NO);
  test_assert([nestedSafeArray count] == 1); // Only contains safeStr
  printf("✓ Complex nested circular reference prevention works\n");

  // Test 5: Verify that legitimate nested structures still work
  NXArray *legitInner1 = [NXArray arrayWithObjects:safeStr, nil];
  NXArray *legitInner2 = [NXArray arrayWithObjects:safeStr, nil];
  NXArray *legitOuter =
      [NXArray arrayWithObjects:legitInner1, legitInner2, nil];

  test_assert([legitOuter count] == 2);
  test_assert([legitInner1 count] == 1);
  test_assert([legitInner2 count] == 1);

  // These should all work since no circular references
  BOOL legitAppend1 = [legitInner1 append:[NXString stringWithCString:"more"]];
  BOOL legitAppend2 = [legitInner2 append:[NXString stringWithCString:"data"]];
  test_assert(legitAppend1 == YES);
  test_assert(legitAppend2 == YES);
  printf("✓ Legitimate nested structures work correctly\n");

  // Test 6: Edge case - try to create circular reference via insert at
  // different positions
  NXArray *circularInsertArray = [NXArray new];
  [circularInsertArray append:[NXString stringWithCString:"item1"]];
  [circularInsertArray append:[NXString stringWithCString:"item2"]];

  // Try to insert self at beginning
  BOOL insertSelfBegin = [circularInsertArray insert:circularInsertArray
                                             atIndex:0];
  test_assert(insertSelfBegin == NO);
  test_assert([circularInsertArray count] == 2);

  // Try to insert self in middle
  BOOL insertSelfMiddle = [circularInsertArray insert:circularInsertArray
                                              atIndex:1];
  test_assert(insertSelfMiddle == NO);
  test_assert([circularInsertArray count] == 2);

  // Try to insert self at end
  BOOL insertSelfEnd = [circularInsertArray insert:circularInsertArray
                                           atIndex:2];
  test_assert(insertSelfEnd == NO);
  test_assert([circularInsertArray count] == 2);
  printf("✓ Circular reference prevention works for insert at all positions\n");

  // Test 26: Comprehensive isEqual: method testing
  printf("\nTest 26: Testing comprehensive isEqual: functionality...\n");

  // Test 1: Identity comparison (same instance)
  NXArray *identityArray = [NXArray arrayWithObjects:safeStr, nil];
  BOOL identityEqual = [identityArray isEqual:identityArray];
  test_assert(identityEqual == YES);
  printf("✓ Identity comparison: same instance returns YES\n");

  // Test 2: Nil comparison
  BOOL nilEqual = [identityArray isEqual:nil];
  test_assert(nilEqual == NO);
  printf("✓ Nil comparison: returns NO\n");

  // Test 3: Wrong type comparison
  NXString *notAnArray = [NXString stringWithCString:"not an array"];
  BOOL wrongTypeEqual = [identityArray isEqual:notAnArray];
  test_assert(wrongTypeEqual == NO);
  printf("✓ Wrong type comparison: returns NO\n");

  // Test 4: Empty arrays comparison
  NXArray *emptyArray1 = [NXArray new];
  NXArray *emptyArray2 = [NXArray new];
  BOOL emptyEqual = [emptyArray1 isEqual:emptyArray2];
  test_assert(emptyEqual == YES);
  test_assert([emptyArray1 count] == 0);
  test_assert([emptyArray2 count] == 0);
  printf("✓ Empty arrays comparison: returns YES\n");

  // Test 5: Arrays with different lengths
  NXArray *shortArray = [NXArray arrayWithObjects:safeStr, nil];
  NXArray *longArray = [NXArray
      arrayWithObjects:safeStr, [NXString stringWithCString:"extra"], nil];
  BOOL differentLengthEqual = [shortArray isEqual:longArray];
  test_assert(differentLengthEqual == NO);
  test_assert([shortArray count] == 1);
  test_assert([longArray count] == 2);
  printf("✓ Different length arrays: returns NO\n");

  // Test 6: Arrays with identical content
  NXString *eqStr1 = [NXString stringWithCString:"hello"];
  NXString *eqStr2 = [NXString stringWithCString:"world"];
  NXArray *eqArray1 = [NXArray arrayWithObjects:eqStr1, eqStr2, nil];

  NXString *eqStr3 =
      [NXString stringWithCString:"hello"]; // Same content as eqStr1
  NXString *eqStr4 =
      [NXString stringWithCString:"world"]; // Same content as eqStr2
  NXArray *eqArray2 = [NXArray arrayWithObjects:eqStr3, eqStr4, nil];

  BOOL identicalContentEqual = [eqArray1 isEqual:eqArray2];
  test_assert(identicalContentEqual == YES);
  test_assert([eqArray1 count] == 2);
  test_assert([eqArray2 count] == 2);
  printf("✓ Identical content arrays: returns YES\n");

  // Test 7: Arrays with different content (same length)
  NXString *diffStr = [NXString stringWithCString:"different"];
  NXArray *eqArray3 = [NXArray arrayWithObjects:eqStr1, diffStr, nil];
  BOOL differentContentEqual = [eqArray1 isEqual:eqArray3];
  test_assert(differentContentEqual == NO);
  test_assert([eqArray1 count] == 2);
  test_assert([eqArray3 count] == 2);
  printf("✓ Different content arrays: returns NO\n");

  // Test 8: Arrays with different order (same elements)
  NXArray *reversedArray =
      [NXArray arrayWithObjects:eqStr2, eqStr1, nil]; // Reversed order
  BOOL differentOrderEqual = [eqArray1 isEqual:reversedArray];
  test_assert(differentOrderEqual == NO);
  test_assert([eqArray1 count] == 2);
  test_assert([reversedArray count] == 2);
  printf("✓ Different order arrays: returns NO\n");

  // Test 9: Nested array equality
  NXArray *nestedInner1 = [NXArray arrayWithObjects:eqStr1, nil];
  NXArray *nestedInner2 =
      [NXArray arrayWithObjects:eqStr1, nil]; // Same content as nestedInner1
  NXArray *nestedOuter1 = [NXArray arrayWithObjects:nestedInner1, eqStr2, nil];
  NXArray *nestedOuter2 = [NXArray arrayWithObjects:nestedInner2, eqStr2, nil];

  BOOL nestedEqual = [nestedOuter1 isEqual:nestedOuter2];
  test_assert(nestedEqual == YES);
  test_assert([nestedOuter1 count] == 2);
  test_assert([nestedOuter2 count] == 2);
  printf("✓ Nested arrays with same content: returns YES\n");

  // Test 10: Nested arrays with different content
  NXArray *differentNested = [NXArray arrayWithObjects:diffStr, nil];
  NXArray *nestedOuter3 =
      [NXArray arrayWithObjects:differentNested, eqStr2, nil];
  BOOL nestedDifferentEqual = [nestedOuter1 isEqual:nestedOuter3];
  test_assert(nestedDifferentEqual == NO);
  printf("✓ Nested arrays with different content: returns NO\n");

  // Test 11: Mixed object types
  NXNumber *equalNum1 = [NXNumber numberWithInt32:42];
  NXNumber *equalNum2 = [NXNumber numberWithInt32:42];    // Same value
  NXNumber *differentNum = [NXNumber numberWithInt32:99]; // Different value

  NXArray *mixedArray1 = [NXArray arrayWithObjects:eqStr1, equalNum1, nil];
  NXArray *mixedArray2 =
      [NXArray arrayWithObjects:eqStr3, equalNum2, nil]; // Same content
  NXArray *mixedArray3 =
      [NXArray arrayWithObjects:eqStr1, differentNum, nil]; // Different number

  BOOL mixedSameEqual = [mixedArray1 isEqual:mixedArray2];
  BOOL mixedDifferentEqual = [mixedArray1 isEqual:mixedArray3];
  test_assert(mixedSameEqual == YES);
  test_assert(mixedDifferentEqual == NO);
  printf("✓ Mixed object types: correctly compares using isEqual:\n");

  // Test 12: Single element arrays
  NXArray *singleArray1 = [NXArray arrayWithObjects:eqStr1, nil];
  NXArray *singleArray2 =
      [NXArray arrayWithObjects:eqStr3, nil]; // Same content
  NXArray *singleArray3 =
      [NXArray arrayWithObjects:eqStr2, nil]; // Different content

  BOOL singleSameEqual = [singleArray1 isEqual:singleArray2];
  BOOL singleDifferentEqual = [singleArray1 isEqual:singleArray3];
  test_assert(singleSameEqual == YES);
  test_assert(singleDifferentEqual == NO);
  printf("✓ Single element arrays: correctly compare single elements\n");

  // Test 13: Large arrays equality (performance test)
  NXArray *largeArray1 = [NXArray new];
  NXArray *largeArray2 = [NXArray new];

  // Add 20 identical elements to both arrays
  unsigned int k;
  for (k = 0; k < 20; k++) {
    NXString *element1 = [NXString stringWithCString:"element"];
    NXString *element2 = [NXString stringWithCString:"element"]; // Same content
    [largeArray1 append:element1];
    [largeArray2 append:element2];
  }

  BOOL largeEqual = [largeArray1 isEqual:largeArray2];
  test_assert(largeEqual == YES);
  test_assert([largeArray1 count] == 20);
  test_assert([largeArray2 count] == 20);

  // Make the last element different
  NXString *differentLast = [NXString stringWithCString:"different"];
  [largeArray2 removeObjectAtIndex:19];
  [largeArray2 append:differentLast];

  BOOL largeModifiedEqual = [largeArray1 isEqual:largeArray2];
  test_assert(largeModifiedEqual == NO);
  test_assert([largeArray1 count] == 20);
  test_assert([largeArray2 count] == 20);
  printf("✓ Large arrays: correctly handles performance and difference "
         "detection\n");

  // Test 27: stringWithObjectsJoinedByString: method testing
  printf("\nTest 27: Testing stringWithObjectsJoinedByString: functionality...\n");

  // Test 1: Empty array
  NXArray *emptyJoinArray = [NXArray new];
  NXString *emptyResult = [emptyJoinArray stringWithObjectsJoinedByString:@", "];
  test_assert(emptyResult != nil);
  test_assert(strcmp([emptyResult cStr], "") == 0);
  printf("✓ Empty array: returns empty string\n");

  // Test 2: Single object array with string
  NXString *singleStr = [NXString stringWithCString:"Hello"];
  NXArray *singleArray = [NXArray arrayWithObjects:singleStr, nil];
  NXString *singleResult = [singleArray stringWithObjectsJoinedByString:@", "];
  test_assert(singleResult != nil);
  test_assert(strcmp([singleResult cStr], "Hello") == 0);
  printf("✓ Single string object: returns object description without delimiter\n");

  // Test 3: Single object array with non-string
  NXNumber *singleNum = [NXNumber numberWithInt32:42];
  NXArray *singleNumArray = [NXArray arrayWithObjects:singleNum, nil];
  NXString *singleNumResult = [singleNumArray stringWithObjectsJoinedByString:@", "];
  test_assert(singleNumResult != nil);
  test_assert(strcmp([singleNumResult cStr], "42") == 0);
  printf("✓ Single non-string object: returns object description without delimiter\n");

  // Test 4: Multiple string objects
  NXString *str1 = [NXString stringWithCString:"Apple"];
  NXString *str2 = [NXString stringWithCString:"Banana"];
  NXString *str3 = [NXString stringWithCString:"Cherry"];
  NXArray *stringArray = [NXArray arrayWithObjects:str1, str2, str3, nil];
  NXString *stringResult = [stringArray stringWithObjectsJoinedByString:@", "];
  test_assert(stringResult != nil);
  test_assert(strcmp([stringResult cStr], "Apple, Banana, Cherry") == 0);
  printf("✓ Multiple strings: correctly joins with delimiter\n");

  // Test 5: Multiple mixed objects (strings and numbers)
  NXNumber *joinNum1 = [NXNumber numberWithInt32:1];
  NXNumber *joinNum2 = [NXNumber numberWithInt32:2];
  NXString *str4 = [NXString stringWithCString:"three"];
  NXArray *joinMixedArray = [NXArray arrayWithObjects:joinNum1, str4, joinNum2, nil];
  NXString *mixedResult = [joinMixedArray stringWithObjectsJoinedByString:@"-"];
  test_assert(mixedResult != nil);
  test_assert(strcmp([mixedResult cStr], "1-three-2") == 0);
  printf("✓ Mixed objects: correctly joins numbers and strings\n");

  // Test 6: Different delimiter
  NXArray *delimArray = [NXArray arrayWithObjects:str1, str2, nil];
  NXString *delimResult = [delimArray stringWithObjectsJoinedByString:@" | "];
  test_assert(delimResult != nil);
  test_assert(strcmp([delimResult cStr], "Apple | Banana") == 0);
  printf("✓ Different delimiter: correctly uses custom delimiter\n");

  // Test 7: Single character delimiter
  NXString *charResult = [delimArray stringWithObjectsJoinedByString:@"/"];
  test_assert(charResult != nil);
  test_assert(strcmp([charResult cStr], "Apple/Banana") == 0);
  printf("✓ Single character delimiter: works correctly\n");

  // Test 8: Empty delimiter
  NXString *emptyDelimResult = [delimArray stringWithObjectsJoinedByString:@""];
  test_assert(emptyDelimResult != nil);
  test_assert(strcmp([emptyDelimResult cStr], "AppleBanana") == 0);
  printf("✓ Empty delimiter: concatenates without separator\n");

  // Test 9: Many objects (performance test)
  NXArray *manyArray = [NXArray new];
  for (int i = 0; i < 10; i++) {
    NXString *numStr = [NXString stringWithFormat:@"item%d", i];
    [manyArray append:numStr];
  }
  NXString *manyResult = [manyArray stringWithObjectsJoinedByString:@","];
  test_assert(manyResult != nil);
  test_assert(strcmp([manyResult cStr], "item0,item1,item2,item3,item4,item5,item6,item7,item8,item9") == 0);
  printf("✓ Many objects: correctly handles larger arrays\n");

  // Test 10: Length validation - verify the result length matches expected
  NXArray *lengthTestArray = [NXArray arrayWithObjects:
    [NXString stringWithCString:"AB"], 
    [NXString stringWithCString:"CD"], 
    [NXString stringWithCString:"EF"], 
    nil];
  NXString *lengthResult = [lengthTestArray stringWithObjectsJoinedByString:@"XX"];
  test_assert(lengthResult != nil);
  test_assert(strcmp([lengthResult cStr], "ABXXCDXXEF") == 0);
  test_assert([lengthResult length] == 10); // 2+2+2+2+2 = 10 characters
  printf("✓ Length validation: result length matches expected calculation\n");

  printf("✓ Test 27 completed: stringWithObjectsJoinedByString: works correctly\n");

  // Note: testStr1, testStr2, testStr3, testArray, nestedArray, outerArray,
  // emptyTestArray, appendArray are autoreleased No manual releases needed for
  // these objects

  // Clean up allocated objects
  [basicObject release];
  [object1 release];
  [object2 release];

  printf("\n⚠️  Note: NXArray implementation now supports all major array "
         "operations with comprehensive testing\n");
  printf("    ✅ Implemented: initWithObjects:, arrayWithObjects:, JSONString, "
         "JSONBytes, containsObject:, append:, insert:atIndex:, "
         "indexForObject:, remove:, removeObjectAtIndex:, isEqual:, stringWithObjectsJoinedByString:\n");
  printf("    ✅ Handles non-JSON-compliant objects via description method\n");
  printf("    ✅ Supports recursive collection search in containsObject:\n");
  printf(
      "    ✅ Dynamic array growth with append: and insert:atIndex: methods\n");
  printf("    ✅ Array element removal with remove: and removeObjectAtIndex: "
         "methods\n");
  printf("    ✅ Circular reference prevention for mutation operations\n");
  printf("    ✅ Comprehensive equality testing with isEqual: method\n");
  printf("    ✅ Comprehensive edge case coverage: memory management, "
         "boundaries, nested arrays\n");
  printf("    ✅ Protocol conformance and polymorphic behavior validation\n");
  printf("    ✅ Performance characteristics and stress testing\n");
  printf("    ✅ JSON serialization edge cases and accuracy verification\n");
  printf("    ⚠️  objectAtIndex: uses assertions for bounds checking\n");

  printf("\n✅ All NXArray methods and edge cases tested comprehensively (27 "
         "test suites)!\n");
  return 0;
}
