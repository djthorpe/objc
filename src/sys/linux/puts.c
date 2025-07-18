#include <stdio.h>

void sys_puts(const char *str) {
  if (str == NULL) {
    return; // Do nothing if the string is NULL
  }

  // Output the string to standard output
  fputs(str, stdout);

  // Flush the output to ensure it appears immediately
  fflush(stdout);
}
