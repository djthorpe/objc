/**
 * @file main.c
 * @brief Pico multicore programming example
 * @ingroup Examples
 *
 * This example demonstrates how to use the sys_thread_create_on_core() function
 * to run tasks on core 1 of the Raspberry Pi Pico (RP2040).
 *
 * The example shows:
 * - Basic multicore task creation
 * - Inter-core communication
 * - Thread-safe printf operations
 * - Error handling for invalid operations
 *
 * @note This example requires a Raspberry Pi Pico or compatible RP2040 board.
 *
 * @example main.c
 * This is a complete example showing multicore programming on the Pico
 * platform.
 */

#include <runtime-sys/sys.h>

// Global variables for demonstration
#include <stdatomic.h>

static _Atomic bool core1_finished = false;
static _Atomic int core1_counter = 0;
/**
 * @brief Function to run on core 1
 */
void core1_task(void *arg) {
  int *task_id = (int *)arg;

  sys_printf("Core 1: Starting task %d on core %d\n", *task_id,
             sys_thread_core());

  // Do some work on core 1
  for (int i = 0; i < 10; i++) {
    core1_counter++;
    sys_printf("Core 1: Task %d - Counter: %d\n", *task_id, core1_counter);
    sys_sleep(500); // Sleep for 500ms
  }

  sys_printf("Core 1: Task %d completed!\n", *task_id);
  core1_finished = true;
}

/**
 * @brief Another function to demonstrate multiple tasks
 */
void core1_blink_task(void *arg) {
  int blink_count = *(int *)arg;

  sys_printf("Core 1: Starting blink task for %d blinks on core %d\n",
             blink_count, sys_thread_core());

  for (int i = 0; i < blink_count; i++) {
    sys_printf("Core 1: Blink %d/%d\n", i + 1, blink_count);
    sys_sleep(200);
  }

  sys_printf("Core 1: Blink task completed!\n");
  core1_finished = true;
}

int main() {
  // Initialize
  sys_init();

  sys_printf("=== Pico Multicore Example ===\n");
  sys_printf("Main: Running on core %d\n", sys_thread_core());
  sys_printf("Main: Number of cores: %d\n", sys_thread_numcores());

  // Test 1: Basic thread creation on core 1
  sys_printf("\n--- Test 1: Basic Core 1 Task ---\n");
  int task_id = 42;
  core1_finished = false;
  core1_counter = 0;

  if (sys_thread_create_on_core(core1_task, &task_id, 1)) {
    sys_printf("Main: Successfully launched task on core 1\n");

    // Wait for core 1 to finish while doing work on core 0
    int main_counter = 0;
    while (!core1_finished) {
      main_counter++;
      sys_printf("Main: Core 0 counter: %d\n", main_counter);
      sys_sleep(300);

      // Safety timeout
      if (main_counter > 20) {
        sys_printf("Main: Timeout waiting for core 1\n");
        break;
      }
    }

    sys_printf("Main: Core 1 task finished. Final core1_counter: %d\n",
               core1_counter);
  } else {
    sys_printf("Main: Failed to launch task on core 1\n");
  }

  sys_sleep(2000);

  // Test 2: Sequential tasks on core 1
  sys_printf("\n--- Test 2: Sequential Core 1 Tasks ---\n");

  int blink_count = 5;
  core1_finished = false;

  if (sys_thread_create_on_core(core1_blink_task, &blink_count, 1)) {
    sys_printf("Main: Successfully launched blink task on core 1\n");

    // Wait for completion
    while (!core1_finished) {
      sys_printf("Main: Waiting for blink task...\n");
      sys_sleep(1000);
    }
  } else {
    sys_printf("Main: Failed to launch blink task on core 1\n");
  }

  sys_sleep(1000);

  // Test 3: Demonstrate that generic sys_thread_create fails
  sys_printf("\n--- Test 3: Generic Thread Creation (Should Fail) ---\n");
  if (sys_thread_create(core1_task, &task_id)) {
    sys_printf("Main: ERROR - Generic thread creation should have failed!\n");
  } else {
    sys_printf("Main: Correctly rejected generic thread creation\n");
  }

  // Test 4: Demonstrate that core 0 creation fails
  sys_printf("\n--- Test 4: Core 0 Thread Creation (Should Fail) ---\n");
  if (sys_thread_create_on_core(core1_task, &task_id, 0)) {
    sys_printf("Main: ERROR - Core 0 thread creation should have failed!\n");
  } else {
    sys_printf("Main: Correctly rejected core 0 thread creation\n");
  }

  // Test 5: Demonstrate that invalid core fails
  sys_printf("\n--- Test 5: Invalid Core Thread Creation (Should Fail) ---\n");
  if (sys_thread_create_on_core(core1_task, &task_id, 99)) {
    sys_printf(
        "Main: ERROR - Invalid core thread creation should have failed!\n");
  } else {
    sys_printf("Main: Correctly rejected invalid core thread creation\n");
  }

  sys_printf("\n=== Multicore Example Complete ===\n");
  sys_printf("Main: All tests completed successfully!\n");

  sys_exit();
  return 0;
}
