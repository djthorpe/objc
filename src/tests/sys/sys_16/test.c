#include <runtime-sys/sys.h>
#include <tests/tests.h>

// Forward declarations
int test_sys_env_serial(void);
int test_sys_env_name(void);
int test_sys_env_version(void);

int test_sys_16(void) {
  sys_puts("Running environment function tests...\n");

  test_sys_env_serial();
  test_sys_env_name();
  test_sys_env_version();

  sys_puts("All environment function tests passed!\n");
  return 0;
}

int test_sys_env_serial(void) {
  sys_puts("Testing sys_env_serial()...\n");

  // Test that sys_env_serial returns a non-NULL string
  const char *serial = sys_env_serial();
  test_assert(serial != NULL);

  // Test that the serial string is not empty
  test_assert(serial[0] != '\0');

  // Test that consecutive calls return the same value
  const char *serial2 = sys_env_serial();
  test_cstrings_equal(serial, serial2);

  sys_printf("Serial: %s\n", serial);

  sys_puts("sys_env_serial() tests passed\n");
  return 0;
}

int test_sys_env_name(void) {
  sys_puts("Testing sys_env_name()...\n");

  // Test that sys_env_name returns a non-NULL string
  const char *name = sys_env_name();
  test_assert(name != NULL);

  // Test that the name string is not empty
  test_assert(name[0] != '\0');

  // Test that consecutive calls return the same value
  const char *name2 = sys_env_name();
  test_cstrings_equal(name, name2);

  sys_printf("Name: %s\n", name);

  sys_puts("sys_env_name() tests passed\n");
  return 0;
}

int test_sys_env_version(void) {
  sys_puts("Testing sys_env_version()...\n");

  // Test that sys_env_version returns a non-NULL string
  const char *version = sys_env_version();
  test_assert(version != NULL);

  // Test that the version string is not empty
  test_assert(version[0] != '\0');

  // Test that consecutive calls return the same value
  const char *version2 = sys_env_version();
  test_cstrings_equal(version, version2);

  sys_printf("Version: %s\n", version);

  sys_puts("sys_env_version() tests passed\n");
  return 0;
}
