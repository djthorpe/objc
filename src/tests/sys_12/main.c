#include <runtime-sys/event.h>
#include <runtime-sys/sync.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

// Forward declarations
int test_sys_12(void);
int test_basic_queue_operations(void);
int test_queue_overflow_behavior(void);
int test_producer_consumer_threading(void);
int test_peek_before_acquire(void);
int test_queue_shutdown(void);
int test_high_volume_stress(void);
int test_rapid_shutdown_stress(void);
int test_mixed_operations_stress(void);

int main(void) {
  // Run event queue tests
  return TestMain("test_sys_12", test_sys_12);
}

int test_sys_12(void) {
  int result = 0;

  sys_printf("=== Event Queue Tests ===\n");

  // Test 1: Basic queue operations
  sys_printf("Running basic queue operations test...\n");
  result = test_basic_queue_operations();
  if (result != 0) {
    sys_printf("FAILED: Basic queue operations test\n");
    return result;
  }
  sys_printf("PASSED: Basic queue operations test\n");

  // Test 2: Queue overflow behavior
  sys_printf("Running queue overflow behavior test...\n");
  result = test_queue_overflow_behavior();
  if (result != 0) {
    sys_printf("FAILED: Queue overflow behavior test\n");
    return result;
  }
  sys_printf("PASSED: Queue overflow behavior test\n");

#ifdef SYSTEM_NAME_PICO
  // On Pico, skip concurrent threading tests and run simplified versions
  sys_printf("Running simplified producer/consumer test for Pico...\n");
  result = test_producer_consumer_threading();
  if (result != 0) {
    sys_printf("FAILED: Simplified producer/consumer test\n");
    return result;
  }
  sys_printf("PASSED: Simplified producer/consumer test\n");
#else
  // Test 3: Producer/consumer threading (full test on other platforms)
  sys_printf("Running producer/consumer threading test...\n");
  result = test_producer_consumer_threading();
  if (result != 0) {
    sys_printf("FAILED: Producer/consumer threading test\n");
    return result;
  }
  sys_printf("PASSED: Producer/consumer threading test\n");
#endif

  // Test 4: Peek before acquire
  sys_printf("Running peek before acquire test...\n");
  result = test_peek_before_acquire();
  if (result != 0) {
    sys_printf("FAILED: Peek before acquire test\n");
    return result;
  }
  sys_printf("PASSED: Peek before acquire test\n");

  // Test 5: Queue shutdown
  sys_printf("Running queue shutdown test...\n");
  result = test_queue_shutdown();
  if (result != 0) {
    sys_printf("FAILED: Queue shutdown test\n");
    return result;
  }
  sys_printf("PASSED: Queue shutdown test\n");

#ifndef SYSTEM_NAME_PICO
  // Skip stress tests on Pico as they require heavy concurrent access
  // Test 6: High volume stress test
  sys_printf("Running high volume stress test...\n");
  result = test_high_volume_stress();
  if (result != 0) {
    sys_printf("FAILED: High volume stress test\n");
    return result;
  }
  sys_printf("PASSED: High volume stress test\n");

  // Test 7: Rapid shutdown stress test
  sys_printf("Running rapid shutdown stress test...\n");
  result = test_rapid_shutdown_stress();
  if (result != 0) {
    sys_printf("FAILED: Rapid shutdown stress test\n");
    return result;
  }
  sys_printf("PASSED: Rapid shutdown stress test\n");

  // Test 8: Mixed operations stress test
  sys_printf("Running mixed operations stress test...\n");
  result = test_mixed_operations_stress();
  if (result != 0) {
    sys_printf("FAILED: Mixed operations stress test\n");
    return result;
  }
  sys_printf("PASSED: Mixed operations stress test\n");
#else
  sys_printf("Skipping stress tests on Pico (require concurrent threading)\n");
#endif

  sys_printf("=== All Event Queue Tests Passed ===\n");
  return 0;
}

/**
 * Test basic queue operations: init, push, pop, try_push, try_pop, size, empty
 */
int test_basic_queue_operations(void) {
  // Initialize queue with capacity 3
  sys_event_queue_t queue = sys_event_queue_init(3);
  if (queue.items == NULL) {
    sys_printf("ERROR: Failed to initialize event queue\n");
    return 1;
  }

  // Check initial state
  if (!sys_event_queue_empty(&queue)) {
    sys_printf("ERROR: New queue should be empty\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  if (sys_event_queue_size(&queue) != 0) {
    sys_printf("ERROR: New queue should have size 0\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Test try_pop on empty queue
  sys_event_t event = sys_event_queue_try_pop(&queue);
  if (event != NULL) {
    sys_printf("ERROR: try_pop on empty queue should return NULL\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Push some events
  char *test_data1 = "event1";
  char *test_data2 = "event2";
  char *test_data3 = "event3";

  if (!sys_event_queue_push(&queue, test_data1)) {
    sys_printf("ERROR: Failed to push event1\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  if (!sys_event_queue_try_push(&queue, test_data2)) {
    sys_printf("ERROR: Failed to try_push event2\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  if (!sys_event_queue_push(&queue, test_data3)) {
    sys_printf("ERROR: Failed to push event3\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Check queue state
  if (sys_event_queue_empty(&queue)) {
    sys_printf("ERROR: Queue should not be empty after pushes\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  if (sys_event_queue_size(&queue) != 3) {
    sys_printf("ERROR: Queue should have size 3, got %zu\n",
               sys_event_queue_size(&queue));
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Pop events and verify order
  event = sys_event_queue_try_pop(&queue);
  if (event != test_data1) {
    sys_printf("ERROR: Expected event1, got different event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  event = sys_event_queue_try_pop(&queue);
  if (event != test_data2) {
    sys_printf("ERROR: Expected event2, got different event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Check size after pops
  if (sys_event_queue_size(&queue) != 1) {
    sys_printf("ERROR: Queue should have size 1 after 2 pops, got %zu\n",
               sys_event_queue_size(&queue));
    sys_event_queue_finalize(&queue);
    return 1;
  }

  event = sys_event_queue_try_pop(&queue);
  if (event != test_data3) {
    sys_printf("ERROR: Expected event3, got different event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Queue should be empty again
  if (!sys_event_queue_empty(&queue)) {
    sys_printf("ERROR: Queue should be empty after all pops\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_event_queue_finalize(&queue);
  return 0;
}

/**
 * Test queue overflow behavior (circular buffer with overwrite)
 */
int test_queue_overflow_behavior(void) {
  // Initialize queue with capacity 2
  sys_event_queue_t queue = sys_event_queue_init(2);
  if (queue.items == NULL) {
    sys_printf("ERROR: Failed to initialize event queue\n");
    return 1;
  }

  char *event1 = "overflow1";
  char *event2 = "overflow2";
  char *event3 = "overflow3";
  char *event4 = "overflow4";

  // Fill the queue to capacity
  if (!sys_event_queue_push(&queue, event1)) {
    sys_printf("ERROR: Failed to push first event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  if (!sys_event_queue_push(&queue, event2)) {
    sys_printf("ERROR: Failed to push second event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Queue should be full
  if (sys_event_queue_size(&queue) != 2) {
    sys_printf("ERROR: Queue should be full (size 2), got %zu\n",
               sys_event_queue_size(&queue));
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // try_push should fail on full queue
  if (sys_event_queue_try_push(&queue, event3)) {
    sys_printf("ERROR: try_push should fail on full queue\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // push should succeed and overwrite oldest
  if (!sys_event_queue_push(&queue, event3)) {
    sys_printf("ERROR: push should succeed with overwrite\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Size should still be 2 (capacity)
  if (sys_event_queue_size(&queue) != 2) {
    sys_printf("ERROR: Queue size should remain 2 after overwrite, got %zu\n",
               sys_event_queue_size(&queue));
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Push another to overwrite again
  if (!sys_event_queue_push(&queue, event4)) {
    sys_printf("ERROR: Failed to push fourth event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Now pop - should get event3 and event4 (event1 and event2 were overwritten)
  sys_event_t popped = sys_event_queue_try_pop(&queue);
  if (popped != event3) {
    sys_printf("ERROR: Expected event3 after overwrite, got different event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  popped = sys_event_queue_try_pop(&queue);
  if (popped != event4) {
    sys_printf("ERROR: Expected event4 after overwrite, got different event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_event_queue_finalize(&queue);
  return 0;
}

/**
 * Producer/consumer data for threading test
 */
typedef struct {
  sys_event_queue_t *queue;
  sys_waitgroup_t *wg;
  int producer_id;
  int events_per_producer;
  int *consumer_results;
  int consumer_count;
} producer_data_t;

typedef struct {
  sys_event_queue_t *queue;
  sys_waitgroup_t *wg;
  int consumer_id;
  int *results;
  int expected_events;
} consumer_data_t;

void producer_worker(void *arg) {
  producer_data_t *data = (producer_data_t *)arg;

  for (int i = 0; i < data->events_per_producer; i++) {
    // Create event data (using static string for simplicity)
    char *event_data = (char *)sys_malloc(32);
    if (event_data) {
      sys_sprintf(event_data, 32, "P%d-E%d", data->producer_id, i);
      sys_event_queue_push(data->queue, event_data);
    }

    // Small delay to interleave with other producers
    sys_sleep(1);
  }

  sys_waitgroup_done(data->wg);
}

void consumer_worker(void *arg) {
  consumer_data_t *data = (consumer_data_t *)arg;
  int consumed = 0;

  while (consumed < data->expected_events) {
    sys_event_t event = sys_event_queue_pop(data->queue);
    if (event == NULL) {
      // Queue was shut down
      break;
    }

    // Process event (just count it)
    consumed++;
    sys_free(event); // Free the allocated string
  }

  data->results[data->consumer_id] = consumed;
  sys_waitgroup_done(data->wg);
}

/**
 * Test producer/consumer threading with multiple threads
 */
int test_producer_consumer_threading(void) {
#ifdef SYSTEM_NAME_PICO
  // Simplified test for Pico - single core sequential execution
  sys_printf("Running simplified producer/consumer test (single core)\n");

  const int TOTAL_EVENTS = 10;

  // Initialize queue
  sys_event_queue_t queue = sys_event_queue_init(TOTAL_EVENTS);
  if (queue.items == NULL) {
    sys_printf("ERROR: Failed to initialize event queue\n");
    return 1;
  }

  // Test: Push events (producer role)
  for (int i = 0; i < TOTAL_EVENTS; i++) {
    char *event_data = sys_malloc(50);
    if (!event_data) {
      sys_printf("ERROR: Failed to allocate event data\n");
      sys_event_queue_finalize(&queue);
      return 1;
    }
    sys_sprintf(event_data, 50, "event_%d", i);

    if (!sys_event_queue_push(&queue, event_data)) {
      sys_printf("ERROR: Failed to push event %d\n", i);
      sys_free(event_data);
      sys_event_queue_finalize(&queue);
      return 1;
    }
  }

  sys_printf("Successfully pushed %d events\n", TOTAL_EVENTS);

  // Verify queue size
  if (sys_event_queue_size(&queue) != TOTAL_EVENTS) {
    sys_printf("ERROR: Expected queue size %d, got %zu\n", TOTAL_EVENTS,
               sys_event_queue_size(&queue));
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Test: Pop events (consumer role)
  int consumed = 0;
  while (!sys_event_queue_empty(&queue)) {
    sys_event_t event = sys_event_queue_try_pop(&queue);
    if (event == NULL) {
      break;
    }
    consumed++;
    sys_free(event); // Free the allocated string
  }

  sys_printf("Successfully consumed %d events\n", consumed);

  if (consumed != TOTAL_EVENTS) {
    sys_printf("ERROR: Expected to consume %d events, but consumed %d\n",
               TOTAL_EVENTS, consumed);
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_event_queue_finalize(&queue);
  return 0;

#else
  // Full concurrent test for other platforms
  const int NUM_PRODUCERS = 3;
  const int NUM_CONSUMERS = 2;
  const int EVENTS_PER_PRODUCER = 5;
  const int TOTAL_EVENTS = NUM_PRODUCERS * EVENTS_PER_PRODUCER;

  // Initialize queue
  sys_event_queue_t queue = sys_event_queue_init(10);
  if (queue.items == NULL) {
    sys_printf("ERROR: Failed to initialize event queue\n");
    return 1;
  }

  // Initialize separate waitgroups for producers and consumers
  sys_waitgroup_t producer_wg = sys_waitgroup_init();
  sys_waitgroup_t consumer_wg = sys_waitgroup_init();
  if (!producer_wg.init || !consumer_wg.init) {
    sys_printf("ERROR: Failed to initialize waitgroups\n");
    sys_event_queue_finalize(&queue);
    if (producer_wg.init)
      sys_waitgroup_finalize(&producer_wg);
    if (consumer_wg.init)
      sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }

  // Consumer results array - use malloc for VLA compatibility
  int *consumer_results = (int *)sys_malloc(NUM_CONSUMERS * sizeof(int));
  if (!consumer_results) {
    sys_printf("ERROR: Failed to allocate consumer results\n");
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&producer_wg);
    sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }
  sys_memset(consumer_results, 0, NUM_CONSUMERS * sizeof(int));

  // Add threads to respective waitgroups
  sys_waitgroup_add(&producer_wg, NUM_PRODUCERS);
  sys_waitgroup_add(&consumer_wg, NUM_CONSUMERS);

  // Create producer threads with allocated data
  producer_data_t *producer_data =
      (producer_data_t *)sys_malloc(NUM_PRODUCERS * sizeof(producer_data_t));
  if (!producer_data) {
    sys_printf("ERROR: Failed to allocate producer data\n");
    sys_free(consumer_results);
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&producer_wg);
    sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }

  for (int i = 0; i < NUM_PRODUCERS; i++) {
    producer_data[i].queue = &queue;
    producer_data[i].wg = &producer_wg; // Use producer waitgroup
    producer_data[i].producer_id = i;
    producer_data[i].events_per_producer = EVENTS_PER_PRODUCER;
    producer_data[i].consumer_results = consumer_results;
    producer_data[i].consumer_count = NUM_CONSUMERS;

    if (!sys_thread_create(producer_worker, &producer_data[i])) {
      sys_printf("ERROR: Failed to create producer thread %d\n", i);
      sys_free(producer_data);
      sys_free(consumer_results);
      sys_event_queue_finalize(&queue);
      sys_waitgroup_finalize(&producer_wg);
      sys_waitgroup_finalize(&consumer_wg);
      return 1;
    }
  }

  // Create consumer threads with allocated data
  consumer_data_t *consumer_data =
      (consumer_data_t *)sys_malloc(NUM_CONSUMERS * sizeof(consumer_data_t));
  if (!consumer_data) {
    sys_printf("ERROR: Failed to allocate consumer data\n");
    sys_free(producer_data);
    sys_free(consumer_results);
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&producer_wg);
    sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }

  for (int i = 0; i < NUM_CONSUMERS; i++) {
    consumer_data[i].queue = &queue;
    consumer_data[i].wg = &consumer_wg; // Use consumer waitgroup
    consumer_data[i].consumer_id = i;
    consumer_data[i].results = consumer_results;
    consumer_data[i].expected_events =
        TOTAL_EVENTS; // Let them consume as much as available

    if (!sys_thread_create(consumer_worker, &consumer_data[i])) {
      sys_printf("ERROR: Failed to create consumer thread %d\n", i);
      sys_free(consumer_data);
      sys_free(producer_data);
      sys_free(consumer_results);
      sys_event_queue_finalize(&queue);
      sys_waitgroup_finalize(&producer_wg);
      sys_waitgroup_finalize(&consumer_wg);
      return 1;
    }
  }

  // Wait for all producers to finish first
  sys_waitgroup_finalize(&producer_wg);

  // Give consumers a moment to process remaining events
  sys_sleep(100);

  // Now shut down queue to wake up any waiting consumers
  sys_event_queue_shutdown(&queue);

  // Wait for all consumers to finish
  sys_waitgroup_finalize(&consumer_wg);

  // Verify results
  int total_consumed = 0;
  for (int i = 0; i < NUM_CONSUMERS; i++) {
    total_consumed += consumer_results[i];
    sys_printf("Consumer %d consumed %d events\n", i, consumer_results[i]);
  }

  if (total_consumed != TOTAL_EVENTS) {
    sys_printf("ERROR: Expected %d total events, but consumed %d\n",
               TOTAL_EVENTS, total_consumed);
    sys_free(consumer_data);
    sys_free(producer_data);
    sys_free(consumer_results);
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&producer_wg);
    sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }

  // Cleanup
  sys_free(consumer_data);
  sys_free(producer_data);
  sys_free(consumer_results);
  sys_event_queue_finalize(&queue);
  sys_waitgroup_finalize(&producer_wg);
  sys_waitgroup_finalize(&consumer_wg);
  return 0;
#endif
}

/**
 * Test peek before acquire functionality
 */
int test_peek_before_acquire(void) {
  // Initialize queue
  sys_event_queue_t queue = sys_event_queue_init(5);
  if (queue.items == NULL) {
    sys_printf("ERROR: Failed to initialize event queue\n");
    return 1;
  }

  char *event1 = "peek_event1";
  char *event2 = "peek_event2";

  // Peek on empty queue
  if (!sys_event_queue_lock(&queue)) {
    sys_printf("ERROR: Failed to lock queue\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_event_t peeked = sys_event_queue_peek(&queue);
  if (peeked != NULL) {
    sys_printf("ERROR: Peek on empty queue should return NULL\n");
    sys_event_queue_unlock(&queue);
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_event_queue_unlock(&queue);

  // Push some events
  sys_event_queue_push(&queue, event1);
  sys_event_queue_push(&queue, event2);

  // Check size before locking
  size_t size_before = sys_event_queue_size(&queue);
  if (size_before != 2) {
    sys_printf("ERROR: Queue should have 2 events before peek\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Peek at first event
  if (!sys_event_queue_lock(&queue)) {
    sys_printf("ERROR: Failed to lock queue\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  peeked = sys_event_queue_peek(&queue);
  if (peeked != event1) {
    sys_printf("ERROR: Peek should return first event\n");
    sys_event_queue_unlock(&queue);
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Peek again - should return same event
  sys_event_t peeked2 = sys_event_queue_peek(&queue);
  if (peeked2 != event1) {
    sys_printf("ERROR: Second peek should return same event\n");
    sys_event_queue_unlock(&queue);
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_event_queue_unlock(&queue);

  // Check size after peeking - should be unchanged
  size_t size_after = sys_event_queue_size(&queue);
  if (size_after != size_before) {
    sys_printf("ERROR: Peek should not change queue size (was %zu, now %zu)\n",
               size_before, size_after);
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Now pop the first event
  sys_event_t popped = sys_event_queue_try_pop(&queue);
  if (popped != event1) {
    sys_printf("ERROR: Pop should return the peeked event\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Peek at next event
  if (!sys_event_queue_lock(&queue)) {
    sys_printf("ERROR: Failed to lock queue\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  peeked = sys_event_queue_peek(&queue);
  if (peeked != event2) {
    sys_printf("ERROR: Peek should now return second event\n");
    sys_event_queue_unlock(&queue);
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_event_queue_unlock(&queue);

  sys_event_queue_finalize(&queue);
  return 0;
}

/**
 * Test queue shutdown functionality
 */
int test_queue_shutdown(void) {
  // Initialize queue
  sys_event_queue_t queue = sys_event_queue_init(5);
  if (queue.items == NULL) {
    sys_printf("ERROR: Failed to initialize event queue\n");
    return 1;
  }

  char *event1 = "shutdown_event1";
  char *event2 = "shutdown_event2";

  // Add some events
  sys_event_queue_push(&queue, event1);
  sys_event_queue_push(&queue, event2);

  // Verify events are there
  if (sys_event_queue_size(&queue) != 2) {
    sys_printf("ERROR: Queue should have 2 events before shutdown\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Shutdown the queue
  sys_event_queue_shutdown(&queue);

  // Should be able to pop existing events using try_pop (now that it's fixed)
  sys_event_t popped = sys_event_queue_try_pop(&queue);
  if (popped != event1) {
    sys_printf("ERROR: Should be able to pop existing events after shutdown\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  popped = sys_event_queue_try_pop(&queue);
  if (popped != event2) {
    sys_printf(
        "ERROR: Should be able to pop second existing event after shutdown\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Now queue should be empty and try_pop should return NULL
  popped = sys_event_queue_try_pop(&queue);
  if (popped != NULL) {
    sys_printf("ERROR: try_pop on empty shutdown queue should return NULL\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  // Test that blocking pop also returns immediately on empty shutdown queue
  popped = sys_event_queue_pop(&queue);
  if (popped != NULL) {
    sys_printf(
        "ERROR: Blocking pop on empty shutdown queue should return NULL\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_event_queue_finalize(&queue);
  return 0;
}

/**
 * High volume stress test: Many producers, many consumers, many events
 */
int test_high_volume_stress(void) {
  const int NUM_PRODUCERS = 8;         // More producers
  const int NUM_CONSUMERS = 4;         // More consumers
  const int EVENTS_PER_PRODUCER = 100; // Many more events per producer
  const int TOTAL_EVENTS = NUM_PRODUCERS * EVENTS_PER_PRODUCER;

  sys_printf("High volume test: %d producers × %d events = %d total events, %d "
             "consumers\n",
             NUM_PRODUCERS, EVENTS_PER_PRODUCER, TOTAL_EVENTS, NUM_CONSUMERS);

  // Use smaller queue to force more contention and overwrite scenarios
  sys_event_queue_t queue = sys_event_queue_init(20);
  if (queue.items == NULL) {
    sys_printf("ERROR: Failed to initialize event queue\n");
    return 1;
  }

  // Initialize waitgroups
  sys_waitgroup_t producer_wg = sys_waitgroup_init();
  sys_waitgroup_t consumer_wg = sys_waitgroup_init();
  if (!producer_wg.init || !consumer_wg.init) {
    sys_printf("ERROR: Failed to initialize waitgroups\n");
    sys_event_queue_finalize(&queue);
    if (producer_wg.init)
      sys_waitgroup_finalize(&producer_wg);
    if (consumer_wg.init)
      sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }

  // Consumer results array
  int *consumer_results = (int *)sys_malloc(NUM_CONSUMERS * sizeof(int));
  if (!consumer_results) {
    sys_printf("ERROR: Failed to allocate consumer results\n");
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&producer_wg);
    sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }
  sys_memset(consumer_results, 0, NUM_CONSUMERS * sizeof(int));

  sys_waitgroup_add(&producer_wg, NUM_PRODUCERS);
  sys_waitgroup_add(&consumer_wg, NUM_CONSUMERS);

  // Create producer data
  producer_data_t *producer_data =
      (producer_data_t *)sys_malloc(NUM_PRODUCERS * sizeof(producer_data_t));
  if (!producer_data) {
    sys_printf("ERROR: Failed to allocate producer data\n");
    sys_free(consumer_results);
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&producer_wg);
    sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }

  // Start producers
  for (int i = 0; i < NUM_PRODUCERS; i++) {
    producer_data[i].queue = &queue;
    producer_data[i].wg = &producer_wg;
    producer_data[i].producer_id = i;
    producer_data[i].events_per_producer = EVENTS_PER_PRODUCER;
    producer_data[i].consumer_results = consumer_results;
    producer_data[i].consumer_count = NUM_CONSUMERS;

    if (!sys_thread_create(producer_worker, &producer_data[i])) {
      sys_printf("ERROR: Failed to create producer thread %d\n", i);
      sys_free(producer_data);
      sys_free(consumer_results);
      sys_event_queue_finalize(&queue);
      sys_waitgroup_finalize(&producer_wg);
      sys_waitgroup_finalize(&consumer_wg);
      return 1;
    }
  }

  // Create consumer data
  consumer_data_t *consumer_data =
      (consumer_data_t *)sys_malloc(NUM_CONSUMERS * sizeof(consumer_data_t));
  if (!consumer_data) {
    sys_printf("ERROR: Failed to allocate consumer data\n");
    sys_free(producer_data);
    sys_free(consumer_results);
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&producer_wg);
    sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }

  // Start consumers
  for (int i = 0; i < NUM_CONSUMERS; i++) {
    consumer_data[i].queue = &queue;
    consumer_data[i].wg = &consumer_wg;
    consumer_data[i].consumer_id = i;
    consumer_data[i].results = consumer_results;
    consumer_data[i].expected_events = TOTAL_EVENTS;

    if (!sys_thread_create(consumer_worker, &consumer_data[i])) {
      sys_printf("ERROR: Failed to create consumer thread %d\n", i);
      sys_free(consumer_data);
      sys_free(producer_data);
      sys_free(consumer_results);
      sys_event_queue_finalize(&queue);
      sys_waitgroup_finalize(&producer_wg);
      sys_waitgroup_finalize(&consumer_wg);
      return 1;
    }
  }

  // Wait for producers to finish
  sys_waitgroup_finalize(&producer_wg);

  // Give consumers time to process
  sys_sleep(200);

  // Shutdown queue
  sys_event_queue_shutdown(&queue);

  // Wait for consumers
  sys_waitgroup_finalize(&consumer_wg);

  // Verify results - with overwrite semantics, we may not get all events
  int total_consumed = 0;
  for (int i = 0; i < NUM_CONSUMERS; i++) {
    total_consumed += consumer_results[i];
  }

  int percentage = (total_consumed * 100) / TOTAL_EVENTS;
  sys_printf("High volume results: %d/%d events consumed (%d%%)\n",
             total_consumed, TOTAL_EVENTS, percentage);

  // With a small queue and many producers, some events will be overwritten
  // Just verify we consumed a reasonable number (at least 80% due to overwrite)
  int min_expected = (TOTAL_EVENTS * 80) / 100; // 80% of total
  if (total_consumed < min_expected) {
    sys_printf("ERROR: Too few events consumed: %d < %d (80%% of total)\n",
               total_consumed, min_expected);
    sys_free(consumer_data);
    sys_free(producer_data);
    sys_free(consumer_results);
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&producer_wg);
    sys_waitgroup_finalize(&consumer_wg);
    return 1;
  }

  // Cleanup
  sys_free(consumer_data);
  sys_free(producer_data);
  sys_free(consumer_results);
  sys_event_queue_finalize(&queue);
  sys_waitgroup_finalize(&producer_wg);
  sys_waitgroup_finalize(&consumer_wg);
  return 0;
}

/**
 * Worker for rapid shutdown test
 */
void shutdown_stress_worker(void *arg) {
  sys_event_queue_t *queue = (sys_event_queue_t *)arg;

  // Rapidly push and pop events while other threads might be shutting down
  for (int i = 0; i < 50; i++) {
    char *event_data = (char *)sys_malloc(16);
    if (event_data) {
      sys_sprintf(event_data, 16, "rapid_%d", i);
      sys_event_queue_push(queue, event_data);
    }

    // Try to pop immediately
    sys_event_t popped = sys_event_queue_try_pop(queue);
    if (popped) {
      sys_free(popped);
    }

    // No sleep - maximum stress
  }
}

/**
 * Rapid shutdown stress test: Test shutdown during high activity
 */
int test_rapid_shutdown_stress(void) {
  const int NUM_THREADS = 6;
  const int NUM_ITERATIONS = 10; // Run multiple iterations

  sys_printf("Rapid shutdown test: %d threads, %d iterations\n", NUM_THREADS,
             NUM_ITERATIONS);

  for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
    sys_event_queue_t queue = sys_event_queue_init(10);
    if (queue.items == NULL) {
      sys_printf("ERROR: Failed to initialize event queue (iteration %d)\n",
                 iter);
      return 1;
    }

    // Start worker threads
    for (int i = 0; i < NUM_THREADS; i++) {
      if (!sys_thread_create(shutdown_stress_worker, &queue)) {
        sys_printf("ERROR: Failed to create worker thread %d (iteration %d)\n",
                   i, iter);
        sys_event_queue_finalize(&queue);
        return 1;
      }
    }

    // Let them run briefly
    sys_sleep(5);

    // Shutdown while threads are active
    sys_event_queue_shutdown(&queue);

    // Give threads time to notice shutdown and finish
    sys_sleep(20);

    sys_event_queue_finalize(&queue);
  }

  return 0;
}

/**
 * Worker for mixed operations test
 */
typedef struct {
  sys_event_queue_t *queue;
  sys_waitgroup_t *wg;
  int worker_id;
  int *operation_counts; // [pushes, pops, peeks, try_pushes, try_pops]
} mixed_worker_data_t;

void mixed_operations_worker(void *arg) {
  mixed_worker_data_t *data = (mixed_worker_data_t *)arg;
  int pushes = 0, pops = 0, peeks = 0, try_pushes = 0, try_pops = 0;

  // Perform random mix of operations
  for (int i = 0; i < 100; i++) {
    int operation = i % 7; // 7 different operations

    switch (operation) {
    case 0: // push
    case 1: {
      char *event_data = (char *)sys_malloc(24);
      if (event_data) {
        sys_sprintf(event_data, 24, "W%d_%d", data->worker_id, i);
        if (sys_event_queue_push(data->queue, event_data)) {
          pushes++;
        } else {
          sys_free(event_data);
        }
      }
      break;
    }

    case 2: { // try_push
      char *event_data = (char *)sys_malloc(24);
      if (event_data) {
        sys_sprintf(event_data, 24, "TW%d_%d", data->worker_id, i);
        if (sys_event_queue_try_push(data->queue, event_data)) {
          try_pushes++;
        } else {
          sys_free(event_data);
        }
      }
      break;
    }

    case 3: { // try_pop
      sys_event_t event = sys_event_queue_try_pop(data->queue);
      if (event) {
        try_pops++;
        sys_free(event);
      }
      break;
    }

    case 4: { // pop with timeout (using try_pop for simplicity)
      sys_event_t event = sys_event_queue_try_pop(data->queue);
      if (event) {
        pops++;
        sys_free(event);
      }
      break;
    }

    case 5: { // peek operation
      if (sys_event_queue_lock(data->queue)) {
        sys_event_t event = sys_event_queue_peek(data->queue);
        if (event) {
          peeks++;
        }
        sys_event_queue_unlock(data->queue);
      }
      break;
    }

    case 6: { // check size/empty
      sys_event_queue_size(data->queue);
      sys_event_queue_empty(data->queue);
      break;
    }
    }

    // Small random delay to create more realistic contention
    if (i % 10 == 0) {
      sys_sleep(1);
    }
  }

  // Store operation counts
  data->operation_counts[0] = pushes;
  data->operation_counts[1] = pops;
  data->operation_counts[2] = peeks;
  data->operation_counts[3] = try_pushes;
  data->operation_counts[4] = try_pops;

  sys_waitgroup_done(data->wg);
}

/**
 * Mixed operations stress test: All operations concurrently
 */
int test_mixed_operations_stress(void) {
  const int NUM_WORKERS = 6;

  sys_printf("Mixed operations test: %d workers performing all operations "
             "concurrently\n",
             NUM_WORKERS);

  sys_event_queue_t queue = sys_event_queue_init(15);
  if (queue.items == NULL) {
    sys_printf("ERROR: Failed to initialize event queue\n");
    return 1;
  }

  sys_waitgroup_t wg = sys_waitgroup_init();
  if (!wg.init) {
    sys_printf("ERROR: Failed to initialize waitgroup\n");
    sys_event_queue_finalize(&queue);
    return 1;
  }

  sys_waitgroup_add(&wg, NUM_WORKERS);

  // Allocate worker data and operation counts
  mixed_worker_data_t *workers = (mixed_worker_data_t *)sys_malloc(
      NUM_WORKERS * sizeof(mixed_worker_data_t));
  int *all_counts =
      (int *)sys_malloc(NUM_WORKERS * 5 * sizeof(int)); // 5 counts per worker

  if (!workers || !all_counts) {
    sys_printf("ERROR: Failed to allocate worker data\n");
    if (workers)
      sys_free(workers);
    if (all_counts)
      sys_free(all_counts);
    sys_event_queue_finalize(&queue);
    sys_waitgroup_finalize(&wg);
    return 1;
  }

  sys_memset(all_counts, 0, NUM_WORKERS * 5 * sizeof(int));

  // Start workers
  for (int i = 0; i < NUM_WORKERS; i++) {
    workers[i].queue = &queue;
    workers[i].wg = &wg;
    workers[i].worker_id = i;
    workers[i].operation_counts = &all_counts[i * 5];

    if (!sys_thread_create(mixed_operations_worker, &workers[i])) {
      sys_printf("ERROR: Failed to create worker thread %d\n", i);
      sys_free(workers);
      sys_free(all_counts);
      sys_event_queue_finalize(&queue);
      sys_waitgroup_finalize(&wg);
      return 1;
    }
  }

  // Wait for all workers to complete
  sys_waitgroup_finalize(&wg);

  // Collect and report statistics
  int total_pushes = 0, total_pops = 0, total_peeks = 0, total_try_pushes = 0,
      total_try_pops = 0;

  for (int i = 0; i < NUM_WORKERS; i++) {
    total_pushes += workers[i].operation_counts[0];
    total_pops += workers[i].operation_counts[1];
    total_peeks += workers[i].operation_counts[2];
    total_try_pushes += workers[i].operation_counts[3];
    total_try_pops += workers[i].operation_counts[4];
  }

  sys_printf("Operations completed - Push: %d, Pop: %d, Peek: %d, TryPush: %d, "
             "TryPop: %d\n",
             total_pushes, total_pops, total_peeks, total_try_pushes,
             total_try_pops);

  // Final queue state
  size_t final_size = sys_event_queue_size(&queue);
  sys_printf("Final queue size: %zu\n", final_size);

  // Clean up remaining events
  while (!sys_event_queue_empty(&queue)) {
    sys_event_t event = sys_event_queue_try_pop(&queue);
    if (event) {
      sys_free(event);
    } else {
      break;
    }
  }

  // Cleanup
  sys_free(workers);
  sys_free(all_counts);
  sys_event_queue_finalize(&queue);
  sys_waitgroup_finalize(&wg);
  return 0;
}
