#include <stdarg.h>
#include <sys/sys.h>

// Define the panic function
void sys_panicf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  sys_puts("PANIC:");
  sys_vprintf(fmt, args);
  va_end(args);
  sys_abort(); // Use abort to terminate the program
}
