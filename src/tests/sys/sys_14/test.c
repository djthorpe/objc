#include <runtime-sys/sys.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <tests/tests.h>

// Test data structures
typedef struct {
  sys_event_queue_t core0_to_core1_queue;
  atomic_int timer_count;
  atomic_int core0_consumed;
  atomic_int core1_consumed;
  atomic_bool should_stop;
  sys_timer_t core0_timer;
  sys_timer_t core1_timer;
} dual_core_test_data_t;

// Forward declarations
static void core1_consumer_task(void *arg);
static void timer_callback(sys_timer_t *timer);
static void consume_events_loop(dual_core_test_data_t *data,
                                const char *core_name,
                                atomic_int *consumed_counter);

// Shared event consumption loop for both cores
static void consume_events_loop(dual_core_test_data_t *data,
                                const char *core_name,
                                atomic_int *consumed_counter) {
  while (!atomic_load(&data->should_stop)) {
    // Use blocking pop for efficiency - no polling needed
    sys_event_t event = sys_event_queue_pop(&data->core0_to_core1_queue);

    if (event != NULL) {
      // Decode event: core_id is in thousands, event_id is remainder
      int event_value = (int)(uintptr_t)event;
      int producer_core = (event_value >= 1000) ? 1 : 0;
      int event_id = (event_value >= 1000) ? (event_value - 1000) : event_value;

      atomic_fetch_add(consumed_counter, 1);
      sys_printf("%s consumed event %d (from Core %d), queue_size=%zu\n",
                 core_name, event_id, producer_core,
                 sys_event_queue_size(&data->core0_to_core1_queue));

      // Add random processing delay between 25-75ms to simulate real work
      // and improve fairness between cores (moderate delay to force cross-core
      // mixing)
      int delay_ms = 25 + (sys_random_uint32() % 50); // Random 25-75ms
      sys_sleep(delay_ms);
    } else {
      // NULL means queue was shut down - exit loop
      break;
    }
  }

  // After timer stops, drain any remaining events from the queue
  sys_event_t event;
  while ((event = sys_event_queue_try_pop(&data->core0_to_core1_queue)) !=
         NULL) {
    int event_value = (int)(uintptr_t)event;
    int producer_core = (event_value >= 1000) ? 1 : 0;
    int event_id = (event_value >= 1000) ? (event_value - 1000) : event_value;

    atomic_fetch_add(consumed_counter, 1);
    sys_printf("%s drained event %d (from Core %d), queue_size=%zu\n",
               core_name, event_id, producer_core,
               sys_event_queue_size(&data->core0_to_core1_queue));

    // Add random processing delay during draining to create fair competition
    int delay_ms = 10 + (sys_random_uint32() % 20); // Random 10-30ms
    sys_sleep(delay_ms);
  }
}

// Core 1 consumer task - starts its own timer and continuously consumes events
// from the shared queue (works as consumer thread on macOS/Linux)
static void core1_consumer_task(void *arg) {
  dual_core_test_data_t *data = (dual_core_test_data_t *)arg;

  // Start the core 1 timer (or consumer thread timer on macOS/Linux)
  if (!sys_timer_start(&data->core1_timer)) {
#ifdef SYSTEM_NAME_PICO
    sys_printf("✗ Failed to start timer on core 1\n");
#else
    sys_printf("✗ Failed to start timer on consumer thread\n");
#endif
    atomic_store(&data->should_stop, true);
    return;
  }
#ifdef SYSTEM_NAME_PICO
  sys_printf("✓ Core 1 timer started\n");
#else
  sys_printf("✓ Consumer thread timer started\n");
#endif

#ifdef SYSTEM_NAME_PICO
  consume_events_loop(data, "Core 1", &data->core1_consumed);
  sys_printf("Core 1 consumer finished (consumed %d events)\n",
             atomic_load(&data->core1_consumed));
#else
  consume_events_loop(data, "Consumer Thread", &data->core1_consumed);
  sys_printf("Consumer thread finished (consumed %d events)\n",
             atomic_load(&data->core1_consumed));
#endif
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
  if (current_count >= 100) {
    atomic_store(&data->should_stop, true);
    // Wake up any blocked consumers immediately
    sys_event_queue_shutdown(&data->core0_to_core1_queue);
    return;
  }

  // Determine which core this timer is running on
  // Core 0 events: 1-50 (odd numbers for easy identification)
  // Core 1 events: 51-100 (even numbers for easy identification)
  int core_id = (timer == &data->core0_timer) ? 0 : 1;
  int event_value = current_count + 1 + (core_id * 1000); // Add core offset

  // Create an event with core identification
  sys_event_t event = (sys_event_t)(uintptr_t)event_value;

  // Push event to the queue (use try_push since we're in IRQ context)
  // Only increment counter if push was successful
  if (sys_event_queue_try_push(&data->core0_to_core1_queue, event)) {
    atomic_fetch_add(&data->timer_count, 1);
  }
}

int test_sys_14(void) {
  sys_printf(
      "=== Test sys_14: Dual-core event queue consumption with timer ===\n");

#ifdef SYSTEM_NAME_PICO
  sys_printf("Running on Pico platform with 2 cores\n");
#else
  sys_printf("Running on macOS/Linux platform with threading\n");
  // On macOS/Linux platforms, we use regular threading instead of core-specific
  // threads
  sys_printf("This test validates multi-threaded event queue behavior\n");
#endif

  // Initialize test data - make it static to avoid stack issues with timer
  // callback
  static dual_core_test_data_t test_data = {0};
  atomic_init(&test_data.timer_count, 0);
  atomic_init(&test_data.core0_consumed, 0);
  atomic_init(&test_data.core1_consumed, 0);
  atomic_init(&test_data.should_stop, false);

  // Create the shared event queue (larger capacity to handle high production
  // rate)
  test_data.core0_to_core1_queue = sys_event_queue_init(100);
  sys_printf("✓ Event queue created successfully\n");

  // Initialize the timers (25ms intervals for both cores)
  test_data.core0_timer = sys_timer_init(25, &test_data, timer_callback);
  test_data.core1_timer = sys_timer_init(25, &test_data, timer_callback);
  sys_printf("✓ Timers initialized (25ms intervals)\n");

#ifdef SYSTEM_NAME_PICO
  // On Pico, start the core 1 consumer task
  sys_printf("Starting core 1 consumer task...\n");
  if (!sys_thread_create_on_core(core1_consumer_task, &test_data, 1)) {
    sys_printf("✗ Failed to start core 1 consumer task\n");
    sys_timer_finalize(&test_data.core0_timer);
    sys_timer_finalize(&test_data.core1_timer);
    sys_event_queue_finalize(&test_data.core0_to_core1_queue);
    return 1;
  }
  sys_printf("✓ Core 1 consumer task started\n");
#else
  // On macOS/Linux, start a regular consumer thread
  sys_printf("Starting consumer thread...\n");
  if (!sys_thread_create(core1_consumer_task, &test_data)) {
    sys_printf("✗ Failed to start consumer thread\n");
    sys_timer_finalize(&test_data.core0_timer);
    sys_timer_finalize(&test_data.core1_timer);
    sys_event_queue_finalize(&test_data.core0_to_core1_queue);
    return 1;
  }
  sys_printf("✓ Consumer thread started\n");
#endif

  // Start the core 0 timer (or main thread timer on macOS/Linux)
  if (!sys_timer_start(&test_data.core0_timer)) {
#ifdef SYSTEM_NAME_PICO
    sys_printf("✗ Failed to start timer on core 0\n");
#else
    sys_printf("✗ Failed to start timer on main thread\n");
#endif
    atomic_store(&test_data.should_stop, true);
    return 1;
  }
#ifdef SYSTEM_NAME_PICO
  sys_printf("✓ Core 0 timer started\n");
#else
  sys_printf("✓ Main thread timer started\n");
#endif

  // Wait a moment for consumer thread to be ready
  sys_sleep(50);

#ifdef SYSTEM_NAME_PICO
  // Core 0 also consumes events using the shared consumption loop
  consume_events_loop(&test_data, "Core 0", &test_data.core0_consumed);
#else
  // Main thread also consumes events using the shared consumption loop
  consume_events_loop(&test_data, "Main Thread", &test_data.core0_consumed);
#endif

  // Stop and finalize the timers
  sys_timer_finalize(&test_data.core0_timer);
  sys_timer_finalize(&test_data.core1_timer);
  sys_printf("✓ Timers stopped and finalized\n");

  // Signal core 1 to stop (already done by timer, but ensure it's set)
  atomic_store(&test_data.should_stop, true);

  // Give both cores a fair chance to drain remaining events
  // Core 0 also participates in draining
  sys_printf("Core 0 starting to drain remaining events...\n");
  sys_event_t event;
  while ((event = sys_event_queue_try_pop(&test_data.core0_to_core1_queue)) !=
         NULL) {
    int event_value = (int)(uintptr_t)event;
    int producer_core = (event_value >= 1000) ? 1 : 0;
    int event_id = (event_value >= 1000) ? (event_value - 1000) : event_value;

    atomic_fetch_add(&test_data.core0_consumed, 1);
    sys_printf("Core 0 drained event %d (from Core %d), queue_size=%zu\n",
               event_id, producer_core,
               sys_event_queue_size(&test_data.core0_to_core1_queue));

    // Add random processing delay during draining to create fair competition
    int delay_ms = 10 + (sys_random_uint32() % 20); // Random 10-30ms
    sys_sleep(delay_ms);
  }

  // Wait for core 1 to finish
  sys_sleep(100);

  // Collect final statistics
  int total_timer_events = atomic_load(&test_data.timer_count);
  int core0_consumed = atomic_load(&test_data.core0_consumed);
  int core1_consumed = atomic_load(&test_data.core1_consumed);
  int total_consumed = core0_consumed + core1_consumed;

  sys_printf("\n=== Test Results ===\n");
  sys_printf("Timer events generated: %d\n", total_timer_events);
  sys_printf("Core 0 consumed: %d events\n", core0_consumed);
  sys_printf("Core 1 consumed: %d events\n", core1_consumed);
  sys_printf("Total consumed: %d events\n", total_consumed);

  // Cleanup
  sys_event_queue_finalize(&test_data.core0_to_core1_queue);
  sys_printf("✓ Event queue finalized\n");

  // Verify results
  if (total_timer_events != 100) {
    sys_printf("✗ Expected 100 timer events, got %d\n", total_timer_events);
    return 1;
  }

  if (total_consumed != 100) {
    sys_printf("✗ Expected 100 total consumed events, got %d\n",
               total_consumed);
    return 1;
  }

#ifdef SYSTEM_NAME_PICO
  // On Pico, we expect both cores to have consumed some events
  if (core0_consumed == 0) {
    sys_printf("✗ Core 0 didn't consume any events\n");
    return 1;
  }

  if (core1_consumed == 0) {
    sys_printf("✗ Core 1 didn't consume any events\n");
    return 1;
  }

  sys_printf("✓ Both cores successfully consumed events from shared queue\n");
#else
  // On macOS/Linux platforms, just verify both threads worked
  if (core0_consumed == 0) {
    sys_printf("✗ Main thread didn't consume any events\n");
    return 1;
  }
  if (core1_consumed == 0) {
    sys_printf("✗ Consumer thread didn't consume any events\n");
    return 1;
  }
  sys_printf("✓ Both threads successfully consumed events from shared queue\n");
#endif

  sys_printf("✓ All tests passed!\n");
  return 0;
}
