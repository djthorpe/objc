/**
 * @file examples/runtime/watchdog/main.m
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
// TIMER CALLBACK

static int timer_count = 0;

void timer_callback(sys_timer_t *timer) {
  hw_watchdog_t *watchdog = (hw_watchdog_t *)timer->userdata;
  sys_assert(watchdog && hw_watchdog_valid(watchdog));

  // Did the watchdog trigger a reset?
  if (timer_count == 0 && hw_watchdog_did_reset(watchdog)) {
    sys_printf("core %d: Last reboot was triggered by watchdog!\n",
               sys_thread_core());
  }

  // Enable the watchdog
  if (timer_count == 0) {
    sys_printf("core %d: Enabling watchdog with max timeout %lu ms\n",
               sys_thread_core(), hw_watchdog_maxtimeout());
    hw_watchdog_enable(watchdog, true);
  }

  // Ping the watchdog to reset the timeout
  hw_watchdog_ping(watchdog);
  sys_printf("core %d: Pinged watchdog\n", (int)sys_thread_core());

  // Increment the timer count
  if (++timer_count >= 10) {
    // After 10 invocations, cancel the timer
    sys_printf("core %d: Stopping timer after %d invocations\n",
               sys_thread_core(), timer_count);
    sys_timer_finalize(timer);
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
  // Create the watchdog with max timeout
  // Note the watchdog is not yet enabled, we will enable it later
  hw_watchdog_t watchdog = hw_watchdog_init(hw_watchdog_maxtimeout());
  if (!hw_watchdog_valid(&watchdog)) {
    sys_printf("core 0: Failed to initialize watchdog\n");
    return false;
  }

  // Create a timer that will trigger every 1s
  sys_timer_t timer = sys_timer_init(1000, &watchdog, timer_callback);
  if (!sys_timer_start(&timer)) {
    sys_printf("core 0: Failed to start timer\n");
    hw_watchdog_finalize(&watchdog);
    return false;
  }

  // Run the event processing loop - which is just sleeping for 1s
  do {
    sys_sleep(1000);
  } while (true);

  // If we were to reach here, finalize the timer and watchdog
  sys_timer_finalize(&timer);
  hw_watchdog_finalize(&watchdog);

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
  if (core0_task() == false) {
    sys_printf("Main: core 0 task failed\n");
  }

  // Cleanup and exit
  hw_exit();
  sys_exit();
  return 0;
}
