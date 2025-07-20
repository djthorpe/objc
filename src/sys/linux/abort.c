#include <stdlib.h>
#include <sys/sys.h>

void sys_abort(void) {
  sys_puts("\nHALT");
  abort();
}
