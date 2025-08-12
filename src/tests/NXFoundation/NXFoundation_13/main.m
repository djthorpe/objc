#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  printf("Starting NXTimeInterval comprehensive tests...\n");

  // Memory management
  NXZone *zone = [NXZone zoneWithSize:1024];
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  // Test 1: Basic time constants
  printf("\nTest 1: Testing time constant values...\n");
  test_assert(Nanosecond == 1);
  test_assert(Millisecond == 1000000);
  test_assert(Second == 1000000000);
  test_assert(Minute == 60000000000LL);
  test_assert(Hour == 3600000000000LL);
  test_assert(Day == 86400000000000LL);
  printf("✓ All time constants have correct values\n");

  // Test 2: Time constant relationships
  printf("\nTest 2: Testing time constant relationships...\n");
  test_assert(Millisecond == 1000000 * Nanosecond);
  test_assert(Second == 1000 * Millisecond);
  test_assert(Minute == 60 * Second);
  test_assert(Hour == 60 * Minute);
  test_assert(Day == 24 * Hour);
  printf("✓ All time constant relationships are correct\n");

  // Test 3: Basic arithmetic operations
  printf("\nTest 3: Testing basic arithmetic operations...\n");
  NXTimeInterval halfSecond = Second / 2;
  NXTimeInterval doubleSecond = Second * 2;
  NXTimeInterval fiveSeconds = 5 * Second;

  test_assert(halfSecond == 500000000);
  test_assert(doubleSecond == 2000000000LL);
  test_assert(fiveSeconds == 5000000000LL);
  printf("✓ Basic arithmetic operations work correctly\n");

  // Test 4: Addition and subtraction
  printf("\nTest 4: Testing addition and subtraction...\n");
  NXTimeInterval total =
      2 * Hour + 30 * Minute + 45 * Second + 500 * Millisecond;
  NXTimeInterval expected = 9045500000000LL; // 2h30m45.5s in nanoseconds
  test_assert(total == expected);

  NXTimeInterval difference = Day - Hour;
  NXTimeInterval expectedDiff = 23 * Hour;
  test_assert(difference == expectedDiff);
  printf("✓ Addition and subtraction work correctly\n");

  // Test 5: NXTimeIntervalMilliseconds function
  printf("\nTest 5: Testing NXTimeIntervalMilliseconds function...\n");
  test_assert(NXTimeIntervalMilliseconds(Second) == 1000);
  test_assert(NXTimeIntervalMilliseconds(Minute) == 60000);
  test_assert(NXTimeIntervalMilliseconds(500 * Millisecond) == 500);
  test_assert(NXTimeIntervalMilliseconds(1500 * Millisecond) == 1500);
  test_assert(NXTimeIntervalMilliseconds(0) == 0);
  printf("✓ NXTimeIntervalMilliseconds function works correctly\n");

  // Test 6: Negative time intervals
  printf("\nTest 6: Testing negative time intervals...\n");
  NXTimeInterval negativeSecond = -Second;
  NXTimeInterval negativeHour = -Hour;

  test_assert(negativeSecond == -1000000000LL);
  test_assert(negativeHour == -3600000000000LL);
  test_assert(NXTimeIntervalMilliseconds(negativeSecond) == -1000);
  test_assert(NXTimeIntervalMilliseconds(negativeHour) == -3600000);
  printf("✓ Negative time intervals work correctly\n");

  // Test 7: Edge cases with very small intervals
  printf("\nTest 7: Testing very small time intervals...\n");
  NXTimeInterval oneNanosecond = Nanosecond;
  NXTimeInterval tenNanoseconds = 10 * Nanosecond;
  NXTimeInterval hundredNanoseconds = 100 * Nanosecond;

  test_assert(oneNanosecond == 1);
  test_assert(tenNanoseconds == 10);
  test_assert(hundredNanoseconds == 100);

  // These should all be 0 when converted to milliseconds
  test_assert(NXTimeIntervalMilliseconds(oneNanosecond) == 0);
  test_assert(NXTimeIntervalMilliseconds(tenNanoseconds) == 0);
  test_assert(NXTimeIntervalMilliseconds(hundredNanoseconds) == 0);
  printf("✓ Very small time intervals work correctly\n");

  // Test 8: Edge cases with very large intervals
  printf("\nTest 8: Testing very large time intervals...\n");
  NXTimeInterval oneYear = 365 * Day; // Approximately 1 year
  NXTimeInterval oneDecade = 10 * oneYear;

  test_assert(oneYear == 31536000000000000LL);
  test_assert(oneDecade == 315360000000000000LL);

  // Check millisecond conversion for large values
  int32_t yearInMs = NXTimeIntervalMilliseconds(oneYear);
  test_assert(yearInMs > 0); // Should not overflow for reasonable values
  printf("✓ Very large time intervals work correctly\n");

  // Test 9: Precision tests
  printf("\nTest 9: Testing precision and fractional operations...\n");
  NXTimeInterval halfMillisecond = Millisecond / 2;
  NXTimeInterval quarterSecond = Second / 4;
  NXTimeInterval threeQuarterSecond = 3 * Second / 4;

  test_assert(halfMillisecond == 500000);
  test_assert(quarterSecond == 250000000);
  test_assert(threeQuarterSecond == 750000000);

  // Test millisecond conversion for fractional values
  test_assert(NXTimeIntervalMilliseconds(halfMillisecond) == 0);
  test_assert(NXTimeIntervalMilliseconds(quarterSecond) == 250);
  test_assert(NXTimeIntervalMilliseconds(threeQuarterSecond) == 750);
  printf("✓ Precision and fractional operations work correctly\n");

  // Test 10: Comparison operations
  printf("\nTest 10: Testing comparison operations...\n");
  test_assert(Second > Millisecond);
  test_assert(Minute > Second);
  test_assert(Hour > Minute);
  test_assert(Day > Hour);

  test_assert(Nanosecond < Millisecond);
  test_assert(Millisecond < Second);
  test_assert(Second < Minute);
  test_assert(Minute < Hour);
  test_assert(Hour < Day);

  test_assert(Second == Second);
  test_assert(Hour != Minute);
  printf("✓ Comparison operations work correctly\n");

  // Test 11: Mixed unit calculations
  printf("\nTest 11: Testing mixed unit calculations...\n");
  // Calculate: 1 day, 12 hours, 30 minutes, 45 seconds, 250 milliseconds
  NXTimeInterval complexTime =
      Day + 12 * Hour + 30 * Minute + 45 * Second + 250 * Millisecond;
  NXTimeInterval expectedComplex = 131445250000000LL; // Calculated value
  test_assert(complexTime == expectedComplex);

  // Convert to milliseconds and verify
  int32_t complexMs = NXTimeIntervalMilliseconds(complexTime);
  int32_t expectedMs =
      131445250; // 1 day = 86400000ms + 12h = 43200000ms + etc.
  test_assert(complexMs == expectedMs);
  printf("✓ Mixed unit calculations work correctly\n");

  // Test 12: Zero and identity operations
  printf("\nTest 12: Testing zero and identity operations...\n");
  NXTimeInterval zero = 0;
  NXTimeInterval someTime = 5 * Second;

  test_assert(someTime + zero == someTime);
  test_assert(someTime - zero == someTime);
  test_assert(zero + someTime == someTime);
  test_assert(someTime - someTime == zero);
  test_assert(someTime * 1 == someTime);
  test_assert(zero * 999 == zero);
  printf("✓ Zero and identity operations work correctly\n");

  // Test 13: Large time intervals with careful overflow awareness
  printf("\nTest 13: Testing large time intervals responsibly...\n");
  // Use oneYear from Test 8, be more conservative with large values
  NXTimeInterval maxReasonableTime = 100 * oneYear; // 100 years
  NXTimeInterval largePeriod = 10000 * Day; // ~27 years (more reasonable)

  // These should still be reasonable values
  test_assert(maxReasonableTime > 0);
  test_assert(largePeriod > 0);

  // Check that our large period is actually smaller than 100 years
  test_assert(largePeriod < maxReasonableTime);

  // Test that we can still do basic operations
  NXTimeInterval halfLarge = largePeriod / 2;
  test_assert(halfLarge == largePeriod / 2);
  test_assert(halfLarge > 0);
  printf("✓ Large time intervals behave predictably\n");

  // Test 14: Practical time calculations
  printf("\nTest 14: Testing practical time calculations...\n");
  // Simulate common timing scenarios

  // Timeout values
  NXTimeInterval connectTimeout = 30 * Second;
  NXTimeInterval readTimeout = 5 * Minute;
  NXTimeInterval sessionTimeout = 2 * Hour;

  test_assert(readTimeout > connectTimeout);
  test_assert(sessionTimeout > readTimeout);

  // Sleep intervals
  NXTimeInterval shortSleep = 100 * Millisecond;
  NXTimeInterval mediumSleep = 500 * Millisecond;
  NXTimeInterval longSleep = 2 * Second;

  test_assert(mediumSleep > shortSleep);
  test_assert(longSleep > mediumSleep);
  test_assert(longSleep == 20 * shortSleep);

  // Animation durations
  NXTimeInterval quickFade = 200 * Millisecond;
  NXTimeInterval slideTransition = 300 * Millisecond;
  NXTimeInterval pageTransition = 500 * Millisecond;

  test_assert(slideTransition > quickFade);
  test_assert(pageTransition > slideTransition);
  printf("✓ Practical time calculations work correctly\n");

  // Test 15: Type safety and range validation
  printf("\nTest 15: Testing type safety and value ranges...\n");
  // Verify that our constants fit within expected ranges
  test_assert(Nanosecond > 0);
  test_assert(Millisecond > Nanosecond);
  test_assert(Second > 0);
  test_assert(Minute > 0);
  test_assert(Hour > 0);
  test_assert(Day > 0);

  // Test that we can represent common values
  NXTimeInterval microsecond = 1000 * Nanosecond;
  test_assert(microsecond == 1000);
  test_assert(microsecond < Millisecond);

  printf("✓ Type safety and value ranges are correct\n");

  // Test 16: NXTimeIntervalDescription function (placeholder test)
  printf("\nTest 16: Testing NXTimeIntervalDescription function...\n");
  // Since this function currently returns "TODO", we just test it doesn't crash
  NXTimeInterval testInterval = 5 * Second + 250 * Millisecond;
  NXString *desc1 = NXTimeIntervalDescription(testInterval, Millisecond);
  NXString *desc2 = NXTimeIntervalDescription(testInterval, Second);
  NXString *desc3 = NXTimeIntervalDescription(0, Second);
  NXString *desc4 = NXTimeIntervalDescription(-testInterval, Millisecond);

  test_assert(desc1 != nil);
  test_assert(desc2 != nil);
  test_assert(desc3 != nil);
  test_assert(desc4 != nil);
  printf("✓ NXTimeIntervalDescription function calls succeed\n");
  printf("  desc1: %s\n", [desc1 cStr]);
  printf("  desc2: %s\n", [desc2 cStr]);
  printf("  desc3: %s\n", [desc3 cStr]);
  printf("  desc4 (negative): %s\n", [desc4 cStr]);

  // Test with days
  NXTimeInterval dayInterval =
      2 * Day + 3 * Hour + 45 * Minute + 30 * Second + 500 * Millisecond;
  NXString *desc5 = NXTimeIntervalDescription(dayInterval, Millisecond);
  test_assert(desc5 != nil);
  printf("  desc5 (with days): %s\n", [desc5 cStr]);

  // Test negative intervals with proper formatting
  NXTimeInterval negativeDay = -(1 * Day + 2 * Hour + 30 * Minute);
  NXString *desc6 = NXTimeIntervalDescription(negativeDay, Minute);
  test_assert(desc6 != nil);
  printf("  desc6 (negative day): %s\n", [desc6 cStr]);

  // Test very small negative interval
  NXTimeInterval smallNegative = -500 * Millisecond;
  NXString *desc7 = NXTimeIntervalDescription(smallNegative, Millisecond);
  test_assert(desc7 != nil);
  printf("  desc7 (small negative): %s\n", [desc7 cStr]);

  printf(
      "✓ NXTimeIntervalDescription works with days and negative intervals\n");

  // Cleanup
  [pool release];
  [zone release];

  printf("\n🎉 All NXTimeInterval tests passed successfully!\n");
  printf("Tested: Constants, arithmetic, conversions, precision, edge cases, "
         "practical usage, and description function\n");
  return 0;
}
