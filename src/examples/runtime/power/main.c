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

void power_callback(hw_power_t *power, hw_power_flag_t flags,
                    uint8_t battery_percent) {
  (void)power;
  if (flags & HW_POWER_BATTERY) {
    sys_printf("Power source is battery (estimated %u%%)\n", battery_percent);
  }
  if (flags & HW_POWER_USB) {
    sys_printf("Power source is USB (estimated %u%%)\n", battery_percent);
  }
  if (flags & HW_POWER_UNKNOWN) {
    sys_printf("Power source is unknown\n");
  }
  if (flags & HW_POWER_RESET) {
    sys_printf("Power is about to force a reset\n");
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
    sys_printf("core 0: Failed to initialize power management\n");
    return false;
  }

  // Loop and print out the status
  while (true) {
    // Poll the hardware devices occasionally to update status
    hw_poll();

    // Sleep for a bit
    sys_sleep(100);
  }

  // If we were to reach here, finalize the power management
  hw_power_finalize(power);

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
