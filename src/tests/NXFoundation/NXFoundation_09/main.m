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

  // Test the underlying sys_date_get_now function first
  sys_date_t test_date;
  bool date_success = sys_date_get_now(&test_date);
  printf("sys_date_get_now success: %d\n", date_success);
  if (date_success) {
    printf("Current date: %lld seconds, %d nanoseconds\n",
           (long long)test_date.seconds, test_date.nanoseconds);
  }
  test_assert(date_success);

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

  // Test 11: Test date component getters
  printf("\nTest 11: Testing date component getters...\n");
  NXDate *componentDate = [NXDate date];
  test_assert(componentDate != nil);

  uint16_t year;
  uint8_t month, day, weekday;
  BOOL dateSuccess = [componentDate year:&year
                                   month:&month
                                     day:&day
                                 weekday:&weekday];
  test_assert(dateSuccess);
  test_assert(year >= 1970 && year <= 9999);
  test_assert(month >= 1 && month <= 12);
  test_assert(day >= 1 && day <= 31);
  test_assert(weekday <= 6);
  printf("✓ Date components: %04d-%02d-%02d (weekday: %d)\n", year, month, day,
         weekday);

  // Test 12: Test time component getters
  printf("\nTest 12: Testing time component getters...\n");
  uint8_t hours, minutes, seconds;
  uint32_t nanoseconds;
  BOOL timeSuccess = [componentDate hours:&hours
                                  minutes:&minutes
                                  seconds:&seconds
                              nanoseconds:&nanoseconds];
  test_assert(timeSuccess);
  test_assert(hours <= 23);
  test_assert(minutes <= 59);
  test_assert(seconds <= 59);
  test_assert(nanoseconds < 1000000000);
  printf("✓ Time components: %02d:%02d:%02d.%09d\n", hours, minutes, seconds,
         nanoseconds);

  // Test 13: Test date comparison methods
  printf("\nTest 13: Testing date comparison methods...\n");
  NXDate *baseDate = [NXDate date];
  NXDate *laterDate = [NXDate dateWithTimeIntervalSinceNow:5 * Second];
  NXDate *earlierDate = [NXDate dateWithTimeIntervalSinceNow:-5 * Second];

  // Test compare method directly
  NXTimeInterval diff1 = [baseDate compare:laterDate];
  NXTimeInterval diff2 = [baseDate compare:earlierDate];
  test_assert(diff1 < 0); // baseDate is earlier than laterDate
  test_assert(diff2 > 0); // baseDate is later than earlierDate

  // Test comparison convenience methods
  test_assert([baseDate isEarlierThan:laterDate]);
  test_assert([baseDate isLaterThan:earlierDate]);
  test_assert(![baseDate isEarlierThan:earlierDate]);
  test_assert(![baseDate isLaterThan:laterDate]);
  printf("✓ Date comparison methods work correctly\n");

  // Test 14: Test addTimeInterval method
  printf("\nTest 14: Testing addTimeInterval method...\n");
  NXDate *mutableDate = [NXDate date];
  test_assert(mutableDate != nil);

  NXString *beforeDesc = [mutableDate description];
  [mutableDate addTimeInterval:3 * Hour + 15 * Minute]; // Add 3h 15m
  NXString *afterDesc = [mutableDate description];

  test_assert(beforeDesc != nil && afterDesc != nil);
  test_assert(strcmp([beforeDesc cStr], [afterDesc cStr]) !=
              0); // Should be different
  printf("✓ Before: %s\n", [beforeDesc cStr]);
  printf("✓ After (+3h15m): %s\n", [afterDesc cStr]);

  // Test 15: Test dateByAddingTimeInterval method
  printf("\nTest 15: Testing dateByAddingTimeInterval method...\n");
  NXDate *originalDate = [NXDate date];
  NXDate *derivedDate =
      [originalDate dateByAddingTimeInterval:-2 * Day]; // 2 days ago

  test_assert(originalDate != nil && derivedDate != nil);
  test_assert(originalDate != derivedDate); // Different instances
  test_assert([derivedDate isEarlierThan:originalDate]);

  NXString *origDesc = [originalDate description];
  NXString *derivedDesc = [derivedDate description];
  printf("✓ Original: %s\n", [origDesc cStr]);
  printf("✓ Derived (-2d): %s\n", [derivedDesc cStr]);

  // Test 16: Test setter methods
  printf("\nTest 16: Testing date/time setter methods...\n");
  NXDate *setterDate = [NXDate date];
  test_assert(setterDate != nil);

  // Test date setter
  BOOL dateSetSuccess = [setterDate setYear:2024 month:12 day:25];
  test_assert(dateSetSuccess);

  uint16_t newYear;
  uint8_t newMonth, newDay;
  [setterDate year:&newYear month:&newMonth day:&newDay weekday:NULL];
  test_assert(newYear == 2024);
  test_assert(newMonth == 12);
  test_assert(newDay == 25);
  printf("✓ Date setter: Set to 2024-12-25\n");

  // Test time setter
  BOOL timeSetSuccess = [setterDate setHours:14
                                     minutes:30
                                     seconds:45
                                 nanoseconds:123456789];
  test_assert(timeSetSuccess);

  uint8_t newHours, newMinutes, newSeconds;
  uint32_t newNanoseconds;
  [setterDate hours:&newHours
            minutes:&newMinutes
            seconds:&newSeconds
        nanoseconds:&newNanoseconds];
  test_assert(newHours == 14);
  test_assert(newMinutes == 30);
  test_assert(newSeconds == 45);
  test_assert(newNanoseconds == 123456789);
  printf("✓ Time setter: Set to 14:30:45.123456789\n");

  NXString *setterDesc = [setterDate description];
  printf("✓ Final date: %s\n", [setterDesc cStr]);

  // Test 17: Test invalid setter parameters
  printf("\nTest 17: Testing invalid setter parameter validation...\n");
  NXDate *validationDate = [NXDate date];

  // Invalid date parameters
  test_assert(![validationDate setYear:0 month:1 day:1]);     // Invalid year
  test_assert(![validationDate setYear:2025 month:0 day:1]);  // Invalid month
  test_assert(![validationDate setYear:2025 month:13 day:1]); // Invalid month
  test_assert(![validationDate setYear:2025 month:1 day:0]);  // Invalid day
  test_assert(![validationDate setYear:2025 month:1 day:32]); // Invalid day

  // Invalid time parameters
  test_assert(![validationDate setHours:24
                                minutes:0
                                seconds:0
                            nanoseconds:0]); // Invalid hour
  test_assert(![validationDate setHours:0
                                minutes:60
                                seconds:0
                            nanoseconds:0]); // Invalid minute
  test_assert(![validationDate setHours:0
                                minutes:0
                                seconds:60
                            nanoseconds:0]); // Invalid second
  test_assert(![validationDate setHours:0
                                minutes:0
                                seconds:0
                            nanoseconds:1000000000]); // Invalid nanoseconds
  printf("✓ Invalid parameter validation works correctly\n");

  // Cleanup
  [pool release];
  [zone release];

  printf("\n🎉 All NXDate comprehensive tests passed successfully!\n");
  printf("Tested: Creation, intervals, components, comparisons, setters, "
         "validation\n");
  return 0;
}
