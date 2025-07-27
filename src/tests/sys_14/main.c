#include <runtime-sys/sys.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <tests/tests.h>

// Test data structures
typedef struct {
  sys_event_queue_t core0_to_core1_queue;
  atomic_int timer_count;
  atomic_int core0_consumed;
  atomic_int core1_consumed;
  atomic_bool should_stop;
  sys_timer_t timer;
} dual_core_test_data_t;

// Forward declarations
int test_sys_14(void);
static void core1_consumer_task(void *arg);
static void timer_callback(sys_timer_t *timer);
static void consume_events_loop(dual_core_test_data_t *data,
                                const char *core_name,
                                atomic_int *consumed_counter);

int main(void) { return TestMain("test_sys_14", test_sys_14); }

// Shared event consumption loop for both cores
static void consume_events_loop(dual_core_test_data_t *data,
                                const char *core_name,
                                atomic_int *consumed_counter) {
  while (!atomic_load(&data->should_stop)) {
    // Use blocking pop to compete fairly between cores
    printf("[DEBUG] %s: About to pop, queue_size=%zu\n", core_name,
           sys_event_queue_size(&data->core0_to_core1_queue));
    sys_event_t event = sys_event_queue_pop(&data->core0_to_core1_queue);

    if (event != NULL) {
      // Event value contains the event ID
      int event_id = (int)(uintptr_t)event;
      atomic_fetch_add(consumed_counter, 1);
      printf("%s consumed event %d, queue_size=%zu\n", core_name, event_id,
             sys_event_queue_size(&data->core0_to_core1_queue));
    } else {
      // NULL means queue was shut down or error - check should_stop
      printf("[DEBUG] %s: Got NULL event, queue_size=%zu\n", core_name,
             sys_event_queue_size(&data->core0_to_core1_queue));
      if (atomic_load(&data->should_stop)) {
        break;
      }
    }
  }
}

// Core 1 consumer task - continuously consumes events from the shared queue
static void core1_consumer_task(void *arg) {
  dual_core_test_data_t *data = (dual_core_test_data_t *)arg;
  consume_events_loop(data, "Core 1", &data->core1_consumed);
  printf("Core 1 consumer finished (consumed %d events)\n",
         atomic_load(&data->core1_consumed));
}

// Timer callback - called periodically to push events into the queue
static void timer_callback(sys_timer_t *timer) {
  dual_core_test_data_t *data = (dual_core_test_data_t *)timer->userdata;

  // Check if we should stop before doing anything
  if (atomic_load(&data->should_stop)) {
    return;
  }

  // Get current count BEFORE incrementing
  int current_count = atomic_load(&data->timer_count);
  if (current_count >= 10) {
    atomic_store(&data->should_stop, true);
    // Wake up any blocked consumers immediately
    sys_event_queue_shutdown(&data->core0_to_core1_queue);
    return;
  }

  // Increment the counter atomically only if we're going to create an event
  atomic_fetch_add(&data->timer_count, 1);
  int new_count = atomic_load(&data->timer_count);

  // Create an event with the current count as the value
  sys_event_t event = (sys_event_t)(uintptr_t)current_count;

  // Push event to the queue (use try_push since we're in IRQ context)
  if (sys_event_queue_try_push(&data->core0_to_core1_queue, event) == false) {
    sys_puts("[T] FAIL\n");
  }
}

int test_sys_14(void) {
  printf("=== Test sys_14: Dual-core event queue consumption with timer ===\n");

#ifdef SYSTEM_NAME_PICO
  printf("Running on Pico platform with 2 cores\n");
#else
  printf("Running on non-Pico platform\n");
  // On non-Pico platforms, we can't guarantee multicore behavior
  printf("This test is optimized for Pico's dual-core architecture\n");
#endif

  // Initialize test data - make it static to avoid stack issues with timer
  // callback
  static dual_core_test_data_t test_data = {0};
  atomic_init(&test_data.timer_count, 0);
  atomic_init(&test_data.core0_consumed, 0);
  atomic_init(&test_data.core1_consumed, 0);
  atomic_init(&test_data.should_stop, false);

  // Create the shared event queue
  test_data.core0_to_core1_queue = sys_event_queue_init(20);
  printf("✓ Event queue created successfully\n");

  // Initialize the timer (100ms intervals - faster for debugging)
  test_data.timer = sys_timer_init(100, &test_data, timer_callback);
  printf("✓ Timer initialized (100ms intervals)\n");

#ifdef SYSTEM_NAME_PICO
  // On Pico, start the core 1 consumer task
  printf("Starting core 1 consumer task...\n");
  if (!sys_thread_create_on_core(core1_consumer_task, &test_data, 1)) {
    printf("✗ Failed to start core 1 consumer task\n");
    sys_timer_finalize(&test_data.timer);
    sys_event_queue_finalize(&test_data.core0_to_core1_queue);
    return 1;
  }
  printf("✓ Core 1 consumer task started\n");
#endif

  // Start the timer FIRST, before cores start consuming
  printf("[DEBUG] About to start timer on core %d\n", sys_thread_core());
  if (!sys_timer_start(&test_data.timer)) {
    printf("✗ Failed to start timer\n");
#ifdef SYSTEM_NAME_PICO
    // Core 1 task is already running, we can't easily stop it
    // but the test data cleanup will signal it to stop
    atomic_store(&test_data.should_stop, true);
#endif
    sys_event_queue_finalize(&test_data.core0_to_core1_queue);
    return 1;
  }
  printf("✓ Timer started\n");

  // Core 0 also consumes events using the shared consumption loop
  consume_events_loop(&test_data, "Core 0", &test_data.core0_consumed);

  // Stop and finalize the timer
  sys_timer_finalize(&test_data.timer);
  printf("✓ Timer stopped and finalized\n");

  // Signal core 1 to stop (already done by timer, but ensure it's set)
  atomic_store(&test_data.should_stop, true);

  // Wait for core 1 to finish
  sys_sleep(100);

  // Collect final statistics
  int total_timer_events = atomic_load(&test_data.timer_count);
  int core0_consumed = atomic_load(&test_data.core0_consumed);
  int core1_consumed = atomic_load(&test_data.core1_consumed);
  int total_consumed = core0_consumed + core1_consumed;

  printf("\n=== Test Results ===\n");
  printf("Timer events generated: %d\n", total_timer_events);
  printf("Core 0 consumed: %d events\n", core0_consumed);
  printf("Core 1 consumed: %d events\n", core1_consumed);
  printf("Total consumed: %d events\n", total_consumed);

  // Cleanup
  sys_event_queue_finalize(&test_data.core0_to_core1_queue);
  printf("✓ Event queue finalized\n");

  // Verify results
  if (total_timer_events != 10) {
    printf("✗ Expected 10 timer events, got %d\n", total_timer_events);
    return 1;
  }

  if (total_consumed != 10) {
    printf("✗ Expected 10 total consumed events, got %d\n", total_consumed);
    return 1;
  }

#ifdef SYSTEM_NAME_PICO
  // On Pico, we expect both cores to have consumed some events
  if (core0_consumed == 0) {
    printf("✗ Core 0 didn't consume any events\n");
    return 1;
  }

  if (core1_consumed == 0) {
    printf("✗ Core 1 didn't consume any events\n");
    return 1;
  }

  printf("✓ Both cores successfully consumed events from shared queue\n");
#else
  // On non-Pico platforms, just verify core 0 worked
  if (core0_consumed == 0) {
    printf("✗ Core 0 didn't consume any events\n");
    return 1;
  }
  printf("✓ Core 0 successfully consumed events\n");
#endif

  printf("✓ All tests passed!\n");
  return 0;
}
