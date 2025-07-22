#include <stdio.h>

void sys_puts(const char *str) {
  if (str != NULL) {
    // Output the string to standard output
    fputs(str, stdout);
  }
  // Flush the output to ensure it appears immediately
  fflush(stdout);
}

void sys_putch(const char ch) {
  // Output the character to standard output
  fputc(ch, stdout);
}
