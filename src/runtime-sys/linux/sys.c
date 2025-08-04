#include "../all/printf.h"
#include <runtime-sys/sys.h>

void sys_init(void) {
  // Initialize printf mutex for thread-safe printing
  _sys_printf_init();
  // Call sys_date_get_timestamp() to initialize the timestamp
  sys_date_get_timestamp();
}

void sys_exit(void) {
  // Finalize printf mutex and cleanup resources
  _sys_printf_finalize();
}
