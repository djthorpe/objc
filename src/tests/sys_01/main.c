#include <sys/sys.h>
#include <tests/tests.h>

int main(void) {
  // Output a message to the console
  do {
    size_t len = sys_printf("Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // Output a formatted message
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // ...with no buffer
  do {
    size_t len = sys_sprintf(NULL, 0, "Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // ...with % at the end, just print the %
  do {
    size_t len = sys_printf("Hello, World!%");
    test_assert(len == 14);
  } while (0);

  // ...with %% at the end
  do {
    size_t len = sys_printf("Hello, World!%%");
    test_assert(len == 14);
  } while (0);

  // ...with %% in the middle
  do {
    size_t len = sys_printf("Hello%% World!\n");
    test_assert(len == 14);
  } while (0);

  // ...with %s and zero-length c-string
  do {
    size_t len = sys_printf("Hello%s, World!\n", "");
    test_assert(len == 14);
  } while (0);

  // ...with %s and c-string
  do {
    size_t len = sys_printf("Hello, %s\n", "World!");
    test_assert(len == 14);
  } while (0);

  // ...with %s and NULL c-string
  do {
    size_t len = sys_printf("Hello%s World!\n", NULL);
    test_assert(len == 18);
  } while (0);

  // ...with missing argument
  do {
    size_t len = sys_printf("Hello%s World!\n");
    test_assert(len == 18);
  } while (0);
}
