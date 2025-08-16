#include <runtime-sys/sys.h>

static const char *days_of_week[] = {"Sunday",    "Monday",   "Tuesday",
                                     "Wednesday", "Thursday", "Friday",
                                     "Saturday"};

static const char *months_of_year[] = {
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

/**
 * @brief Function to read the current date and time and print it.
 */
bool printdate(sys_date_t *date) {
  // Date
  uint16_t year;
  uint8_t month, day, weekday;
  if (!sys_date_get_date_local(date, &year, &month, &day, &weekday)) {
    sys_printf("Failed to get date components\n");
    return false;
  } else {
    sys_printf("%s %s %02u, %04u", days_of_week[weekday],
               months_of_year[month - 1], day, year);
  }

  sys_putch(' ');

  // Time
  uint8_t hours, minutes, seconds;
  if (!sys_date_get_time_local(date, &hours, &minutes, &seconds)) {
    sys_printf("Failed to get time components\n");
    return false;
  } else {
    sys_printf("%02u:%02u:%02u", hours, minutes, seconds);
  }

  // Timezone offset
  if (date->tzoffset != 0) {
    sys_printf(" (UTC%+2d:%02d)", date->tzoffset / 3600,
               (date->tzoffset % 3600) / 60);
  } else {
    sys_printf(" (UTC)");
  }

  sys_putch('\n');

  return true;
}
