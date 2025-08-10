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
  do {
    sys_puts("Sleeping...\n");
    sys_sleep(100);
  } while (0);

  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();

  // Run the task
  if (core0_task() == false) {
    sys_printf("Main: core 0 task failed\n");
  }

  // Cleanup and exit
  sys_exit();
  return 0;
}
