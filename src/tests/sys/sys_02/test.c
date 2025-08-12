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
  return 0;
}
