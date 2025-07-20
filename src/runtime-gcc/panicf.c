#include <stdarg.h>
#include <stdio.h>
#include <sys/sys.h>

// Define the panic function
void panicf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  sys_puts("PANIC:");
  vprintf(fmt, args);
  va_end(args);
  sys_abort(); // Use abort to terminate the program
}
