#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <runtime-sys/sys.h>
#include <signal.h>
#include <string.h>
#include <time.h>

// Timer callback function for POSIX timers
static void linux_timer_callback(union sigval sv) {
  sys_timer_t *timer = (sys_timer_t *)sv.sival_ptr;
  if (timer && timer->callback) {
    timer->callback(timer);
  }
}

/**
 * @brief Initializes a new timer context.
 */
sys_timer_t sys_timer_init(uint32_t interval_ms, void *userdata,
                           void (*callback)(sys_timer_t *)) {
  sys_timer_t timer;

  // Check arguments
  timer.callback = NULL;
  timer.interval = 0;
  memset(&timer.ctx, 0, sizeof(timer.ctx)); // Clear the entire union
  if (interval_ms == 0 || callback == NULL) {
    return timer; // Return an invalid timer context
  }
  if (sizeof(timer_t) > SYS_TIMER_CTX_SIZE) {
    return timer; // Return an invalid timer context
  }

  // Store timer parameters
  timer.callback = callback;
  timer.interval = interval_ms;
  timer.userdata = userdata;

  // Return timer context
  return timer;
}

/**
 * @brief Starts a timer.
 */
bool sys_timer_start(sys_timer_t *timer) {
  if (timer == NULL) {
    return false; // Invalid timer context
  }
  if (timer->callback == NULL) {
    return false; // Invalid timer (not properly initialized)
  }

  // Check if timer is already running (ctx buffer has been initialized)
  timer_t *timer_id = (timer_t *)timer->ctx.ctx;
  if (*timer_id != 0) {
    return false; // Timer is already running
  }

  // Initialize the context in the buffer
  *timer_id = 0;

  // Create the timer event structure
  struct sigevent event = {
      .sigev_notify = SIGEV_THREAD,
      .sigev_value.sival_ptr = timer, // Pass timer as userdata
      .sigev_notify_function = linux_timer_callback,
  };

  if (timer_create(CLOCK_MONOTONIC, &event, timer_id) != 0) {
    // Clear context on failure
    *timer_id = 0;
    return false;
  }

  // Set up the timer interval (convert ms to timespec)
  struct itimerspec timer_spec = {
      .it_interval = {.tv_sec = timer->interval / 1000,
                      .tv_nsec = (timer->interval % 1000) * 1000000L},
      .it_value = {.tv_sec = timer->interval / 1000,
                   .tv_nsec = (timer->interval % 1000) * 1000000L}};

  // Start the timer
  if (timer_settime(*timer_id, 0, &timer_spec, NULL) != 0) {
    // Clean up on failure
    timer_delete(*timer_id);
    *timer_id = 0;
    return false;
  }

  return true; // Return true on success
}

/**
 * @brief Cancels and finalizes a timer.
 */
bool sys_timer_finalize(sys_timer_t *timer) {
  if (timer == NULL) {
    return false; // Invalid timer context
  }

  timer_t *timer_id = (timer_t *)timer->ctx.ctx;
  if (*timer_id == 0) {
    return false; // Timer not running or already finalized
  }

  // Delete the POSIX timer
  timer_delete(*timer_id);

  // Clear the context buffer
  *timer_id = 0;

  return true; // Return true on success
}
