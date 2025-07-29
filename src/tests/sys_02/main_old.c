#include <inttypes.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>
#include <time.h>

// Forward declaration
int test_sys_02(void);

int main(void) {
  // Run sys_printf tests
  return TestMain("test_sys_02", test_sys_02);
}

int test_sys_02(void) {
  // Test 1: Basic sys_date_get_now functionality
  do {
    sys_date_t date;
    bool result = sys_date_get_now(&date);

    test_assert(result == true);
    test_assert(date.seconds >= 0); // Should be zero or positive
    test_assert(date.nanoseconds >= 0 &&
                date.nanoseconds < 1000000000); // Valid nanoseconds range

    sys_printf("sys_date_get_now: seconds=%ld, nanoseconds=%ld, tzoffset=%ld\n", date.seconds,
               date.nanoseconds, date.tzoffset);
  } while (0);

  // Test 2: sys_date_get_now with NULL parameter
  do {
    bool result = sys_date_get_now(NULL);
    test_assert(result == false);
    sys_printf("sys_date_get_now(NULL) correctly returned false\n");
  } while (0);

  // Test 3: Time consistency - two calls should be very close
  do {
    sys_date_t date1, date2;
    bool result1 = sys_date_get_now(&date1);
    sys_sleep(100); // Sleep for 100 milliseconds to ensure time passes
    bool result2 = sys_time_get_utc(&time2);

    test_assert(result1 == true);
    test_assert(result2 == true);

    // Second call should be same or later than first call
    test_assert(time2.seconds >= time1.seconds);

    if (time2.seconds == time1.seconds) {
      // Same second, nanoseconds should be >=
      test_assert(time2.nanoseconds >= time1.nanoseconds);
    }

    // Time difference should be very small (less than 1 second for this test)
    int64_t diff_seconds = time2.seconds - time1.seconds;
    test_assert(diff_seconds < 1);

    sys_printf("Time consistency test passed: diff=%" PRId64 " seconds\n",
               diff_seconds);
  } while (0);

  // Test 4: Time progresses (multiple samples)
  do {
    sys_time_t times[3];

    // Get three time samples with small delays
    bool result1 = sys_time_get_utc(&times[0]);
    sys_sleep(100); // Sleep for 100 milliseconds to ensure time passes
    bool result2 = sys_time_get_utc(&times[1]);
    sys_sleep(100); // Sleep for 100 milliseconds to ensure time passes
    bool result3 = sys_time_get_utc(&times[2]);

    test_assert(result1 == true);
    test_assert(result2 == true);
    test_assert(result3 == true);

    // Time should progress monotonically
    test_assert(times[1].seconds >= times[0].seconds);
    test_assert(times[2].seconds >= times[1].seconds);

    // If same second, nanoseconds should progress
    if (times[1].seconds == times[0].seconds) {
      test_assert(times[1].nanoseconds >= times[0].nanoseconds);
    }
    if (times[2].seconds == times[1].seconds) {
      test_assert(times[2].nanoseconds >= times[1].nanoseconds);
    }

    sys_printf("Time progression test passed\n");
  } while (0);

  // Test 7: sys_time_get_time_utc functionality
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    uint8_t hours, minutes, seconds;
    bool time_result = sys_time_get_time_utc(&time, &hours, &minutes, &seconds);
    sys_printf("sys_time_get_time_utc: hours=%u, minutes=%u, seconds=%u\n",
               hours, minutes, seconds);
    test_assert(time_result == true);
    test_assert(hours < 24);
    test_assert(minutes < 60);
    test_assert(seconds < 60);

    sys_printf("Time: %02u:%02u:%02u (nanoseconds from time struct: %d)\n",
               hours, minutes, seconds, time.nanoseconds);
  } while (0);

  // Test 8: sys_time_get_time_utc with NULL parameters (should now succeed)
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    uint8_t hours, minutes, seconds;

    // Test NULL time (should still fail - time parameter is required)
    test_assert(sys_time_get_time_utc(NULL, &hours, &minutes, &seconds) ==
                false);

    // Test selective extraction with NULL parameters (should succeed)
    test_assert(sys_time_get_time_utc(&time, NULL, &minutes, &seconds) ==
                true); // Skip hours
    test_assert(sys_time_get_time_utc(&time, &hours, NULL, &seconds) ==
                true); // Skip minutes
    test_assert(sys_time_get_time_utc(&time, &hours, &minutes, NULL) ==
                true); // Skip seconds

    // Test extracting only specific components
    uint8_t just_hours;
    test_assert(sys_time_get_time_utc(&time, &just_hours, NULL, NULL) == true);
    test_assert(just_hours < 24);

    sys_printf("sys_time_get_time_utc selective extraction tests passed\n");
  } while (0);

  // Test 9: sys_time_get_date_utc functionality
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    uint16_t year;
    uint8_t month, day, weekday;
    bool date_result =
        sys_time_get_date_utc(&time, &year, &month, &day, &weekday);

    test_assert(date_result == true);
    test_assert(year >= 1970); // Should be 1970 or later
    test_assert(month >= 1 && month <= 12);
    test_assert(day >= 1 && day <= 31);
    test_assert(weekday < 7); // 0-6 for Sunday-Saturday

    sys_printf("Date: %04u-%02u-%02u (weekday: %u)\n", year, month, day,
               weekday);
  } while (0);

  // Test 10: sys_time_get_date_utc with NULL parameters (should now succeed)
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    uint16_t year;
    uint8_t month, day, weekday;

    // Test NULL time (should still fail - time parameter is required)
    test_assert(sys_time_get_date_utc(NULL, &year, &month, &day, &weekday) ==
                false);

    // Test selective extraction with NULL parameters (should succeed)
    test_assert(sys_time_get_date_utc(&time, NULL, &month, &day, &weekday) ==
                true); // Skip year
    test_assert(sys_time_get_date_utc(&time, &year, NULL, &day, &weekday) ==
                true); // Skip month
    test_assert(sys_time_get_date_utc(&time, &year, &month, NULL, &weekday) ==
                true); // Skip day
    test_assert(sys_time_get_date_utc(&time, &year, &month, &day, NULL) ==
                true); // Skip weekday

    // Test extracting only specific components
    uint16_t just_year;
    test_assert(sys_time_get_date_utc(&time, &just_year, NULL, NULL, NULL) ==
                true);
    uint16_t current_year =
        1970 +
        (sys_time_get_utc(&time) ? time.seconds / (365 * 24 * 60 * 60) : 0);
    test_assert(just_year >= current_year); // Should be current year or later

    sys_printf("sys_time_get_date_utc selective extraction tests passed\n");
  } while (0);

  // Test 11: sys_time_set_time_utc functionality
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    // Get original date for comparison
    uint16_t orig_year;
    uint8_t orig_month, orig_day, orig_weekday;
    test_assert(sys_time_get_date_utc(&time, &orig_year, &orig_month, &orig_day,
                                      &orig_weekday) == true);

    // Set new time: 14:30:45 (nanoseconds set separately)
    bool set_result = sys_time_set_time_utc(&time, 14, 30, 45);
    test_assert(set_result == true);

    // Set nanoseconds directly on the structure
    time.nanoseconds = 123456789;

    // Verify the time was set correctly
    uint8_t hours, minutes, seconds;
    test_assert(sys_time_get_time_utc(&time, &hours, &minutes, &seconds) ==
                true);
    test_assert(hours == 14);
    test_assert(minutes == 30);
    test_assert(seconds == 45);
    // Note: nanoseconds are preserved in the time struct directly
    test_assert(time.nanoseconds == 123456789);

    // Verify date was preserved
    uint16_t new_year;
    uint8_t new_month, new_day, new_weekday;
    test_assert(sys_time_get_date_utc(&time, &new_year, &new_month, &new_day,
                                      &new_weekday) == true);
    test_assert(new_year == orig_year);
    test_assert(new_month == orig_month);
    test_assert(new_day == orig_day);

    sys_printf("sys_time_set_time_utc: Set time to 14:30:45.123456789, date "
               "preserved\n");
  } while (0);

  // Test 12: sys_time_set_time_utc parameter validation
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    // Test NULL pointer
    test_assert(sys_time_set_time_utc(NULL, 12, 0, 0) == false);

    // Test invalid parameters
    test_assert(sys_time_set_time_utc(&time, 24, 0, 0) == false); // hours >= 24
    test_assert(sys_time_set_time_utc(&time, 0, 60, 0) ==
                false); // minutes >= 60
    test_assert(sys_time_set_time_utc(&time, 0, 0, 60) ==
                false); // seconds >= 60
    // Note: nanoseconds validation must be done manually as it's not part of
    // API

    sys_printf("sys_time_set_time_utc parameter validation tests passed\n");
  } while (0);

  // Test 13: sys_time_set_date_utc functionality
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    // Get original time for comparison
    uint8_t orig_hours, orig_minutes, orig_seconds;
    test_assert(sys_time_get_time_utc(&time, &orig_hours, &orig_minutes,
                                      &orig_seconds) == true);
    uint32_t orig_nanoseconds = time.nanoseconds;

    // Set new date: December 25, 2025
    bool set_result = sys_time_set_date_utc(&time, 2025, 12, 25);
    test_assert(set_result == true);

    // Verify the date was set correctly
    uint16_t year;
    uint8_t month, day, weekday;
    test_assert(sys_time_get_date_utc(&time, &year, &month, &day, &weekday) ==
                true);
    test_assert(year == 2025);
    test_assert(month == 12);
    test_assert(day == 25);
    test_assert(weekday == 4); // December 25, 2025 is a Thursday

    // Verify time was preserved
    uint8_t new_hours, new_minutes, new_seconds;
    test_assert(sys_time_get_time_utc(&time, &new_hours, &new_minutes,
                                      &new_seconds) == true);
    uint32_t new_nanoseconds = time.nanoseconds;
    test_assert(new_hours == orig_hours);
    test_assert(new_minutes == orig_minutes);
    test_assert(new_seconds == orig_seconds);
    test_assert(new_nanoseconds == orig_nanoseconds);

    sys_printf("sys_time_set_date_utc: Set date to 2025-12-25 (Thu), time "
               "preserved\n");
  } while (0);

  // Test 14: sys_time_set_date_utc parameter validation
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    // Test NULL pointer
    test_assert(sys_time_set_date_utc(NULL, 2025, 1, 1) == false);

    // Test invalid parameters
    test_assert(sys_time_set_date_utc(&time, 2025, 0, 1) == false); // month < 1
    test_assert(sys_time_set_date_utc(&time, 2025, 13, 1) ==
                false); // month > 12
    test_assert(sys_time_set_date_utc(&time, 2025, 1, 0) == false);  // day < 1
    test_assert(sys_time_set_date_utc(&time, 2025, 1, 32) == false); // day > 31

    // Test pre-epoch dates (should now work)
    test_assert(sys_time_set_date_utc(&time, 1969, 12, 25) ==
                true); // Dec 25, 1969
    test_assert(sys_time_set_date_utc(&time, 1900, 1, 1) ==
                true); // Jan 1, 1900

    // Test month-specific day limits
    test_assert(sys_time_set_date_utc(&time, 2025, 2, 30) ==
                false); // Feb 30 (invalid)
    test_assert(sys_time_set_date_utc(&time, 2025, 4, 31) ==
                false); // Apr 31 (invalid)
    test_assert(sys_time_set_date_utc(&time, 2025, 6, 31) ==
                false); // Jun 31 (invalid)
    test_assert(sys_time_set_date_utc(&time, 2025, 9, 31) ==
                false); // Sep 31 (invalid)
    test_assert(sys_time_set_date_utc(&time, 2025, 11, 31) ==
                false); // Nov 31 (invalid)

    // Test leap year handling
    test_assert(sys_time_set_date_utc(&time, 2025, 2, 29) ==
                false); // Feb 29, 2025 (not leap year)
    test_assert(sys_time_set_date_utc(&time, 2024, 2, 29) ==
                true); // Feb 29, 2024 (leap year)
    test_assert(sys_time_set_date_utc(&time, 2000, 2, 29) ==
                true); // Feb 29, 2000 (leap year, divisible by 400)
    test_assert(
        sys_time_set_date_utc(&time, 1900, 2, 29) ==
        false); // Feb 29, 1900 (not leap year, divisible by 100 but not 400)

    sys_printf("sys_time_set_date_utc parameter validation tests passed\n");
  } while (0);

  // Test 15: sys_time_compare_ns functionality
  do {
    // Test with two specific times
    sys_time_t time1 = {1000,
                        500000000}; // 1000 seconds, 0.5 seconds in nanoseconds
    sys_time_t time2 = {1001,
                        700000000}; // 1001 seconds, 0.7 seconds in nanoseconds

    // time2 is 1.2 seconds later than time1
    // Expected difference: 1.2 seconds = 1,200,000,000 nanoseconds
    int64_t diff = sys_time_compare_ns(&time1, &time2);
    test_assert(diff == 1200000000LL);

    // Reverse comparison should be negative
    int64_t reverse_diff = sys_time_compare_ns(&time2, &time1);
    test_assert(reverse_diff == -1200000000LL);

    sys_printf("sys_time_compare_ns: Basic comparison tests passed\n");
  } while (0);

  // Test 16: sys_time_compare_ns with same times
  do {
    sys_time_t time1 = {1000, 123456789};
    sys_time_t time2 = {1000, 123456789};

    int64_t diff = sys_time_compare_ns(&time1, &time2);
    test_assert(diff == 0);

    sys_printf("sys_time_compare_ns: Same time comparison passed\n");
  } while (0);

  // Test 17: sys_time_compare_ns with NULL start (epoch)
  do {
    sys_time_t time = {1, 0}; // 1 second after epoch

    int64_t diff = sys_time_compare_ns(NULL, &time);
    test_assert(diff == 1000000000LL); // 1 second = 1 billion nanoseconds

    sys_printf("sys_time_compare_ns: NULL start (epoch) test passed\n");
  } while (0);

  // Test 18: sys_time_compare_ns with NULL end (current time)
  do {
    sys_time_t current_time;
    test_assert(sys_time_get_utc(&current_time) == true);

    sys_time_t past_time = {current_time.seconds - 1, current_time.nanoseconds};

    int64_t diff = sys_time_compare_ns(&past_time, NULL);

    // Should be approximately 1 second (1 billion nanoseconds)
    // Allow some tolerance for execution time
    test_assert(diff >= 900000000LL && diff <= 1100000000LL);

    sys_printf("sys_time_compare_ns: NULL end (current time) test passed\n");
  } while (0);

  // Test 20: sys_time_compare_ns with nanosecond precision
  do {
    sys_time_t time1 = {1000, 123456789};
    sys_time_t time2 = {1000, 123456790}; // 1 nanosecond later

    int64_t diff = sys_time_compare_ns(&time1, &time2);
    test_assert(diff == 1);

    // Test nanosecond underflow
    sys_time_t time3 = {1001, 100000000};
    sys_time_t time4 = {1000, 900000000}; // earlier time but more nanoseconds

    int64_t diff2 = sys_time_compare_ns(&time4, &time3);
    test_assert(diff2 == 200000000LL); // 0.2 seconds = 200 million nanoseconds

    sys_printf("sys_time_compare_ns: Nanosecond precision tests passed\n");
  } while (0);

  sys_printf("All sys_02 time tests passed!\n");
  
  // Test 21: gmtime functionality with known timestamps
  do {
    // Test Unix epoch: January 1, 1970, 00:00:00 UTC (Thursday)
    time_t epoch = 0;
    struct tm *tm_epoch = gmtime(&epoch);
    test_assert(tm_epoch != NULL);
    test_assert(tm_epoch->tm_year == 70);  // 1970 - 1900
    test_assert(tm_epoch->tm_mon == 0);    // January (0-based)
    test_assert(tm_epoch->tm_mday == 1);   // 1st day
    test_assert(tm_epoch->tm_hour == 0);   // Midnight
    test_assert(tm_epoch->tm_min == 0);
    test_assert(tm_epoch->tm_sec == 0);
    test_assert(tm_epoch->tm_wday == 4);   // Thursday
    test_assert(tm_epoch->tm_yday == 0);   // First day of year
    test_assert(tm_epoch->tm_isdst == 0);  // No DST for UTC
    
    sys_printf("gmtime: Unix epoch test passed\n");
  } while (0);

  // Test 22: gmtime with various known timestamps
  do {
    // Test September 9, 2001, 01:46:40 UTC (Sunday)
    time_t timestamp_2001 = 1000000000;
    struct tm *tm_2001 = gmtime(&timestamp_2001);
    test_assert(tm_2001 != NULL);
    test_assert(tm_2001->tm_year == 101); // 2001 - 1900
    test_assert(tm_2001->tm_mon == 8);    // September (0-based)
    test_assert(tm_2001->tm_mday == 9);   // 9th day
    test_assert(tm_2001->tm_hour == 1);   // 01:46:40
    test_assert(tm_2001->tm_min == 46);
    test_assert(tm_2001->tm_sec == 40);
    test_assert(tm_2001->tm_wday == 0);   // Sunday
    
    sys_printf("gmtime: September 9, 2001 test passed\n");
  } while (0);

  // Test 23: gmtime leap year handling
  do {
    // Test February 29, 2000 (leap year) - timestamp for 2000-02-29 00:00:00 UTC
    time_t leap_day = 951782400; // Feb 29, 2000
    struct tm *tm_leap = gmtime(&leap_day);
    test_assert(tm_leap != NULL);
    test_assert(tm_leap->tm_year == 100); // 2000 - 1900
    test_assert(tm_leap->tm_mon == 1);    // February (0-based)
    test_assert(tm_leap->tm_mday == 29);  // 29th day (leap day)
    
    sys_printf("gmtime: Leap year February 29, 2000 test passed\n");
  } while (0);

  // Test 24: gmtime with current test timestamp
  do {
    // Use our current test timestamp from sys_time_get_utc
    sys_time_t current_time;
    test_assert(sys_time_get_utc(&current_time) == true);
    
    time_t test_timestamp = (time_t)current_time.seconds;
    struct tm *tm_current = gmtime(&test_timestamp);
    test_assert(tm_current != NULL);
    
    // Verify basic ranges
    test_assert(tm_current->tm_year >= 70);  // After 1970
    test_assert(tm_current->tm_mon >= 0 && tm_current->tm_mon <= 11);
    test_assert(tm_current->tm_mday >= 1 && tm_current->tm_mday <= 31);
    test_assert(tm_current->tm_hour >= 0 && tm_current->tm_hour <= 23);
    test_assert(tm_current->tm_min >= 0 && tm_current->tm_min <= 59);
    test_assert(tm_current->tm_sec >= 0 && tm_current->tm_sec <= 59);
    test_assert(tm_current->tm_wday >= 0 && tm_current->tm_wday <= 6);
    test_assert(tm_current->tm_yday >= 0 && tm_current->tm_yday <= 365);
    test_assert(tm_current->tm_isdst == 0);
    
    sys_printf("gmtime: Current timestamp validation passed\n");
  } while (0);

  // Test 25: timegm round-trip consistency
  do {
    // Test that gmtime -> timegm gives us back the original timestamp
    time_t original_timestamps[] = {0, 1000000000, 1735689600}; // epoch, 2001, 2025
    
    for (int i = 0; i < 3; i++) {
      time_t original = original_timestamps[i];
      struct tm *tm_converted = gmtime(&original);
      test_assert(tm_converted != NULL);
      
      time_t round_trip = timegm(tm_converted);
      test_assert(round_trip == original);
      
      sys_printf("timegm: Round-trip test %d passed (timestamp %ld)\n", i+1, original);
    }
  } while (0);

  // Test 26: timegm with manually constructed tm structures
  do {
    // Test New Year 2000: January 1, 2000, 00:00:00 UTC
    struct tm tm_y2k = {0};
    tm_y2k.tm_year = 100;  // 2000 - 1900
    tm_y2k.tm_mon = 0;     // January
    tm_y2k.tm_mday = 1;    // 1st
    tm_y2k.tm_hour = 0;
    tm_y2k.tm_min = 0;
    tm_y2k.tm_sec = 0;
    
    time_t y2k_timestamp = timegm(&tm_y2k);
    test_assert(y2k_timestamp == 946684800); // Known Y2K timestamp
    
    sys_printf("timegm: Y2K timestamp test passed\n");
  } while (0);

  // Test 27: timegm leap year validation
  do {
    // Test leap day 2024: February 29, 2024
    struct tm tm_leap2024 = {0};
    tm_leap2024.tm_year = 124; // 2024 - 1900
    tm_leap2024.tm_mon = 1;    // February
    tm_leap2024.tm_mday = 29;  // 29th (leap day)
    tm_leap2024.tm_hour = 12;
    tm_leap2024.tm_min = 0;
    tm_leap2024.tm_sec = 0;
    
    time_t leap2024_timestamp = timegm(&tm_leap2024);
    test_assert(leap2024_timestamp > 0); // Should be valid
    
    // Verify round-trip
    struct tm *tm_back = gmtime(&leap2024_timestamp);
    test_assert(tm_back != NULL);
    test_assert(tm_back->tm_year == 124);
    test_assert(tm_back->tm_mon == 1);
    test_assert(tm_back->tm_mday == 29);
    test_assert(tm_back->tm_hour == 12);
    
    sys_printf("timegm: Leap day 2024 test passed\n");
  } while (0);

  // Test 28: gmtime/timegm consistency with sys_time functions
  do {
    // Compare gmtime results with our sys_time_get_date_utc and sys_time_get_time_utc
    sys_time_t sys_time;
    test_assert(sys_time_get_utc(&sys_time) == true);
    
    // Get time/date using our sys functions
    uint8_t sys_hours, sys_minutes, sys_seconds;
    uint16_t sys_year;
    uint8_t sys_month, sys_day, sys_weekday;
    
    test_assert(sys_time_get_time_utc(&sys_time, &sys_hours, &sys_minutes, &sys_seconds) == true);
    test_assert(sys_time_get_date_utc(&sys_time, &sys_year, &sys_month, &sys_day, &sys_weekday) == true);
    
    // Get same info using gmtime
    time_t timestamp = (time_t)sys_time.seconds;
    struct tm *tm_result = gmtime(&timestamp);
    test_assert(tm_result != NULL);
    
    // Compare results
    test_assert(sys_hours == (uint8_t)tm_result->tm_hour);
    test_assert(sys_minutes == (uint8_t)tm_result->tm_min);
    test_assert(sys_seconds == (uint8_t)tm_result->tm_sec);
    test_assert(sys_year == (uint16_t)(tm_result->tm_year + 1900));
    test_assert(sys_month == (uint8_t)(tm_result->tm_mon + 1));
    test_assert(sys_day == (uint8_t)tm_result->tm_mday);
    test_assert(sys_weekday == (uint8_t)tm_result->tm_wday);
    
    sys_printf("gmtime/timegm: Consistency with sys_time functions passed\n");
  } while (0);

  // Test 29: Edge case timestamps
  do {
    // Test some edge cases
    time_t edge_cases[] = {
      1,           // 1 second after epoch
      86399,       // Last second of first day (23:59:59)
      86400,       // First second of second day (00:00:00)
      31535999,    // Last second of 1970 (non-leap year)
      31536000     // First second of 1971
    };
    
    for (int i = 0; i < 5; i++) {
      struct tm *tm_edge = gmtime(&edge_cases[i]);
      test_assert(tm_edge != NULL);
      
      // Verify round-trip
      time_t round_trip = timegm(tm_edge);
      test_assert(round_trip == edge_cases[i]);
      
      sys_printf("gmtime/timegm: Edge case %d passed (timestamp %ld)\n", i+1, edge_cases[i]);
    }
  } while (0);

  // Test 30: Month boundary tests
  do {
    // Test various month boundaries to ensure proper month/day calculations
    struct tm test_dates[] = {
      {0, 0, 0, 31, 0, 70, 0, 0, 0},   // Jan 31, 1970
      {0, 0, 0, 1, 1, 70, 0, 0, 0},    // Feb 1, 1970
      {0, 0, 0, 28, 1, 70, 0, 0, 0},   // Feb 28, 1970 (non-leap)
      {0, 0, 0, 1, 2, 70, 0, 0, 0},    // Mar 1, 1970
      {0, 0, 0, 31, 11, 70, 0, 0, 0}   // Dec 31, 1970
    };
    
    for (int i = 0; i < 5; i++) {
      time_t timestamp = timegm(&test_dates[i]);
      test_assert(timestamp > 0);
      
      struct tm *tm_back = gmtime(&timestamp);
      test_assert(tm_back != NULL);
      test_assert(tm_back->tm_mday == test_dates[i].tm_mday);
      test_assert(tm_back->tm_mon == test_dates[i].tm_mon);
      test_assert(tm_back->tm_year == test_dates[i].tm_year);
      
      sys_printf("gmtime/timegm: Month boundary test %d passed\n", i+1);
    }
  } while (0);

  sys_printf("All sys_02 time tests including gmtime/timegm passed!\n");
  return 0;
}
