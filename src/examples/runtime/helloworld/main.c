/**
 * @file examples/runtime/helloworld/main.c
 *
 * This is the most minimal example of a Hello World program
 */
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// CORE 0 TASKS

/**
 * @brief Function to run on core 0
 *
 * This function outputs "Hello, World!" to the system console.
 */
bool core0_task() {

  int count = 0;
  do {
    sys_printf("Loop %d...\n", count);
    sys_sleep(1000);
  } while (count++ < 5);

  sys_printf("Ending...\n");
  sys_sleep(1000);
  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();

  // Print information
  sys_printf("NAME=%s\n", sys_env_name());
  sys_printf("SERIAL=%s\n", sys_env_serial());
  sys_printf("VERSION=%s\n", sys_env_version());

#ifdef PICO_CYW43_SUPPORTED
  sys_printf("WIFI SUPPORTED\n");
#else
  sys_printf("WIFI NOT SUPPORTED\n");
#endif

  // Run the task
  if (core0_task() == false) {
    sys_printf("Main: core 0 task failed\n");
  }

  // Cleanup and exit
  sys_exit();
  return 0;
}
