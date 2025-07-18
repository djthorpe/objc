#include <objc/objc.h>
#include <stdarg.h>
#include <sys/sys.h>

void panicf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  sys_puts("PANIC: ");
  objc_vprintf(fmt, args);
  va_end(args);
  sys_puts("\n");
  sys_abort();
}
