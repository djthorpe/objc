#include <pico/stdlib.h>
#include <runtime-sys/sys.h>

/**
 * @brief Initializes the system on startup.
 */
void sys_init(void) {
  // Initialize standard I/O
  stdio_init_all();
  sleep_ms(1000);
}

/**
 * @brief Cleans up the system on shutdown.
 */
void sys_exit(void) {
  while (true) {
    // Optionally, you can add a delay here to prevent busy-waiting
    sleep_ms(1000);
  }
}
