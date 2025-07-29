#include "../all/printf.h"
#include "date.h"
#include <pico/stdlib.h>
#include <runtime-sys/sys.h>

/**
 * @brief Initializes the system on startup.
 */
void sys_init(void) {
  stdio_init_all();
  sleep_ms(1000);
  _sys_printf_init();
  _sys_date_init();
}

/**
 * @brief Cleans up the system on shutdown.
 */
void sys_exit(void) {
  _sys_date_exit();
  _sys_printf_finalize();
  while (true) {
    sleep_ms(1000);
  }
}
