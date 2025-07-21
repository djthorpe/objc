// Ensure POSIX time constants are available
#define _POSIX_C_SOURCE 199309L
#define _GNU_SOURCE // For timegm function

#include <sys/sys.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

bool sys_time_get_utc(sys_time_t *time) {
  if (time == NULL) {
    return false;
  }

  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    return false; // Return false if clock_gettime fails
  }

  time->seconds = (int64_t)ts.tv_sec;
  time->nanoseconds = (int32_t)ts.tv_nsec;
  return true;
}

bool sys_time_get_time_utc(sys_time_t *time, uint8_t *hours, uint8_t *minutes,
                           uint8_t *seconds, uint32_t *nanoseconds) {
  if (time == NULL || hours == NULL || minutes == NULL || seconds == NULL ||
      nanoseconds == NULL) {
    return false;
  }

  time_t timestamp = (time_t)time->seconds;
  struct tm *utc_tm = gmtime(&timestamp);
  if (utc_tm == NULL) {
    return false;
  }

  *hours = (uint8_t)utc_tm->tm_hour;
  *minutes = (uint8_t)utc_tm->tm_min;
  *seconds = (uint8_t)utc_tm->tm_sec;
  *nanoseconds = (uint32_t)time->nanoseconds;

  return true;
}

bool sys_time_get_date_utc(sys_time_t *time, uint16_t *year, uint8_t *month,
                           uint8_t *day, uint8_t *weekday) {
  if (time == NULL || year == NULL || month == NULL || day == NULL ||
      weekday == NULL) {
    return false;
  }

  time_t timestamp = (time_t)time->seconds;
  struct tm *utc_tm = gmtime(&timestamp);
  if (utc_tm == NULL) {
    return false;
  }

  *year = (uint16_t)(utc_tm->tm_year + 1900); // tm_year is years since 1900
  *month = (uint8_t)(utc_tm->tm_mon + 1);     // tm_mon is 0-11, we want 1-12
  *day = (uint8_t)utc_tm->tm_mday;            // tm_mday is already 1-31
  *weekday = (uint8_t)utc_tm->tm_wday;        // tm_wday is 0-6 (Sun=0)

  return true;
}

bool sys_time_set_time_utc(sys_time_t *time, uint8_t hours, uint8_t minutes,
                           uint8_t seconds, uint32_t nanoseconds) {
  if (time == NULL) {
    return false;
  }

  // Validate input parameters
  if (hours >= 24 || minutes >= 60 || seconds >= 60 ||
      nanoseconds >= 1000000000) {
    return false;
  }

  // Get current date components to preserve them
  time_t timestamp = (time_t)time->seconds;
  struct tm *utc_tm = gmtime(&timestamp);
  if (utc_tm == NULL) {
    return false;
  }

  // Modify only the time components
  utc_tm->tm_hour = hours;
  utc_tm->tm_min = minutes;
  utc_tm->tm_sec = seconds;

  // Convert back to timestamp
  time_t new_timestamp = timegm(utc_tm);
  if (new_timestamp == -1) {
    return false;
  }

  time->seconds = (int64_t)new_timestamp;
  time->nanoseconds = (int32_t)nanoseconds;

  return true;
}

bool sys_time_set_date_utc(sys_time_t *time, uint16_t year, uint8_t month,
                           uint8_t day) {
  if (time == NULL) {
    return false;
  }

  // Validate input parameters
  if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31) {
    return false;
  }

  // Check for month-specific day limits
  // Array representing the number of days in each month (1-based index).
  // Index 0 is unused, indices 1-12 correspond to months January to December.
  uint8_t days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Handle leap year for February
  if (month == 2) {
    // Leap year calculation: divisible by 4, but not by 100 unless also by 400
    bool is_leap_year =
        (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
    if (is_leap_year) {
      days_in_month[2] = 29;
    }
  }

  // Validate day against month limits
  if (day > days_in_month[month]) {
    return false;
  }

  // Get current time components to preserve them
  time_t timestamp = (time_t)time->seconds;
  struct tm *utc_tm = gmtime(&timestamp);
  if (utc_tm == NULL) {
    return false;
  }

  // Modify only the date components
  utc_tm->tm_year = year - 1900; // tm_year is years since 1900
  utc_tm->tm_mon = month - 1;    // tm_mon is 0-11, we have 1-12
  utc_tm->tm_mday = day;         // tm_mday is already 1-31

  // Convert back to timestamp
  time_t new_timestamp = timegm(utc_tm);
  if (new_timestamp == -1) {
    return false;
  }

  time->seconds = (int64_t)new_timestamp;
  // Preserve nanoseconds

  return true;
}

int64_t sys_time_compare_ns(const sys_time_t *start, const sys_time_t *end) {
  sys_time_t start_time = {0, 0}; // Default to epoch (0 seconds, 0 nanoseconds)
  sys_time_t end_time;

  // Handle NULL start parameter - treat as epoch
  if (start != NULL) {
    start_time = *start;
  }

  // Handle NULL end parameter - treat as current time
  if (end != NULL) {
    end_time = *end;
  } else {
    if (!sys_time_get_utc(&end_time)) {
      // If we can't get current time, return 0 (no difference)
      return 0;
    }
  }

  // Calculate difference in seconds, then convert to nanoseconds
  int64_t seconds_diff = end_time.seconds - start_time.seconds;
  int64_t nanoseconds_diff = end_time.nanoseconds - start_time.nanoseconds;

  // Convert seconds to nanoseconds and add the nanoseconds difference
  int64_t total_diff_ns = (seconds_diff * 1000000000LL) + nanoseconds_diff;

  return total_diff_ns;
}
