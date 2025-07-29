#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

static bool _sys_date_extract(int64_t time_sec, uint8_t *hours,
                              uint8_t *minutes, uint8_t *seconds,
                              uint16_t *year, uint8_t *month, uint8_t *day,
                              uint8_t *weekday) {
  // Convert seconds to struct tm in UTC
  struct tm utc_tm;
  time_t timestamp = (time_t)time_sec;
  if (gmtime_r(&timestamp, &utc_tm) == NULL) {
    return false;
  }

  // Extract time components
  if (hours != NULL) {
    *hours = (uint8_t)utc_tm.tm_hour;
  }
  if (minutes != NULL) {
    *minutes = (uint8_t)utc_tm.tm_min;
  }
  if (seconds != NULL) {
    *seconds = (uint8_t)utc_tm.tm_sec;
  }

  // Extract date components
  if (year != NULL) {
    *year = (uint16_t)(utc_tm.tm_year + 1900); // tm_year is years since 1900
  }
  if (month != NULL) {
    *month = (uint8_t)(utc_tm.tm_mon + 1); // tm_mon is 0-11, we want 1-12
  }
  if (day != NULL) {
    *day = (uint8_t)utc_tm.tm_mday; // tm_mday is already 1-31
  }
  if (weekday != NULL) {
    *weekday = (uint8_t)utc_tm.tm_wday; // tm_wday is 0-6 (Sun=0)
  }

  // Success
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Extracts time components from a sys_date_t structure.
 */
bool sys_date_get_time_utc(sys_date_t *date, uint8_t *hours, uint8_t *minutes,
                           uint8_t *seconds) {
  sys_date_t now;
  if (date == NULL) {
    // If date is NULL, get current time
    if (!sys_date_get_now(&now)) {
      return false; // Failed to get current time
    }
    date = &now; // Use current time
  }

  return _sys_date_extract(date->seconds, hours, minutes, seconds, NULL, NULL,
                           NULL, NULL);
}

/**
 * @brief Extracts time components from a sys_date_t structure.
 */
bool sys_date_get_time_local(sys_date_t *date, uint8_t *hours, uint8_t *minutes,
                             uint8_t *seconds) {
  sys_date_t now;
  if (date == NULL) {
    // If date is NULL, get current time
    if (!sys_date_get_now(&now)) {
      return false; // Failed to get current time
    }
    date = &now; // Use current time
  }

  return _sys_date_extract(date->seconds + date->tzoffset, hours, minutes,
                           seconds, NULL, NULL, NULL, NULL);
}

/**
 * @brief Extracts date components from a sys_date_t structure.
 */
bool sys_date_get_date_utc(sys_date_t *date, uint16_t *year, uint8_t *month,
                           uint8_t *day, uint8_t *weekday) {
  sys_date_t now;
  if (date == NULL) {
    // If date is NULL, get current time
    if (!sys_date_get_now(&now)) {
      return false; // Failed to get current time
    }
    date = &now; // Use current time
  }

  return _sys_date_extract(date->seconds, NULL, NULL, NULL, year, month, day,
                           weekday);
}

/**
 * @brief Extracts date components from a sys_date_t structure.
 */
bool sys_date_get_date_local(sys_date_t *date, uint16_t *year, uint8_t *month,
                             uint8_t *day, uint8_t *weekday) {
  sys_date_t now;
  if (date == NULL) {
    // If date is NULL, get current time
    if (!sys_date_get_now(&now)) {
      return false; // Failed to get current time
    }
    date = &now; // Use current time
  }

  return _sys_date_extract(date->seconds + date->tzoffset, NULL, NULL, NULL,
                           year, month, day, weekday);
}

/**
 * @brief Sets time components in a sys_date_t structure, preserving the date
 * and timezone offset.
 */
bool sys_date_set_time_utc(sys_date_t *date, uint8_t hours, uint8_t minutes,
                           uint8_t seconds) {
  if (!date)
    return false;

  // Validate input parameters
  if (hours >= 24 || minutes >= 60 || seconds >= 60) {
    return false; // Invalid time components
  }

  // Extract current date components
  uint16_t year;
  uint8_t month, day;
  if (!_sys_date_extract(date->seconds, NULL, NULL, NULL, &year, &month, &day,
                         NULL)) {
    return false;
  }

  // Create a struct tm with the existing date and new time (UTC)
  struct tm new_tm = {0};
  new_tm.tm_year = year - 1900; // tm_year is years since 1900
  new_tm.tm_mon = month - 1;    // tm_mon is 0-11
  new_tm.tm_mday = day;         // tm_mday is 1-31
  new_tm.tm_hour = hours;
  new_tm.tm_min = minutes;
  new_tm.tm_sec = seconds;
  new_tm.tm_isdst = 0; // No DST for UTC

  // Convert to time_t using timegm (treats tm as UTC)
  time_t new_time = timegm(&new_tm);
  if (new_time == (time_t)-1) {
    // timegm failed
    return false;
  }

  // Store the UTC time directly
  date->seconds = (int64_t)new_time;

  // Keep existing nanoseconds and timezone offset
  return true;
}

/**
 * @brief Sets date components in a sys_date_t structure, preserving the time.
 */
bool sys_date_set_date_utc(sys_date_t *date, uint16_t year, uint8_t month,
                           uint8_t day) {
  if (!date)
    return false;

  // Validate input parameters
  if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
    return false; // Invalid date components
  }

  // Extract current time components
  uint8_t hours, minutes, seconds;
  if (!_sys_date_extract(date->seconds, &hours, &minutes, &seconds, NULL, NULL,
                         NULL, NULL)) {
    return false;
  }

  // Create a struct tm with the new date and existing time (UTC)
  struct tm new_tm = {0};
  new_tm.tm_year = year - 1900; // tm_year is years since 1900
  new_tm.tm_mon = month - 1;    // tm_mon is 0-11
  new_tm.tm_mday = day;         // tm_mday is 1-31
  new_tm.tm_hour = hours;
  new_tm.tm_min = minutes;
  new_tm.tm_sec = seconds;
  new_tm.tm_isdst = 0; // No DST for UTC

  // Convert to time_t using timegm (treats tm as UTC)
  time_t new_time = timegm(&new_tm);
  if (new_time == (time_t)-1) {
    // timegm failed
    return false;
  }

  // Store the UTC time directly
  date->seconds = (int64_t)new_time;

  // Keep existing nanoseconds
  return true;
}

/**
 * @brief Calculate the difference in nanoseconds between two sys_date_t
 * structures.
 */
int64_t sys_date_compare_ns(const sys_date_t *start, const sys_date_t *end) {
  sys_date_t now;
  if (!end)
    return 0;
  if (start == NULL) {
    if (!sys_date_get_now(&now)) {
      return 0;
    }
    start = &now; // Use current time
  }

  // Calculate difference in seconds, then convert to nanoseconds
  int64_t sec_diff = end->seconds - start->seconds;
  int64_t ns_diff = sec_diff * 1000000000LL;

  // Add the nanosecond component difference
  ns_diff += (int64_t)end->nanoseconds - (int64_t)start->nanoseconds;

  // Return the total difference in nanoseconds
  return ns_diff;
}
