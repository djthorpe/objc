#include <runtime-sys/sys.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simulate the appendStringWithFormat logic
void test_append_format(char *existing_str, const char *format, ...) {
  va_list args;
  va_start(args, format);

  // Get the length of the formatted string (like NXString does)
  size_t format_length = sys_vsprintf(NULL, 0, format, args);
  va_end(args);

  printf("Format length calculated: %zu\n", format_length);

  // Restart va_list for actual formatting
  va_start(args, format);

  // Allocate buffer to simulate NXString's behavior
  size_t existing_len = strlen(existing_str);
  size_t total_len = existing_len + format_length + 1;
  char *buffer = malloc(total_len);

  // Copy existing string
  strcpy(buffer, existing_str);

  // Format the new part directly into the buffer
  size_t actual_len =
      sys_vsprintf(buffer + existing_len, format_length + 1, format, args);
  va_end(args);

  printf("Existing: '%s'\n", existing_str);
  printf("Format: '%s'\n", format);
  printf("Expected result: '%sABCD5'\n", existing_str);
  printf("Actual result: '%s'\n", buffer);
  printf("Format actual length: %zu\n", actual_len);

  free(buffer);
}

int main() {
  printf("Testing appendStringWithFormat logic:\n");

  // Test the exact scenario from the failing test
  test_append_format("ABCD", "%d", 5);

  return 0;
}
