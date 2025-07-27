#include "../all/printf.h"
#include <runtime-sys/sys.h>

void sys_init(void) {
  // Initialize printf mutex for thread-safe printing
  _sys_printf_init();
}

void sys_exit(void) {
  // Finalize printf mutex and cleanup resources
  _sys_printf_finalize();
}
