#include <stdlib.h>
#include <runtime-sys/sys.h>

void sys_abort(void) {
  sys_puts("\nHALT");
  abort();
}
