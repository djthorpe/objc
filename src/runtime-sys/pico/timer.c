#include <pico/time.h>
#include <runtime-sys/sys.h>
#include <runtime-sys/timer.h>

// Pico timer context structure that fits in the ctx buffer
typedef struct {
  repeating_timer_t repeating_timer;
} pico_timer_ctx_t;

/**
 * @brief Initializes a new timer context.
 */
sys_timer_t sys_timer_init(uint32_t interval_ms, void *userdata,
                           void (*callback)(sys_timer_t *)) {
  sys_timer_t timer;

  // Check arguments
  timer.callback = NULL;
  timer.interval = 0;
  timer.userdata = 0;
  sys_memset(&timer.ctx, 0, sizeof(timer.ctx)); // Clear the entire union
  if (interval_ms == 0 || callback == NULL) {
#ifdef DEBUG
    sys_printf("Invalid timer parameters\n");
#endif
    return timer; // Return an invalid timer context
  }
  if (sizeof(pico_timer_ctx_t) > SYS_TIMER_CTX_SIZE) {
#ifdef DEBUG
    sys_printf("Invalid timer context size\n");
#endif
    return timer; // Return an invalid timer context
  }

  // Store timer parameters
  timer.callback = callback;
  timer.interval = interval_ms;
  timer.userdata = userdata;

  // Return timer context
  return timer;
}

// Timer callback function for Pico timers
static bool pico_timer_callback(repeating_timer_t *rt) {
  // Get the timer from the user_data
  sys_timer_t *timer = (sys_timer_t *)rt->user_data;
  if (timer && timer->callback) {
    timer->callback(timer);
  }
  return true; // Continue repeating
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

  // Get pointer to the context buffer
  pico_timer_ctx_t *ctx = (pico_timer_ctx_t *)timer->ctx.ctx;

  // Check if timer is already running (alarm_id != 0 means it's active)
  if (ctx->repeating_timer.alarm_id != 0) {
    return false; // Timer is already running
  }

  // Initialize the context in the buffer
  sys_memset(ctx, 0, sizeof(pico_timer_ctx_t));

  // Get alarm pool
  alarm_pool_t *pool = alarm_pool_get_default();
  if (pool == NULL) {
    return false; // Failed to get default alarm pool
  }

  // Start the repeating timer with our callback and timer as user_data
  bool result = alarm_pool_add_repeating_timer_ms(
      pool, timer->interval, pico_timer_callback, timer, &ctx->repeating_timer);

  if (!result) {
    // Clear context on failure
    sys_memset(ctx, 0, sizeof(pico_timer_ctx_t));
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

  pico_timer_ctx_t *ctx = (pico_timer_ctx_t *)timer->ctx.ctx;
  // Check if timer is actually running (alarm_id != 0 means it's active)
  if (ctx->repeating_timer.alarm_id == 0) {
    return false; // Timer not running or already finalized
  }

  // Cancel the repeating timer
  cancel_repeating_timer(&ctx->repeating_timer);

  // Clear the context buffer
  sys_memset(ctx, 0, sizeof(pico_timer_ctx_t));

  return true; // Return true on success
}

/**
 * @brief Checks if a timer is valid, configured and running.
 */
bool sys_timer_valid(sys_timer_t *timer) {
  if (timer == NULL) {
    return false; // Invalid timer context
  }
  pico_timer_ctx_t *ctx = (pico_timer_ctx_t *)timer->ctx.ctx;
  // Check if timer is actually running (alarm_id != 0 means it's active)
  if (ctx->repeating_timer.alarm_id == 0) {
    return false; // Timer not running or already finalized
  }
  return true;
}
