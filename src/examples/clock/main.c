/**
 * @file main.c
 * @brief A clock example using system date and time functions.
 *
 * This example demonstrates how to read the system date and time,
 * format it, and print it to the console. It uses the sys_date_t structure
 * to represent the current date and time, and sys_date_get_now() to retrieve
 * the current date and time.
 */
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// CLOCK

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

/////////////////////////////////////////////////////////////////////
// GLOBALS

static int iteration_count = 0;
static const int MAX_ITERATIONS = 60;

/////////////////////////////////////////////////////////////////////
// TIMER CALLBACK

void timer_callback(sys_timer_t *timer) {
  sys_date_t date;
  if (sys_date_get_now(&date)) {
    sys_printf("Iteration %d: ", iteration_count + 1);
    printdate(&date);
  } else {
    sys_printf("Failed to get current date and time\n");
  }

  iteration_count++;

  if (iteration_count >= MAX_ITERATIONS) {
    sys_timer_finalize(timer);
    sys_waitgroup_done((sys_waitgroup_t *)timer->userdata);
  }
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();

  sys_printf("Clock example - running for %d iterations (1 per second)\n",
             MAX_ITERATIONS);

  // A waitgroup can be used to wait for the timer callback to complete
  sys_waitgroup_t wg = sys_waitgroup_init();

  // Set up a timer to update the clock every second
  sys_timer_t timer = sys_timer_init(1000, &wg, timer_callback);

  // Add one to the waitgroup to wait for the timer callback
  sys_waitgroup_add(&wg, 1);

  // Start the timer
  if (!sys_timer_start(&timer)) {
    sys_printf("Failed to start timer\n");
    return -1;
  }

  // Wait for the timer to finish
  sys_waitgroup_finalize(&wg);

  // Cleanup and exit
  sys_exit();
  return 0;
}
