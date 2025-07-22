#include <pico/aon_timer.h>
#include <pico/stdlib.h>

/**
 * @brief Initializes the system on startup.
 */
void sys_init(void) {
  // Initialize standard I/O
  stdio_init_all();
  sleep_ms(1000);

  // Start the time from zero
  struct timespec ts = {0, 0};
  aon_timer_start(&ts);
}
