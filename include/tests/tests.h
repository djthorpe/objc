#pragma once
#include <runtime-hw/hw.h>
#include <runtime-pix/pix.h>
#include <runtime-sys/sys.h>
#include <string.h>

#define test_assert(condition)                                                 \
  if (!(condition)) {                                                          \
    sys_panicf("Test failed: %s, file %s, line %d", #condition, __FILE__,      \
               __LINE__);                                                      \
  }

#define test_class_has_superclass(cls, supercls)                               \
  do {                                                                         \
    test_assert((cls) != NULL && (supercls) != NULL);                          \
    Class _test_cls = objc_lookupClass(cls);                                   \
    test_assert(_test_cls != Nil);                                             \
    Class _test_supercls = objc_lookupClass(supercls);                         \
    Class _test_actual_super = class_getSuperclass(_test_cls);                 \
    test_assert(_test_actual_super == _test_supercls);                         \
  } while (0)

#define test_stringsequal(str1, str2)                                          \
  do {                                                                         \
    test_assert((str1) != NULL && (str2) != NULL);                             \
    test_assert(strcmp([str1 cStr], [str2 cStr]) == 0);                        \
  } while (0)

#define test_cstrings_equal(str1, str2)                                        \
  do {                                                                         \
    test_assert((str1) != NULL && (str2) != NULL);                             \
    test_assert(strcmp(str1, str2) == 0);                                      \
  } while (0)

/**
 * @brief Main test runner function that initializes the system, runs a test,
 * and cleans up.
 * @param name The name of the test being run (for logging/debugging purposes)
 * @param test_func A pointer to the test function to execute.
 * @return The return value from the test function
 */
static inline int TestMain(const char *name, int (*test_func)(void)) {
  sys_init(); // Initialize the system
  sys_puts("\n");
  sys_puts("================================================================="
           "===============\n");
  sys_puts("START "); // Print the test name
  sys_puts(name);     // Print the test name
  sys_puts("\n");
  int returnValue = test_func(); // Run the test function
  sys_puts("END ");              // Print the test name
  sys_puts(name);                // Print the test name
  sys_puts("\n");
  sys_puts("================================================================="
           "===============\n");
  sys_exit();         // Exit the system
  return returnValue; // Return the test result
}

/**
 * @brief Main test runner function that initializes the system, runs a test,
 * and cleans up.
 * @param name The name of the test being run (for logging/debugging purposes)
 * @param test_func A pointer to the test function to execute.
 * @return The return value from the test function
 */
static inline int TestPixMain(const char *name, int (*test_func)(void)) {
  pix_init();                                  // Initialize the system
  int ReturnValue = TestMain(name, test_func); // Run the test function
  pix_exit();
  return ReturnValue; // Return the test result
}

/**
 * @brief Main test runner function that initializes the system, runs a test,
 * and cleans up.
 * @param name The name of the test being run (for logging/debugging purposes)
 * @param test_func A pointer to the test function to execute.
 * @return The return value from the test function
 */
static inline int TestHardwareMain(const char *name, int (*test_func)(void)) {
  hw_init();                                   // Initialize the system
  int ReturnValue = TestMain(name, test_func); // Run the test function
  hw_exit();
  return ReturnValue; // Return the test result
}
