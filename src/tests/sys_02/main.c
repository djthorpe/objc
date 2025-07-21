#include <inttypes.h>
#include <sys/sys.h>
#include <tests/tests.h>

int main(void) {
  sys_printf("Running sys_02 time tests...\n");

  // Test 1: Basic sys_time_get_utc functionality
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);

    test_assert(result == true);
    test_assert(time.seconds > 0); // Should be positive (after Unix epoch)
    test_assert(time.nanoseconds >= 0 &&
                time.nanoseconds < 1000000000); // Valid nanoseconds range

    sys_printf("sys_time_get_utc: seconds=%" PRId64 ", nanoseconds=%" PRId32
               "\n",
               time.seconds, time.nanoseconds);
  } while (0);

  // Test 2: sys_time_get_utc with NULL parameter
  do {
    bool result = sys_time_get_utc(NULL);
    test_assert(result == false);

    sys_printf("sys_time_get_utc(NULL) correctly returned false\n");
  } while (0);

  // Test 3: Time consistency - two calls should be very close
  do {
    sys_time_t time1, time2;
    bool result1 = sys_time_get_utc(&time1);
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
    // Small busy loop to ensure some time passes
    for (volatile int i = 0; i < 1000; i++) { /* busy wait */
    }
    bool result2 = sys_time_get_utc(&times[1]);
    for (volatile int i = 0; i < 1000; i++) { /* busy wait */
    }
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
    uint32_t nanoseconds;
    bool time_result =
        sys_time_get_time_utc(&time, &hours, &minutes, &seconds, &nanoseconds);

    test_assert(time_result == true);
    test_assert(hours < 24);
    test_assert(minutes < 60);
    test_assert(seconds < 60);
    test_assert(nanoseconds < 1000000000);
    test_assert(nanoseconds == (uint32_t)time.nanoseconds);

    sys_printf("Time: %02u:%02u:%02u.%09u\n", hours, minutes, seconds,
               nanoseconds);
  } while (0);

  // Test 8: sys_time_get_time_utc with NULL parameters
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    uint8_t hours, minutes, seconds;
    uint32_t nanoseconds;

    // Test NULL time
    test_assert(sys_time_get_time_utc(NULL, &hours, &minutes, &seconds,
                                      &nanoseconds) == false);
    // Test NULL hours
    test_assert(sys_time_get_time_utc(&time, NULL, &minutes, &seconds,
                                      &nanoseconds) == false);
    // Test NULL minutes
    test_assert(sys_time_get_time_utc(&time, &hours, NULL, &seconds,
                                      &nanoseconds) == false);
    // Test NULL seconds
    test_assert(sys_time_get_time_utc(&time, &hours, &minutes, NULL,
                                      &nanoseconds) == false);
    // Test NULL nanoseconds
    test_assert(sys_time_get_time_utc(&time, &hours, &minutes, &seconds,
                                      NULL) == false);

    sys_printf("sys_time_get_time_utc NULL parameter tests passed\n");
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
    test_assert(year >= 2025); // Should be current year or later
    test_assert(month >= 1 && month <= 12);
    test_assert(day >= 1 && day <= 31);
    test_assert(weekday < 7); // 0-6 for Sunday-Saturday

    sys_printf("Date: %04u-%02u-%02u (weekday: %u)\n", year, month, day,
               weekday);
  } while (0);

  // Test 10: sys_time_get_date_utc with NULL parameters
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    uint16_t year;
    uint8_t month, day, weekday;

    // Test NULL time
    test_assert(sys_time_get_date_utc(NULL, &year, &month, &day, &weekday) ==
                false);
    // Test NULL year
    test_assert(sys_time_get_date_utc(&time, NULL, &month, &day, &weekday) ==
                false);
    // Test NULL month
    test_assert(sys_time_get_date_utc(&time, &year, NULL, &day, &weekday) ==
                false);
    // Test NULL day
    test_assert(sys_time_get_date_utc(&time, &year, &month, NULL, &weekday) ==
                false);
    // Test NULL weekday
    test_assert(sys_time_get_date_utc(&time, &year, &month, &day, NULL) ==
                false);

    sys_printf("sys_time_get_date_utc NULL parameter tests passed\n");
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

    // Set new time: 14:30:45.123456789
    bool set_result = sys_time_set_time_utc(&time, 14, 30, 45, 123456789);
    test_assert(set_result == true);

    // Verify the time was set correctly
    uint8_t hours, minutes, seconds;
    uint32_t nanoseconds;
    test_assert(sys_time_get_time_utc(&time, &hours, &minutes, &seconds,
                                      &nanoseconds) == true);
    test_assert(hours == 14);
    test_assert(minutes == 30);
    test_assert(seconds == 45);
    test_assert(nanoseconds == 123456789);

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
    test_assert(sys_time_set_time_utc(NULL, 12, 0, 0, 0) == false);

    // Test invalid parameters
    test_assert(sys_time_set_time_utc(&time, 24, 0, 0, 0) ==
                false); // hours >= 24
    test_assert(sys_time_set_time_utc(&time, 0, 60, 0, 0) ==
                false); // minutes >= 60
    test_assert(sys_time_set_time_utc(&time, 0, 0, 60, 0) ==
                false); // seconds >= 60
    test_assert(sys_time_set_time_utc(&time, 0, 0, 0, 1000000000) ==
                false); // nanoseconds >= 1e9

    sys_printf("sys_time_set_time_utc parameter validation tests passed\n");
  } while (0);

  // Test 13: sys_time_set_date_utc functionality
  do {
    sys_time_t time;
    bool result = sys_time_get_utc(&time);
    test_assert(result == true);

    // Get original time for comparison
    uint8_t orig_hours, orig_minutes, orig_seconds;
    uint32_t orig_nanoseconds;
    test_assert(sys_time_get_time_utc(&time, &orig_hours, &orig_minutes,
                                      &orig_seconds,
                                      &orig_nanoseconds) == true);

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
    uint32_t new_nanoseconds;
    test_assert(sys_time_get_time_utc(&time, &new_hours, &new_minutes,
                                      &new_seconds, &new_nanoseconds) == true);
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
    test_assert(sys_time_set_date_utc(&time, 1969, 1, 1) ==
                false); // year < 1970
    test_assert(sys_time_set_date_utc(&time, 2025, 0, 1) == false); // month < 1
    test_assert(sys_time_set_date_utc(&time, 2025, 13, 1) ==
                false); // month > 12
    test_assert(sys_time_set_date_utc(&time, 2025, 1, 0) == false);  // day < 1
    test_assert(sys_time_set_date_utc(&time, 2025, 1, 32) == false); // day > 31

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

  // Test 19: sys_time_compare_ns with both NULL (epoch to current time)
  do {
    int64_t diff = sys_time_compare_ns(NULL, NULL);

    // Should be positive (current time is after epoch)
    // Current time should be many billions of nanoseconds after epoch
    test_assert(diff >
                1000000000000000000LL); // > 1 billion seconds in nanoseconds

    sys_printf("sys_time_compare_ns: Both NULL test passed\n");
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
  return 0;
}
