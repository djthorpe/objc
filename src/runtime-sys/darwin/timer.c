#include <dispatch/dispatch.h>
#include <runtime-sys/sys.h>

// Custom dispatch queue for background execution
static dispatch_queue_t queue;

/**
 * @brief Initializes a new timer context.
 */
sys_timer_t sys_timer_init(uint32_t interval_ms, void *userdata,
                           void (*callback)(sys_timer_t *)) {
  sys_timer_t timer;
  static bool initialized = false;
  if (!initialized) {
    // TODO: Need to call dispatch_release with the queue when done
    // to avoid memory leaks.
    queue = dispatch_queue_create("objc", DISPATCH_QUEUE_SERIAL);
    initialized = true;
  }

  // Check arguments
  timer.callback = NULL;
  timer.interval = 0;
  timer.ctx.ptr = NULL; // Initialize context pointer to NULL
  if (interval_ms == 0 || callback == NULL) {
    return timer; // Return an invalid timer context
  }

  // Store timer parameters (dispatch source created later in start)
  timer.callback = callback;
  timer.interval = interval_ms;
  timer.userdata = userdata; // Store userdata
  timer.ctx.ptr = NULL;      // Not running initially

  // Return timer context
  return timer;
}

static void sys_timer_callback(void *context) {
  sys_timer_t *timer = (sys_timer_t *)context;
  if (timer && timer->callback) {
    timer->callback(timer);
  }
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
  if (timer->ctx.ptr != NULL) {
    return false; // Timer is already running
  }

  // Create a new dispatch source for this run
  dispatch_source_t ctx =
      dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
  if (!ctx) {
    return false; // Failed to create dispatch source
  }

  // Set the event handler with timer context as userdata
  dispatch_set_context(ctx, timer);
  dispatch_source_set_event_handler_f(ctx, sys_timer_callback);

  // Set the timer interval and callback
  int64_t interval_nsec = timer->interval * 1000000LL; // Convert ms to ns
  dispatch_source_set_timer(
      ctx, dispatch_time(DISPATCH_TIME_NOW, interval_nsec), interval_nsec, 0);

  // Store the dispatch source and start the timer
  timer->ctx.ptr = ctx;
  dispatch_resume(ctx);

  return true; // Return true on success
}

/**
 * @brief Cancels and finalizes a timer.
 */
bool sys_timer_finalize(sys_timer_t *timer) {
  if (timer == NULL) {
    return false; // Invalid timer context
  }
  if (timer->ctx.ptr == NULL) {
    return false; // Invalid timer context (already finalized)
  }
  dispatch_source_t ctx = (dispatch_source_t)timer->ctx.ptr;

  // Cancel the timer
  dispatch_source_cancel(ctx);

  timer->ctx.ptr = NULL; // Clear the context pointer
  return true;           // Return true on success
}
