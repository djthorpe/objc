#include <pico/aon_timer.h>
#include <runtime-sys/sys.h>

#define MICROSECONDS_PER_SECOND 1000000LL

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/*
 * @brief Check if a year is a leap year
 */
static int is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/*
 * @brief Get days in a given month for a given year
 */
static int days_in_month(int month, int year) {
  static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 1 && is_leap_year(year)) { // February in leap year
    return 29;
  }
  return days[month];
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/*
 ** @brief Gets the current time based on the system's absolute time.
 */
bool sys_time_get_utc(sys_time_t *time) {
  if (time == NULL) {
    return false;
  }

  // Get the number of microseconds since boot
  struct timespec ts;
  if (!aon_timer_get_time(&ts)) {
    return false;
  }

  // Convert timespec to sys_time_t
  time->seconds = ts.tv_sec;
  time->nanoseconds = ts.tv_nsec;

  // Return success
  return true;
}

bool sys_time_get_time_utc(sys_time_t *time, uint8_t *hours, uint8_t *minutes,
                           uint8_t *seconds) {
  // TODO
  return false;
}

bool sys_time_get_date_utc(sys_time_t *time, uint16_t *year, uint8_t *month,
                           uint8_t *day, uint8_t *weekday) {
  // TODO
  return false;
}

bool sys_time_set_time_utc(sys_time_t *time, uint8_t hours, uint8_t minutes,
                           uint8_t seconds) {
  // TODO
  return false;
}

bool sys_time_set_date_utc(sys_time_t *time, uint16_t year, uint8_t month,
                           uint8_t day) {
  // TODO
  return false;
}