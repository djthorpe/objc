/**
 * @file examples/runtime/watchdog/main.c
 * @brief Watchdog reboot example
 *
 * This example demonstrates how to use the hardware watchdog timer to
 * automatically reboot the system if it becomes unresponsive.
 *
 * We configure the watchdog timer with a timeout period, and then
 * periodically "ping" the watchdog to reset the timeout. If the
 * application fails to ping the watchdog within the timeout period,
 * the watchdog will trigger a system reset.
 *
 * After 10 timer invocations, we no longer ping the watchdog,
 * which will cause the system to reboot after the timeout period.
 */
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// CORE 0 TASKS

/**
 * @brief Function to run on core 0
 *
 * This function creates a watchdog and does hw_poll to keep everything
 * running smoothly.
 */
bool core0_task() {
  // Create the watchdog
  hw_watchdog_t *watchdog = hw_watchdog_init();
  if (!hw_watchdog_valid(watchdog)) {
    sys_printf("core 0: Failed to initialize watchdog\n");
    return false;
  }

  // Explain that the last boot was caused by the watchdog resetting
  if (hw_watchdog_did_reset(watchdog)) {
    sys_printf("Watchdog triggered a reset\n");
  }

  // Run the event processing loop - which is just sleeping for 1s
  int count = 0;
  do {
    sys_printf("Ping the watchdog (%d)\n", count++);
    hw_poll();
    sys_sleep(100);
    if (count == 50) {
      sys_printf("Resetting the watchdog in 5s\n");
      hw_watchdog_reset(watchdog, 5000);
    }
  } while (true);

  // If we were to reach here, finalize the watchdog
  hw_watchdog_finalize(watchdog);

  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();
  hw_init();

  // Check that watchdog is supported
  if (hw_watchdog_maxtimeout() == 0) {
    sys_printf("Watchdog not supported on this hardware\n");
    return -1;
  }

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
