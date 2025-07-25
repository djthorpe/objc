#include <runtime-sys/sys.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int main() {
  char buffer[100];

  // Test sys_sprintf with %d
  size_t len = sys_sprintf(buffer, sizeof(buffer), "%d", 5);
  printf("sys_sprintf result: '%s' (length=%zu)\n", buffer, len);

  // Test comparison
  if (strcmp(buffer, "5") == 0) {
    printf("✓ sys_sprintf with %%d works correctly\n");
  } else {
    printf("✗ sys_sprintf with %%d failed - expected '5', got '%s'\n", buffer);
  }

  // Test with a more complex format
  len = sys_sprintf(buffer, sizeof(buffer), "Number: %d", 42);
  printf("Complex format result: '%s' (length=%zu)\n", buffer, len);

  return 0;
}
