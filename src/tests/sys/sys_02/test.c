#include <inttypes.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>
#include <time.h>

int test_sys_02(void) {
  // Test 1: Basic sys_date_get_now functionality
  do {
    sys_date_t date;
    bool result = sys_date_get_now(&date);

    test_assert(result == true);
    test_assert(date.seconds >= 0); // Should be zero or positive
    test_assert(date.nanoseconds >= 0 &&
                date.nanoseconds < 1000000000); // Valid nanoseconds range

    sys_printf("sys_date_get_now: seconds=%ld, nanoseconds=%ld, tzoffset=%ld\n",
               date.seconds, date.nanoseconds, date.tzoffset);
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
    bool result2 = sys_date_get_now(&date2);

    test_assert(result1 == true);
    test_assert(result2 == true);

    // Calculate the difference in milliseconds
    int64_t diff_seconds = date2.seconds - date1.seconds;
    int64_t diff_nanoseconds = date2.nanoseconds - date1.nanoseconds;
    int64_t total_diff_ms =
        (diff_seconds * 1000) + (diff_nanoseconds / 1000000);

    // Should be approximately 100ms difference, allow some variance
    test_assert(total_diff_ms >= 50 && total_diff_ms <= 200);

    sys_printf("Time difference: %ldms\n", total_diff_ms);
    sys_printf("Date consistency test passed\n");
  } while (0);

  // Test 4: Basic sys_date_get_time_utc functionality
  do {
    sys_date_t date;
    bool result = sys_date_get_now(&date);
    test_assert(result == true);

    uint8_t hours, minutes, seconds;
    bool time_result = sys_date_get_time_utc(&date, &hours, &minutes, &seconds);

    test_assert(time_result == true);
    test_assert(hours <= 23);
    test_assert(minutes <= 59);
    test_assert(seconds <= 59);

    sys_printf("UTC Time: %02d:%02d:%02d\n", hours, minutes, seconds);
  } while (0);

  // Test 5: sys_date_get_time_utc with NULL parameters
  do {
    sys_date_t date;
    sys_date_get_now(&date);

    bool result = sys_date_get_time_utc(&date, NULL, NULL, NULL);
    test_assert(result == true); // Should work with all NULL output parameters

    uint8_t hours;
    bool result2 = sys_date_get_time_utc(&date, &hours, NULL, NULL);
    test_assert(result2 == true);
    test_assert(hours <= 23);

    sys_printf("sys_date_get_time_utc with NULL parameters passed\n");
  } while (0);

  // Test 6: sys_date_get_time_utc with NULL date (should use current time)
  do {
    uint8_t hours, minutes, seconds;
    bool result = sys_date_get_time_utc(NULL, &hours, &minutes, &seconds);

    test_assert(result == true);
    test_assert(hours <= 23);
    test_assert(minutes <= 59);
    test_assert(seconds <= 59);

    sys_printf("sys_date_get_time_utc with NULL date passed\n");
  } while (0);

  // Test 7: Basic sys_date_get_date_utc functionality
  do {
    sys_date_t date;
    bool result = sys_date_get_now(&date);
    test_assert(result == true);

    uint16_t year;
    uint8_t month, day, weekday;
    bool date_result =
        sys_date_get_date_utc(&date, &year, &month, &day, &weekday);

    test_assert(date_result == true);
    test_assert(year >= 1970); // Should be after 1970
    test_assert(month >= 1 && month <= 12);
    test_assert(day >= 1 && day <= 31);
    test_assert(weekday <= 6);

    sys_printf("UTC Date: %04d-%02d-%02d (weekday %d)\n", year, month, day,
               weekday);
  } while (0);

  // Test 8: sys_date_get_date_utc with NULL parameters
  do {
    sys_date_t date;
    sys_date_get_now(&date);

    bool result = sys_date_get_date_utc(&date, NULL, NULL, NULL, NULL);
    test_assert(result == true); // Should work with all NULL output parameters

    uint16_t year;
    bool result2 = sys_date_get_date_utc(&date, &year, NULL, NULL, NULL);
    test_assert(result2 == true);
    test_assert(year >= 1970);

    sys_printf("sys_date_get_date_utc with NULL parameters passed\n");
  } while (0);

  // Test 9: sys_date_get_date_utc with NULL date (should use current time)
  do {
    uint16_t year;
    uint8_t month, day, weekday;
    bool result = sys_date_get_date_utc(NULL, &year, &month, &day, &weekday);

    test_assert(result == true);
    test_assert(year >= 1970);
    test_assert(month >= 1 && month <= 12);
    test_assert(day >= 1 && day <= 31);
    test_assert(weekday <= 6);

    sys_printf("sys_date_get_date_utc with NULL date passed\n");
  } while (0);

  // Test 10: Local time functions
  do {
    sys_date_t date;
    bool result = sys_date_get_now(&date);
    test_assert(result == true);

    uint8_t utc_hours, utc_minutes, utc_seconds;
    uint8_t local_hours, local_minutes, local_seconds;

    bool utc_result =
        sys_date_get_time_utc(&date, &utc_hours, &utc_minutes, &utc_seconds);
    bool local_result = sys_date_get_time_local(&date, &local_hours,
                                                &local_minutes, &local_seconds);

    test_assert(utc_result == true);
    test_assert(local_result == true);

    sys_printf("UTC Time: %02d:%02d:%02d\n", utc_hours, utc_minutes,
               utc_seconds);
    sys_printf("Local Time: %02d:%02d:%02d (offset: %ld seconds)\n",
               local_hours, local_minutes, local_seconds, date.tzoffset);
  } while (0);

  // Test 11: Local date functions
  do {
    sys_date_t date;
    bool result = sys_date_get_now(&date);
    test_assert(result == true);

    uint16_t utc_year, local_year;
    uint8_t utc_month, utc_day, utc_weekday;
    uint8_t local_month, local_day, local_weekday;

    bool utc_result = sys_date_get_date_utc(&date, &utc_year, &utc_month,
                                            &utc_day, &utc_weekday);
    bool local_result = sys_date_get_date_local(
        &date, &local_year, &local_month, &local_day, &local_weekday);

    test_assert(utc_result == true);
    test_assert(local_result == true);

    sys_printf("UTC Date: %04d-%02d-%02d (weekday %d)\n", utc_year, utc_month,
               utc_day, utc_weekday);
    sys_printf("Local Date: %04d-%02d-%02d (weekday %d)\n", local_year,
               local_month, local_day, local_weekday);
  } while (0);

  // Test 12: sys_date_set_time_utc functionality
  do {
    sys_date_t date;
    bool result = sys_date_get_now(&date);
    test_assert(result == true);

    // Set a specific time
    bool set_result = sys_date_set_time_utc(&date, 15, 30, 45);
    test_assert(set_result == true);

    // Verify the time was set correctly
    uint8_t hours, minutes, seconds;
    bool get_result = sys_date_get_time_utc(&date, &hours, &minutes, &seconds);
    test_assert(get_result == true);
    test_assert(hours == 15);
    test_assert(minutes == 30);
    test_assert(seconds == 45);

    sys_printf("sys_date_set_time_utc: Set time to 15:30:45 successfully\n");
  } while (0);

  // Test 13: sys_date_set_time_utc with invalid parameters
  do {
    sys_date_t date;
    sys_date_get_now(&date);

    bool result1 = sys_date_set_time_utc(&date, 25, 30, 45); // Invalid hour
    test_assert(result1 == false);

    bool result2 = sys_date_set_time_utc(&date, 15, 60, 45); // Invalid minute
    test_assert(result2 == false);

    bool result3 = sys_date_set_time_utc(&date, 15, 30, 60); // Invalid second
    test_assert(result3 == false);

    bool result4 = sys_date_set_time_utc(NULL, 15, 30, 45); // NULL pointer
    test_assert(result4 == false);

    sys_printf("sys_date_set_time_utc: Invalid parameter tests passed\n");
  } while (0);

  // Test 14: sys_date_set_date_utc functionality
  do {
    sys_date_t date;
    bool result = sys_date_get_now(&date);
    test_assert(result == true);

    // Set a specific date (Christmas 2023)
    bool set_result = sys_date_set_date_utc(&date, 2023, 12, 25);
    test_assert(set_result == true);

    // Verify the date was set correctly
    uint16_t year;
    uint8_t month, day, weekday;
    bool get_result =
        sys_date_get_date_utc(&date, &year, &month, &day, &weekday);
    test_assert(get_result == true);
    test_assert(year == 2023);
    test_assert(month == 12);
    test_assert(day == 25);
    test_assert(weekday == 1); // Christmas 2023 was a Monday

    sys_printf("sys_date_set_date_utc: Set date to 2023-12-25 successfully\n");
  } while (0);

  // Test 15: sys_date_set_date_utc with invalid parameters
  do {
    sys_date_t date;
    sys_date_get_now(&date);

    bool result1 = sys_date_set_date_utc(&date, 1899, 12, 25); // Before 1900
    test_assert(result1 == false);

    bool result2 = sys_date_set_date_utc(&date, 2023, 13, 25); // Invalid month
    test_assert(result2 == false);

    bool result3 = sys_date_set_date_utc(&date, 2023, 12, 32); // Invalid day
    test_assert(result3 == false);

    bool result4 = sys_date_set_date_utc(NULL, 2023, 12, 25); // NULL pointer
    test_assert(result4 == false);

    sys_printf("sys_date_set_date_utc: Invalid parameter tests passed\n");
  } while (0);

  // Test 16: Date/time preservation tests
  do {
    sys_date_t date;
    sys_date_get_now(&date);

    // Get original time
    uint8_t orig_hours, orig_minutes, orig_seconds;
    sys_date_get_time_utc(&date, &orig_hours, &orig_minutes, &orig_seconds);

    // Set a new date
    sys_date_set_date_utc(&date, 2020, 6, 15);

    // Verify time is preserved
    uint8_t new_hours, new_minutes, new_seconds;
    sys_date_get_time_utc(&date, &new_hours, &new_minutes, &new_seconds);

    test_assert(new_hours == orig_hours);
    test_assert(new_minutes == orig_minutes);
    test_assert(new_seconds == orig_seconds);

    sys_printf("Date change preserves time correctly\n");
  } while (0);

  // Test 17: Date/time preservation tests (reverse)
  do {
    sys_date_t date;
    sys_date_get_now(&date);

    // Get original date
    uint16_t orig_year;
    uint8_t orig_month, orig_day;
    sys_date_get_date_utc(&date, &orig_year, &orig_month, &orig_day, NULL);

    // Set a new time
    sys_date_set_time_utc(&date, 10, 20, 30);

    // Verify date is preserved
    uint16_t new_year;
    uint8_t new_month, new_day;
    sys_date_get_date_utc(&date, &new_year, &new_month, &new_day, NULL);

    test_assert(new_year == orig_year);
    test_assert(new_month == orig_month);
    test_assert(new_day == orig_day);

    sys_printf("Time change preserves date correctly\n");
  } while (0);

  // Test 18: sys_date_compare_ns basic functionality
  do {
    sys_date_t date1, date2;

    // Set up two different times
    sys_date_get_now(&date1);
    date2 = date1;
    date2.seconds += 5; // 5 seconds later

    int64_t diff = sys_date_compare_ns(&date1, &date2);
    test_assert(diff == 5000000000LL); // 5 seconds = 5 billion nanoseconds

    // Test reverse order
    int64_t diff_reverse = sys_date_compare_ns(&date2, &date1);
    test_assert(diff_reverse == -5000000000LL); // Should be negative

    sys_printf("sys_date_compare_ns: Basic comparison tests passed\n");
  } while (0);

  // Test 19: sys_date_compare_ns with NULL start (uses current time)
  do {
    sys_date_t end_date;
    sys_date_get_now(&end_date);
    end_date.seconds += 10; // 10 seconds in the future

    int64_t diff = sys_date_compare_ns(NULL, &end_date);
    test_assert(diff > 0); // Should be positive (end is in future)

    sys_printf("sys_date_compare_ns: NULL start (current time) test passed\n");
  } while (0);

  // Test 20: sys_date_compare_ns with nanosecond precision
  do {
    sys_date_t date1 = {1000, 123456789, 0};
    sys_date_t date2 = {1000, 123456790, 0}; // 1 nanosecond later

    int64_t diff = sys_date_compare_ns(&date1, &date2);
    test_assert(diff == 1);

    // Test nanosecond underflow
    sys_date_t date3 = {1001, 100000000, 0};
    sys_date_t date4 = {1000, 900000000,
                        0}; // earlier time but more nanoseconds

    int64_t diff2 = sys_date_compare_ns(&date4, &date3);
    test_assert(diff2 == 200000000LL); // 0.2 seconds = 200 million nanoseconds

    sys_printf("sys_date_compare_ns: Nanosecond precision tests passed\n");
  } while (0);

  sys_printf("All sys_02 date tests passed!\n");
  /*
  // Test 20.5: Basic timegm availability check
  do {
    sys_printf("Testing timegm availability...\n");

    // Create a simple tm structure for epoch
    struct tm test_tm = {0};
    test_tm.tm_year = 70; // 1970 - 1900
    test_tm.tm_mon = 0;   // January
    test_tm.tm_mday = 1;  // 1st
    test_tm.tm_hour = 0;
    test_tm.tm_min = 0;
    test_tm.tm_sec = 0;
    test_tm.tm_isdst = 0;

    time_t result = timegm(&test_tm);
    sys_printf("timegm({1970-01-01 00:00:00}) = %ld (expected 0)\n", result);

    if (result == -1) {
      sys_printf("ERROR: timegm returned -1, function may not be available!\n");
    }

    sys_printf("timegm availability check completed\n");
  } while (0);

  // Test 21: gmtime functionality with known timestamps
  do {
    // Test Unix epoch: January 1, 1970, 00:00:00 UTC (Thursday)
    time_t epoch = 0;
    struct tm *tm_epoch = gmtime(&epoch);
    test_assert(tm_epoch != NULL);

    // Debug output to see what we actually got
    sys_printf("gmtime debug for epoch 0:\n");
    sys_printf("  tm_year = %d (expected 70)\n", tm_epoch->tm_year);
    sys_printf("  tm_mon = %d (expected 0)\n", tm_epoch->tm_mon);
    sys_printf("  tm_mday = %d (expected 1)\n", tm_epoch->tm_mday);
    sys_printf("  tm_hour = %d (expected 0)\n", tm_epoch->tm_hour);
    sys_printf("  tm_min = %d (expected 0)\n", tm_epoch->tm_min);
    sys_printf("  tm_sec = %d (expected 0)\n", tm_epoch->tm_sec);
    sys_printf("  tm_wday = %d (expected 4)\n", tm_epoch->tm_wday);
    sys_printf("  tm_yday = %d (expected 0)\n", tm_epoch->tm_yday);
    sys_printf("  tm_isdst = %d (expected 0)\n", tm_epoch->tm_isdst);

    test_assert(tm_epoch->tm_year == 70); // 1970 - 1900
    test_assert(tm_epoch->tm_mon == 0);   // January (0-based)
    test_assert(tm_epoch->tm_mday == 1);  // 1st day
    test_assert(tm_epoch->tm_hour == 0);  // Midnight
    test_assert(tm_epoch->tm_min == 0);
    test_assert(tm_epoch->tm_sec == 0);
    test_assert(tm_epoch->tm_wday == 4);  // Thursday
    test_assert(tm_epoch->tm_yday == 0);  // First day of year
    test_assert(tm_epoch->tm_isdst == 0); // No DST for UTC

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
    test_assert(tm_2001->tm_wday == 0); // Sunday

    sys_printf("gmtime: September 9, 2001 test passed\n");
  } while (0);

  // Test 23: gmtime leap year handling
  do {
    // Test February 29, 2000 (leap year) - timestamp for 2000-02-29 00:00:00
    // UTC
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
    // Use our current test timestamp from sys_date_get_now
    sys_date_t current_date;
    test_assert(sys_date_get_now(&current_date) == true);

    time_t test_timestamp = (time_t)current_date.seconds;
    struct tm *tm_current = gmtime(&test_timestamp);
    test_assert(tm_current != NULL);

    // Verify basic ranges
    test_assert(tm_current->tm_year >= 70); // After 1970
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
    time_t original_timestamps[] = {0, 1000000000,
                                    1735689600}; // epoch, 2001, 2025

    for (int i = 0; i < 3; i++) {
      time_t original = original_timestamps[i];
      struct tm *tm_converted = gmtime(&original);
      test_assert(tm_converted != NULL);

      // Debug output for gmtime conversion
      sys_printf("timegm debug for timestamp %ld:\n", original);
      sys_printf(
          "  gmtime result: %04d-%02d-%02d %02d:%02d:%02d (wday=%d, yday=%d)\n",
          tm_converted->tm_year + 1900, tm_converted->tm_mon + 1,
          tm_converted->tm_mday, tm_converted->tm_hour, tm_converted->tm_min,
          tm_converted->tm_sec, tm_converted->tm_wday, tm_converted->tm_yday);

      time_t round_trip = timegm(tm_converted);
      sys_printf("  timegm result: %ld (original: %ld, diff: %ld)\n",
                 round_trip, original, round_trip - original);

      // Let's see if timegm is working at all
      if (round_trip == -1) {
        sys_printf("  ERROR: timegm returned -1 (failure)\n");
      }

      test_assert(round_trip == original);

      sys_printf("timegm: Round-trip test %d passed (timestamp %ld)\n", i + 1,
                 original);
    }
  } while (0);

  // Test 26: timegm with manually constructed tm structures
  do {
    // Test New Year 2000: January 1, 2000, 00:00:00 UTC
    struct tm tm_y2k = {0};
    tm_y2k.tm_year = 100; // 2000 - 1900
    tm_y2k.tm_mon = 0;    // January
    tm_y2k.tm_mday = 1;   // 1st
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

  // Test 28: gmtime/timegm consistency with sys_date functions
  do {
    // Compare gmtime results with our sys_date_get_date_utc and
    // sys_date_get_time_utc
    sys_date_t sys_date;
    test_assert(sys_date_get_now(&sys_date) == true);

    // Get time/date using our sys functions
    uint8_t sys_hours, sys_minutes, sys_seconds;
    uint16_t sys_year;
    uint8_t sys_month, sys_day, sys_weekday;

    test_assert(sys_date_get_time_utc(&sys_date, &sys_hours, &sys_minutes,
                                      &sys_seconds) == true);
    test_assert(sys_date_get_date_utc(&sys_date, &sys_year, &sys_month,
                                      &sys_day, &sys_weekday) == true);

    // Get same info using gmtime
    time_t timestamp = (time_t)sys_date.seconds;
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

    sys_printf("gmtime/timegm: Consistency with sys_date functions passed\n");
  } while (0);

  // Test 29: Edge case timestamps
  do {
    // Test some edge cases
    time_t edge_cases[] = {
        1,        // 1 second after epoch
        86399,    // Last second of first day (23:59:59)
        86400,    // First second of second day (00:00:00)
        31535999, // Last second of 1970 (non-leap year)
        31536000  // First second of 1971
    };

    for (int i = 0; i < 5; i++) {
      struct tm *tm_edge = gmtime(&edge_cases[i]);
      test_assert(tm_edge != NULL);

      // Verify round-trip
      time_t round_trip = timegm(tm_edge);
      test_assert(round_trip == edge_cases[i]);

      sys_printf("gmtime/timegm: Edge case %d passed (timestamp %ld)\n", i + 1,
                 edge_cases[i]);
    }
  } while (0);

  // Test 30: Month boundary tests
  do {
    // Test various month boundaries to ensure proper month/day calculations
    struct tm test_dates[] = {
        {0, 0, 0, 31, 0, 70, 0, 0, 0, 0, NULL}, // Jan 31, 1970
        {0, 0, 0, 1, 1, 70, 0, 0, 0, 0, NULL},  // Feb 1, 1970
        {0, 0, 0, 28, 1, 70, 0, 0, 0, 0, NULL}, // Feb 28, 1970 (non-leap)
        {0, 0, 0, 1, 2, 70, 0, 0, 0, 0, NULL},  // Mar 1, 1970
        {0, 0, 0, 31, 11, 70, 0, 0, 0, 0, NULL} // Dec 31, 1970
    };

    for (int i = 0; i < 5; i++) {
      time_t timestamp = timegm(&test_dates[i]);
      test_assert(timestamp > 0);

      struct tm *tm_back = gmtime(&timestamp);
      test_assert(tm_back != NULL);
      test_assert(tm_back->tm_mday == test_dates[i].tm_mday);
      test_assert(tm_back->tm_mon == test_dates[i].tm_mon);
      test_assert(tm_back->tm_year == test_dates[i].tm_year);

      sys_printf("gmtime/timegm: Month boundary test %d passed\n", i + 1);
    }
  } while (0);

  sys_printf("All sys_02 date tests including gmtime/timegm passed!\n");
  */
  return 0;
}
