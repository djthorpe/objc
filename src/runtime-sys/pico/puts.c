#include <pico/stdio.h>
#include <stddef.h>

void sys_puts(const char *str) {
  if (str != NULL) {
    // Output the string to standard output
    stdio_puts_raw(str);
  }
  // Flush the output to ensure it appears immediately
  stdio_flush();
}

void sys_putch(const char ch) {
  // Output the character to standard output
  stdio_putchar_raw(ch);
}
