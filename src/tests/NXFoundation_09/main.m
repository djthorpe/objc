#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  printf("Starting NXDate comprehensive tests...\n");

  // Memory management
  NXZone *zone = [NXZone zoneWithSize:1024];
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  // Test the underlying sys_time_get_utc function first
  sys_time_t test_time;
  bool time_success = sys_time_get_utc(&test_time);
  printf("sys_time_get_utc success: %d\n", time_success);
  if (time_success) {
    printf("Current time: %lld seconds, %d nanoseconds\n",
           (long long)test_time.seconds, test_time.nanoseconds);
  }
  test_assert(time_success);

  // Test 1: Create current date
  printf("\nTest 1: Creating current date...\n");
  NXDate *currentDate = [NXDate date];
  test_assert(currentDate != nil);

  NXString *desc = [currentDate description];
  test_assert(desc != nil);
  printf("✓ Current date: %s\n", [desc cStr]);

  // Test 2: Create date with positive time interval
  printf("\nTest 2: Creating date 5 seconds in the future...\n");
  NXDate *futureDate =
      [NXDate dateWithTimeIntervalSinceNow:5 * Second]; // 5 seconds
  test_assert(futureDate != nil);

  NXString *futureDesc = [futureDate description];
  test_assert(futureDesc != nil);
  printf("✓ Future date (+5s): %s\n", [futureDesc cStr]);

  // Test 3: Create date with negative time interval
  printf("\nTest 3: Creating date 10 seconds in the past...\n");
  NXDate *pastDate =
      [NXDate dateWithTimeIntervalSinceNow:-10 * Second]; // -10 seconds
  test_assert(pastDate != nil);

  NXString *pastDesc = [pastDate description];
  test_assert(pastDesc != nil);
  printf("✓ Past date (-10s): %s\n", [pastDesc cStr]);

  // Test 4: Create date with large positive interval (1 hour)
  printf("\nTest 4: Creating date 1 hour in the future...\n");
  NXDate *hourFuture = [NXDate dateWithTimeIntervalSinceNow:Hour]; // 1 hour
  test_assert(hourFuture != nil);

  NXString *hourDesc = [hourFuture description];
  test_assert(hourDesc != nil);
  printf("✓ Future date (+1h): %s\n", [hourDesc cStr]);

  // Test 5: Create date with large negative interval (1 day)
  printf("\nTest 5: Creating date 1 day in the past...\n");
  NXDate *dayPast = [NXDate dateWithTimeIntervalSinceNow:-Day]; // -1 day
  test_assert(dayPast != nil);

  NXString *dayDesc = [dayPast description];
  test_assert(dayDesc != nil);
  printf("✓ Past date (-1d): %s\n", [dayDesc cStr]);

  // Test 6: Test zero time interval
  printf("\nTest 6: Creating date with zero interval...\n");
  NXDate *zeroDate = [NXDate dateWithTimeIntervalSinceNow:0];
  test_assert(zeroDate != nil);

  NXString *zeroDesc = [zeroDate description];
  test_assert(zeroDesc != nil);
  printf("✓ Zero interval date: %s\n", [zeroDesc cStr]);

  // Test 7: Test multiple date objects
  printf("\nTest 7: Creating multiple date objects...\n");
  NXDate *date1 = [NXDate date];
  NXDate *date2 = [NXDate date];
  NXDate *date3 = [NXDate date];

  test_assert(date1 != nil && date2 != nil && date3 != nil);
  test_assert(date1 != date2 && date2 != date3 && date1 != date3);
  printf("✓ Created 3 distinct date objects\n");

  // Test 8: Test edge case - very small interval
  printf("\nTest 8: Testing very small time interval (1ms)...\n");
  NXDate *millisecondDate = [NXDate dateWithTimeIntervalSinceNow:Millisecond];
  test_assert(millisecondDate != nil);

  NXString *msDesc = [millisecondDate description];
  test_assert(msDesc != nil);
  printf("✓ Millisecond precision date: %s\n", [msDesc cStr]);

  // Test 9: Test nanosecond overflow handling
  printf("\nTest 9: Testing nanosecond overflow (999ms)...\n");
  NXDate *overflowDate =
      [NXDate dateWithTimeIntervalSinceNow:999 * Millisecond];
  test_assert(overflowDate != nil);

  NXString *overflowDesc = [overflowDate description];
  test_assert(overflowDesc != nil);
  printf("✓ Overflow handling date: %s\n", [overflowDesc cStr]);

  // Test 10: Test description string persistence
  printf("\nTest 10: Testing description string stability...\n");
  NXDate *testDate = [NXDate date];
  test_assert(testDate != nil);

  NXString *desc1 = [testDate description];
  NXString *desc2 = [testDate description];

  test_assert(desc1 != nil && desc2 != nil);
  test_assert(strcmp([desc1 cStr], [desc2 cStr]) == 0);
  printf("✓ Description strings are consistent\n");

  // Cleanup
  [pool release];
  [zone release];

  printf("\n🎉 All NXDate tests passed successfully!\n");
  return 0;
}
