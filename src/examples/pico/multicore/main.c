/**
 * @file main.c
 * @brief Pico multicore programming example with waitgroups
 * @ingroup Examples
 *
 * This example demonstrates how to use the sys_thread_create_on_core() function
 * to run tasks on core 1 of the Raspberry Pi Pico (RP2040), and how to use
 * waitgroups for efficient synchronization between cores.
 *
 * The waitgroup is used to synchronize tasks running on core 1 with the main
 * thread running on core 0. It allows us to wait for tasks to complete without
 * busy-waiting or polling.
 *
 * The waitgroup is created with sys_waitgroup_init() and then just before a
 * task is started, we call sys_waitgroup_add() to indicate that we expect a
 * certain number of tasks to complete (usually one). After the task finishes,
 * it calls sys_waitgroup_done() to signal that it is complete.
 *
 * Meanwhile the main thread can call sys_waitgroup_finalize() to block until
 * all tasks have called done(). This allows us to wait for all tasks to finish
 * without busy-waiting.
 *
 * @example main.c
 * This is a complete example showing multicore programming with waitgroup
 * synchronization on the Pico platform.
 */
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// CORE 1 TASKS

/**
 * @brief Function to run on core 1
 *
 * This function is executed on core 1 of the RP2040 (or just simply "another
 * thread" on other platforms). It just "does something" on that core, and calls
 * "done" on the waitgroup when it's done. This would allow the main thread (or
 * "core") to reset the core and start the next task.
 */
void core1_task(void *arg) {
  sys_waitgroup_t *wg = (sys_waitgroup_t *)arg;
  sys_printf("Core %d: Starting task\n", sys_thread_core());

  // Do some work
  for (int i = 0; i < 10; i++) {
    // Print out the task progress
    sys_printf("  Core %d: Counter: %d\n", sys_thread_core(), i);

    // Sleep for some time less than 500ms
    sys_sleep(sys_random_uint32() % 500);
  }

  // Signal completion using waitgroup
  sys_waitgroup_done(wg);
  sys_printf("Core %d: Task completed!\n", sys_thread_core());
}

/////////////////////////////////////////////////////////////////////
// CORE 0 TASKS

/**
 * @brief Function to run on core 0 (or "main thread")
 *
 * This function is executed on core 0 of the RP2040 (or "main
 * thread" on other platforms). It co-ordinates tasks running on core 1, then
 * waits for those tasks to complete.
 */
bool core0_task() {
  // Initialize waitgroup
  sys_waitgroup_t wg = sys_waitgroup_init();

  // Indicate that we expect one task to complete
  sys_waitgroup_add(&wg, 1);
  sys_printf("Main: Starting task on core 1...\n");

  // Actually create the task on core 1. If this wasn't an RP2040, we would
  // just use sys_thread_create() instead.
  if (sys_thread_create_on_core(core1_task, &wg, 1) == false) {
    sys_printf("Main: Failed to launch task on core 1\n");
    return false;
  }

  // Wait for core 1 task to complete
  sys_printf("Main: Waiting for core 1 task to complete...\n");
  sys_waitgroup_finalize(&wg);
  sys_printf("Main: Core 1 task completed!\n");

  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();

  // Run the task
  if (core0_task() == false) {
    sys_printf("Main: Core 0 task failed\n");
  }

  // Cleanup and exit
  sys_exit();
  return 0;
}
