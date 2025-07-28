/**
 * @file main.c
 * @brief Pico producer/consumer event queue
 * @ingroup Examples
 *
 * This example demonstrates how to use the sys_thread_create_on_core() function
 * to run tasks on core 1 of the Raspberry Pi Pico (RP2040), and how to create
 * events on one core which are consumed on the other core.
 *
 * In this example, we create events on core 1 (the producer) and consume them
 * on core 0 (the consumer). The events are pushed into a shared event queue
 * using sys_event_queue_push(), and consumed using sys_event_queue_pop().
 *
 * We call shutdown after producing a number of events on the producer core,
 * and drain all events on the consumer core, until the queue is empty.
 *
 * You can tune the queue size to control how many events can be stored.
 * In this example, the consumer takes a lot of time to consume events
 * so the queue grows until the producer is then unable to push more
 * events.
 *
 * @example main.c
 * This is a complete example showing how a simple use of an event queue works
 * in a multicore programming context on the Pico platform.
 */
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

static int producer_event_count = 0; // Count of events produced
static int consumer_event_count = 0; // Count of events consumed

/////////////////////////////////////////////////////////////////////
// CORE 1 TASKS

/**
 * @brief Function to run on core 1
 *
 * This function is executed on core 1 of the RP2040 (or just simply "another
 * thread" on other platforms). It creates events at a random interval, and
 * then shuts down the queue, before ending.
 */
void core1_task(void *arg) {
  sys_event_queue_t *queue = (sys_event_queue_t *)arg;
  sys_printf("core %d: Starting task\n", sys_thread_core());

  // Create 100 events to be consumed by core 0
  for (int i = 0; i < 100; i++) {
    // Create an event with some data
    char *event_data = (char *)sys_malloc(32);
    if (!event_data) {
      continue;
    } else {
      sys_sprintf(event_data, 32, "Event(event=%d, core=%d)", i + 1,
                  sys_thread_core());
    }

    // Push the event into the queue. This will fail if the queue is full
    // but we just free the data and continue. If this was a real
    // application, we would handle this more gracefully.
    if (sys_event_queue_try_push(queue, event_data)) {
      sys_printf("  core %d: Pushed event: %s\n", sys_thread_core(),
                 event_data);
      producer_event_count++;
    } else {
      sys_printf("  core %d: Queue full, dropping event: %s\n",
                 sys_thread_core(), event_data);
      sys_free(event_data);
    }

    // Sleep for some time less than 100ms
    sys_sleep(sys_random_uint32() % 100);
  }

  // Shutdown the queue after producing events
  sys_event_queue_shutdown(queue);
  sys_printf("core %d: Task completed!\n", sys_thread_core());
}

/////////////////////////////////////////////////////////////////////
// CORE 0 TASKS

/**
 * @brief Function to run on core 0 (or "main thread")
 *
 * This function is executed on core 0 of the RP2040 (or "main
 * thread" on other platforms). It creates a shared event queue, waits for
 * events to be produced by core 1, consumes them, and then drains the queue
 * until empty.
 *
 * It doesn't make an attempt to wait for core 1 to finish, but rather
 * just consumes all events until the queue is empty, then finalizes the queue.
 */
bool core0_task() {
  // Initialize the event queue with a capacity of 10 events
  sys_event_queue_t queue = sys_event_queue_init(10);

  // Create the task on core 1. If this wasn't an RP2040, we would
  // just use sys_thread_create() instead.
  sys_printf("main: Starting task on core 1...\n");
  if (sys_thread_create_on_core(core1_task, &queue, 1) == false) {
    sys_printf("main: Failed to launch task on core 1\n");
    return false;
  }

  // Consume events from the queue until empty
  while (true) {
    sys_event_t event = sys_event_queue_pop(&queue);
    if (event == NULL) {
      sys_printf("  main: No more events to consume\n");
      break;
    }

    // Simulate doing some work with the event
    sys_printf("  main: Consumed event: %s (queuesize=%d)\n", (char *)event,
               sys_event_queue_size(&queue));
    consumer_event_count++;

    // Random processing time longer than core1 time to simulate
    // it takes more work for the consumer to process the event
    // than the producer takes to produce it
    sys_sleep(sys_random_uint32() % 500);

    // Free the allocated string
    sys_free(event);
  }

  // Finalize the event queue
  sys_event_queue_finalize(&queue);
  sys_printf("main: Event queue finalized\n");
  sys_printf("main: Events produced: %d, Events consumed: %d\n",
             producer_event_count, consumer_event_count);

  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();

  // Run the task
  if (core0_task() == false) {
    sys_printf("Main: Core 0 task failed\n");
  }

  // Cleanup and exit
  sys_exit();
  return 0;
}
