#include <pico/time.h>
#include <runtime-sys/sys.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/*
 ** @brief Gets the current time based on the system's absolute time.
 */
bool sys_time_get_utc(sys_time_t *time) {
  if (time == NULL) {
    return false;
  }

  // Get the number of milliseconds since boot
  uint32_t ms = to_ms_since_boot(get_absolute_time());

  // Convert timespec to sys_time_t
  time->seconds = (int64_t)(ms / 1000); // Convert milliseconds to seconds
  time->nanoseconds =
      (int32_t)(ms % 1000) * 1000000; // Convert milliseconds to nanoseconds

  // Return success
  return true;
}

struct tm *gmtime(const time_t *clock) {
  static struct tm _tm;
  if (clock == NULL) {
    return NULL;
  }

  time_t timestamp = *clock;

  // Calculate time components
  _tm.tm_sec = (int)(timestamp % 60);
  _tm.tm_min = (int)((timestamp / 60) % 60);
  _tm.tm_hour = (int)((timestamp / 3600) % 24);

  // Calculate date components
  int64_t days_since_epoch = timestamp / (24 * 60 * 60);

  // Start from Unix epoch: January 1, 1970 (Thursday)
  // Calculate weekday (0=Sunday, 1=Monday, ..., 6=Saturday)
  _tm.tm_wday =
      (int)((4 + days_since_epoch) % 7); // Unix epoch was Thursday (4)
  if (_tm.tm_wday < 0)
    _tm.tm_wday += 7;

  // Calculate year (approximate)
  int year = 1970;
  int64_t days_remaining = days_since_epoch;

  // Account for leap years
  while (days_remaining >= 365) {
    bool is_leap = (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
    int days_in_year = is_leap ? 366 : 365;

    if (days_remaining >= days_in_year) {
      days_remaining -= days_in_year;
      year++;
    } else {
      break;
    }
  }

  _tm.tm_year = year - 1900;         // tm_year is years since 1900
  _tm.tm_yday = (int)days_remaining; // Day of year (0-365)

  // Calculate month and day
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Handle leap year for February
  bool is_leap_year =
      (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
  if (is_leap_year) {
    days_in_month[1] = 29;
  }

  int month = 0;
  while (month < 12 && days_remaining >= days_in_month[month]) {
    days_remaining -= days_in_month[month];
    month++;
  }

  _tm.tm_mon = month;                    // 0-11
  _tm.tm_mday = (int)days_remaining + 1; // 1-31

  // Daylight saving time flag (not applicable for UTC)
  _tm.tm_isdst = 0;

  return &_tm;
}
