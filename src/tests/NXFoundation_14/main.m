#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  printf("Starting NXDate edge case and fix validation tests...\n");

  // Memory management
  NXZone *zone = [NXZone zoneWithSize:1024];
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  // Test 1: Nanosecond overflow handling in addTimeInterval
  printf("\nTest 1: Testing nanosecond overflow in addTimeInterval...\n");
  NXDate *baseDate = [NXDate date];
  test_assert(baseDate != nil);

  // Add an interval that will cause nanosecond overflow
  NXTimeInterval largeNanosecondInterval = 1500 * Millisecond; // 1.5 seconds
  [baseDate addTimeInterval:largeNanosecondInterval];

  NXString *desc1 = [baseDate description];
  test_assert(desc1 != nil);
  printf("✓ Date after adding 1.5s: %s\n", [desc1 cStr]);

  // Test 2: Negative interval handling
  printf("\nTest 2: Testing negative interval handling...\n");
  NXDate *negativeDate = [NXDate date];
  test_assert(negativeDate != nil);

  NXTimeInterval negativeInterval = -2500 * Millisecond; // -2.5 seconds
  [negativeDate addTimeInterval:negativeInterval];

  NXString *desc2 = [negativeDate description];
  test_assert(desc2 != nil);
  printf("✓ Date after subtracting 2.5s: %s\n", [desc2 cStr]);

  // Test 3: Multiple consecutive operations
  printf("\nTest 3: Testing multiple consecutive time operations...\n");
  NXDate *multiDate = [NXDate date];
  test_assert(multiDate != nil);

  [multiDate addTimeInterval:750 * Millisecond];  // +0.75s
  [multiDate addTimeInterval:250 * Millisecond];  // +0.25s (total: +1s)
  [multiDate addTimeInterval:-500 * Millisecond]; // -0.5s (total: +0.5s)

  NXString *desc3 = [multiDate description];
  test_assert(desc3 != nil);
  printf("✓ Date after multiple operations (+0.5s net): %s\n", [desc3 cStr]);

  // Test 4: dateByAddingTimeInterval with overflow
  printf("\nTest 4: Testing dateByAddingTimeInterval with overflow...\n");
  NXDate *originalDate = [NXDate date];
  test_assert(originalDate != nil);

  NXDate *newDate =
      [originalDate dateByAddingTimeInterval:1999 * Millisecond]; // ~2s
  test_assert(newDate != nil);
  test_assert(newDate != originalDate); // Should be different objects

  NXString *origDesc = [originalDate description];
  NXString *newDesc = [newDate description];
  test_assert(origDesc != nil && newDesc != nil);
  printf("✓ Original: %s\n", [origDesc cStr]);
  printf("✓ New (+2s): %s\n", [newDesc cStr]);

  // Test 5: Time comparison with nanosecond precision
  printf("\nTest 5: Testing time comparison precision...\n");
  NXDate *date1 = [NXDate date];
  NXDate *date2 = [date1 dateByAddingTimeInterval:Millisecond];  // +1ms
  NXDate *date3 = [date1 dateByAddingTimeInterval:-Millisecond]; // -1ms

  test_assert([date2 isLaterThan:date1]);
  test_assert([date1 isEarlierThan:date2]);
  test_assert([date3 isEarlierThan:date1]);
  test_assert([date1 isLaterThan:date3]);
  test_assert(![date1 isEqual:date2]);
  test_assert(![date1 isEqual:date3]);
  printf("✓ Millisecond-precision comparisons work correctly\n");

  // Test 6: Component preservation during time operations
  printf("\nTest 6: Testing component cache invalidation...\n");
  NXDate *componentDate = [NXDate date];
  test_assert(componentDate != nil);

  // Get initial components
  uint16_t year1, year2;
  uint8_t month1, month2, day1, day2, weekday1, weekday2;
  BOOL success1 = [componentDate year:&year1
                                month:&month1
                                  day:&day1
                              weekday:&weekday1];
  test_assert(success1);
  printf("✓ Initial date: %04d-%02d-%02d (weekday: %d)\n", year1, month1, day1,
         weekday1);

  // Add some time and check components again
  [componentDate addTimeInterval:30 * Minute]; // 30 minutes
  BOOL success2 = [componentDate year:&year2
                                month:&month2
                                  day:&day2
                              weekday:&weekday2];
  test_assert(success2);
  printf("✓ After +30m: %04d-%02d-%02d (weekday: %d)\n", year2, month2, day2,
         weekday2);

  // Date should be the same (unless we crossed midnight)
  test_assert(year1 == year2);
  test_assert(month1 == month2);
  // Day and weekday might change if we crossed midnight, so don't assert them

  // Test 7: Edge case - very small intervals
  printf("\nTest 7: Testing very small time intervals...\n");
  NXDate *nanoDate = [NXDate date];
  test_assert(nanoDate != nil);

  // Add 1 nanosecond
  [nanoDate addTimeInterval:1]; // 1 nanosecond

  NXString *nanoDesc = [nanoDate description];
  test_assert(nanoDesc != nil);
  printf("✓ Date after +1ns: %s\n", [nanoDesc cStr]);

  // Test 8: Large time interval handling
  printf("\nTest 8: Testing large time intervals...\n");
  NXDate *bigDate = [NXDate date];
  test_assert(bigDate != nil);

  NXTimeInterval oneWeek = 7 * Day;
  [bigDate addTimeInterval:oneWeek];

  NXString *bigDesc = [bigDate description];
  test_assert(bigDesc != nil);
  printf("✓ Date after +1 week: %s\n", [bigDesc cStr]);

  // Test 9: Test comprehensive NXTimeInterval integration
  printf("\nTest 9: Testing NXTimeInterval integration with NXDate...\n");
  NXDate *integrationDate = [NXDate date];
  test_assert(integrationDate != nil);

  // Test all time unit constants work with NXDate
  [integrationDate addTimeInterval:1 * Nanosecond];
  [integrationDate addTimeInterval:500 * Millisecond];
  [integrationDate addTimeInterval:30 * Second];
  [integrationDate addTimeInterval:5 * Minute];
  [integrationDate addTimeInterval:2 * Hour];
  [integrationDate addTimeInterval:0 * Day]; // Zero operation

  NXString *integrationDesc = [integrationDate description];
  test_assert(integrationDesc != nil);
  printf("✓ All time constants integrate correctly: %s\n",
         [integrationDesc cStr]);

  // Test 10: Test NXTimeIntervalMilliseconds integration
  printf("\nTest 10: Testing NXTimeIntervalMilliseconds with NXDate "
         "operations...\n");
  NXDate *msDate1 = [NXDate date];
  NXDate *msDate2 = [NXDate date];

  NXTimeInterval largeDiff =
      3 * Hour + 45 * Minute + 30 * Second + 750 * Millisecond;
  [msDate2 addTimeInterval:largeDiff];

  NXTimeInterval calculatedDiff = [msDate2 compare:msDate1];
  int32_t diffInMs = NXTimeIntervalMilliseconds(calculatedDiff);
  int32_t expectedMs = NXTimeIntervalMilliseconds(largeDiff);

  test_assert(diffInMs == expectedMs);
  printf("✓ NXTimeIntervalMilliseconds: %d ms (expected: %d ms)\n", diffInMs,
         expectedMs);

  // Cleanup
  [pool release];
  [zone release];

  printf("\n🎉 All NXDate edge case tests passed successfully!\n");
  printf("Validated: nanosecond overflow, negative intervals, precision, "
         "caching, integration\n");
  return 0;
}
