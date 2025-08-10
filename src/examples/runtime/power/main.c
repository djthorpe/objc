/**
 * @file examples/runtime/power/main.c
 * @brief Power management example
 *
 * This example demonstrates how to use the hardware power management features
 * to measure the battery condition, power source, and reset the system.
 */
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// POWER CALLBACK

void power_callback(hw_power_t *power, hw_power_flag_t flags, uint32_t value) {
  (void)power;
  if (flags & HW_POWER_BATTERY) {
    sys_printf("CALLBACK: Power source is battery (estimated %u%%)\n", value);
  }
  if (flags & HW_POWER_USB) {
    sys_printf("CALLBACK: Power source is USB (estimated %u%%)\n", value);
  }
  if (flags & HW_POWER_UNKNOWN) {
    sys_printf("CALLBACK: Power source is unknown\n");
  }
  if (flags & HW_POWER_RESET) {
    sys_printf("CALLBACK: Power is about to force a reset after %u ms\n",
               value);
  }
}

/////////////////////////////////////////////////////////////////////
// CORE 0 TASKS

/**
 * @brief Function to run on core 0
 *
 * This function creates a queue, timer and watchdog, and processes events from
 * the queue.
 */
bool core0_task() {
  // Create the power management instance
  hw_power_t *power = hw_power_init(0xFF, 0xFF, power_callback);
  if (!hw_power_valid(power)) {
    sys_printf("Failed to initialize power management\n");
    return false;
  }

  // Create the watchdog instance
  hw_watchdog_t *watchdog = hw_watchdog_init();
  if (!hw_watchdog_valid(watchdog)) {
    hw_power_finalize(power);
    sys_printf("Failed to initialize watchdog\n");
    return false;
  }

  // Report if last reset was due to watchdog
  if (hw_watchdog_did_reset(watchdog) == true) {
    sys_printf("Last reset was due to watchdog\n");
  }

  // Loop and print out the status
  int count = 0;
  while (true) {
    // Poll the hardware devices occasionally to update status
    hw_poll();

    // Sleep for a bit
    sys_sleep(100);

    if (++count == 100) {
      sys_printf("Performing programmatic reset after 5s\n");
      hw_watchdog_reset(watchdog, 5000);
    } else if (count % 10 == 0) {
      sys_printf("Still running after %d iterations\n", count);
    }
  }

  // If we were to reach here, finalize the power management
  hw_power_finalize(power);
  hw_watchdog_finalize(watchdog);

  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();
  hw_init();

  // Run the task
  sys_printf("Running %s on %s\n", sys_env_name(), sys_env_serial());
  if (core0_task() == false) {
    sys_printf("Main: core 0 task failed\n");
  }

  // Cleanup and exit
  hw_exit();
  sys_exit();
  return 0;
}
