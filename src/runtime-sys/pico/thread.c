#include "hardware/platform_defs.h"
#include "pico/multicore.h"
#include <pico/stdlib.h>
#include <runtime-sys/sys.h>

/**
 * @brief Wrapper function to adapt sys_thread_func_t to Pico multicore function
 */
static void pico_thread_wrapper(void) {
  // Get the function pointer and argument from the multicore FIFO
  sys_thread_func_t func = (sys_thread_func_t)multicore_fifo_pop_blocking();
  void *arg = (void *)multicore_fifo_pop_blocking();

  if (func) {
    // Call the actual thread function
    func(arg);
  } else {
    sys_panicf("Pico thread wrapper: function pointer is NULL");
  }
}

/**
 * @brief Returns the number of CPU cores available on the host system.
 * @return The number of CPU cores available on the system. Returns 1 if the
 *         number of cores cannot be determined or if the system has only one
 *         core.
 *
 * This function queries the system to determine the total number of processing
 * cores available.
 */
uint8_t sys_thread_numcores() {
  // RP2040 has exactly NUM_CORES (2) cores
  return (uint8_t)NUM_CORES;
}

uint8_t sys_thread_core(void) {
  // On RP2040, use get_core_num() to get current core (0 or 1)
  return (uint8_t)get_core_num();
}

/**
 * @brief Pauses the execution of the current thread for a specified time.
 */
void sys_sleep(int32_t msec) { sleep_ms(msec); }

bool sys_thread_create(sys_thread_func_t func, void *arg) {
  // On Pico, we don't support generic thread creation
  // Users must explicitly use sys_thread_create_on_core() with core 1
  // This makes the limitations of the Pico platform explicit
  (void)func; // Suppress unused parameter warning
  (void)arg;  // Suppress unused parameter warning
  return false;
}

bool sys_thread_create_on_core(sys_thread_func_t func, void *arg,
                               uint8_t core) {
  if (func == NULL) {
    return false;
  }

  // On Pico, only core 0 can launch tasks on core 1
  // Core 1 should not be launching tasks on itself
  if (sys_thread_core() != 0) {
    return false;
  }

  // Check if the requested core is valid
  uint8_t num_cores = sys_thread_numcores();
  if (core >= num_cores) {
    return false;
  }

  switch (core) {
  case 0:
    // Core 0 is the main core, we cannot create threads on it
    return false;
  case 1:
    // Reset core 1 to prepare for new task
    multicore_reset_core1();

    // Launch the wrapper function on core 1
    multicore_launch_core1(pico_thread_wrapper);

    // Send the function pointer and argument through the FIFO
    // Order is important: function pointer first, then argument
    const uint32_t timeout_us = 1000000; // 1 second timeout
    if (!multicore_fifo_push_timeout_us((uintptr_t)func, timeout_us)) {
      sys_panicf("Failed to push function pointer to core 1 FIFO");
      return false;
    }
    if (!multicore_fifo_push_timeout_us((uintptr_t)arg, timeout_us)) {
      sys_panicf("Failed to push argument to core 1 FIFO");
      return false;
    }
    return true;
  default:
    // Invalid core number, should not happen
    return false;
  }
}
