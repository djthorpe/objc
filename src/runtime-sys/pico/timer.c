#include <runtime-sys/sys.h>
#include <runtime-sys/timer.h>

/**
 * @brief Initializes a new timer context.
 */
sys_timer_t sys_timer_init(uint32_t interval_ms, void *userdata,
                           void (*callback)(sys_timer_t *)) {
  sys_timer_t timer;
  // TODO: Initialize the timer with the specified parameters for Pico.
  (void)interval_ms; // Suppress unused parameter warning
  (void)userdata;    // Suppress unused parameter warning
  (void)callback;    // Suppress unused parameter warning
  return timer;
}

/**
 * @brief Starts a timer.
 */
bool sys_timer_start(sys_timer_t *timer) {
  if (timer == NULL) {
    return false; // Invalid timer context
  }
  // TODO: Start the timer with the specified interval and callback for Pico.
  return true; // Return true on success
}

/**
 * @brief Cancels and finalizes a timer.
 */
bool sys_timer_finalize(sys_timer_t *timer) {
  if (timer == NULL) {
    return false; // Invalid timer context
  }
  // TODO: Stop the timer and clean up any resources associated with it for Pico.
  return true; // Return true on success
}
