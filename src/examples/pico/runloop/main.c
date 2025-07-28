/**
 * @file main.c
 * @brief Pico event queue as a runloop example
 *
 * This example demonstrates how to use the sys_thread_create_on_core() function
 * to process events on both cores in a runloop style on the Raspberry Pi Pico
 * (RP2040).
 *
 * In this example, we create a timer on each core which produces events,
 * which are then consumed on either core. This is a "runloop" pattern where
 * we expect to continue to process events indefinitely.
 *
 * We call shutdown after producing a number of events from either timer, and
 * the consumers on each core will end when all events have been drained.
 */
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

// Event counters for each core
static int event_count[2];

/////////////////////////////////////////////////////////////////////
// RUNLOOP

void runloop(sys_event_queue_t *queue) {
  int core = sys_thread_core();
  sys_printf("Starting runloop on core %d...\n", core);

  // Process events until the queue is empty
  while (true) {
    sys_event_t event = sys_event_queue_pop(queue);
    if (event == NULL) {
      // No more events to process
      break;
    }

    // Simulate processing the event
    // Sleep for a random time less than 1000ms
    sys_printf("core %d: Processing event: %s (queue size=%d)\n", core,
               (char *)event, sys_event_queue_size(queue));
    sys_sleep(sys_random_uint32() % 1000);

    // Free the allocated string
    sys_free(event);
  }
  sys_printf("core %d: Runloop finished. Total events generated: %d\n", core,
             event_count[core]);
}

/////////////////////////////////////////////////////////////////////
// TIMER CALLBACK

void timer_callback(sys_timer_t *timer) {
  sys_event_queue_t *queue = (sys_event_queue_t *)timer->userdata;
  int core = sys_thread_core();

  // Shutdown the queue and finalize the timer if we have produced enough events
  if (event_count[core] >= 100) {
    sys_printf("core %d: Shutting down queue after %d events\n", core,
               event_count[core]);
    sys_event_queue_shutdown(queue);
    sys_timer_finalize(timer);
    return;
  }

  // Create a new event string
  char *event_data = sys_malloc(50);
  if (!event_data) {
    return;
  } else {
    sys_sprintf(event_data, 50, "event(core=%d timer=%p count=%d)", core, timer,
                ++event_count[core]);
  }

  // Push the event into the queue
  if (!sys_event_queue_try_push(queue, event_data)) {
    sys_printf("core %d: Failed to push event\n", core);
    sys_free(event_data);
    return;
  }
}

/////////////////////////////////////////////////////////////////////
// CORE 1 TASKS

/**
 * @brief Function to run on core 1
 *
 * This function is executed on core 1 of the RP2040 (or just simply "another
 * thread" on other platforms). It creates a timer that produces events, which
 * are then fed into the event queue.
 *
 * A runloop is then run until there are no more events to process.
 */
void core1_task(void *arg) {
  sys_event_queue_t *queue = (sys_event_queue_t *)arg;
  sys_printf("core %d: Starting task\n", sys_thread_core());

  // Create a timer that produces events
  sys_timer_t timer =
      sys_timer_init(250, queue, timer_callback); // 250 ms interval
  if (sys_timer_start(&timer) == false) {
    sys_printf("core %d: Failed to start timer\n", sys_thread_core());
    return;
  } else {
    sys_printf("core %d: Timer started successfully with interval %d\n",
               sys_thread_core(), timer.interval);
  }

  // Run the event processing loop
  runloop(queue);

  sys_printf("core %d: Task completed!\n", sys_thread_core());
}

/////////////////////////////////////////////////////////////////////
// CORE 0 TASKS

/**
 * @brief Function to run on core 0
 *
 * This function creates a queue, starts execution of the core 1 task,
 * and then does exactly the same thing - processes events from the queue
 *
 * A runloop is then run until there are no more events to process.
 */
bool core0_task() {
  // Initialize the event queue with a capacity of 20 events
  sys_event_queue_t queue = sys_event_queue_init(20);

  // Create the task on core 1. If this wasn't an RP2040, we would
  // just use sys_thread_create() instead.
  sys_printf("main: Starting task on core 1...\n");
  if (sys_thread_create_on_core(core1_task, &queue, 1) == false) {
    sys_printf("main: Failed to launch task on core 1\n");
    return false;
  }

  // Create a timer that produces events
  sys_timer_t timer =
      sys_timer_init(250, &queue, timer_callback); // 250 ms interval
  if (sys_timer_start(&timer) == false) {
    sys_printf("core %d: Failed to start timer\n", sys_thread_core());
    return false;
  } else {
    sys_printf("core %d: Timer started successfully with interval %d\n",
               sys_thread_core(), timer.interval);
  }

  // Run the event processing loop
  runloop(&queue);

  sys_printf("core %d: Task completed!\n", sys_thread_core());
  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();

  // Run the task
  if (core0_task() == false) {
    sys_printf("Main: core 0 task failed\n");
  }

  sys_printf("For pico, we need to add timers to the appropriate alarm pool... "
             "at the moment all the timers are on core 0\n");

  // Cleanup and exit
  sys_exit();
  return 0;
}
