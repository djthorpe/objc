#include <runtime-sys/sys.h>
#include <tests/tests.h>

// Global event queues for multicore testing
static sys_event_queue_t g_core0_to_core1_queue; // Core 0 → Core 1
static sys_event_queue_t g_core1_to_core0_queue; // Core 1 → Core 0
static volatile int g_events_received = 0;
static volatile int g_events_sent = 0;
static volatile bool g_core1_ready = false;

// Core 1 entry point - consumer thread
void core1_entry(void *arg) {
  (void)arg; // Unused parameter
  sys_printf("Core 1: Starting event consumer\n");
  g_core1_ready = true;

  // Consumer: receive events from core 0
  for (int i = 0; i < 5; i++) {
    sys_event_t event = sys_event_queue_timed_pop(&g_core0_to_core1_queue,
                                                  1000); // 1 second timeout

    if (event != NULL) {
      // For testing, we'll treat the event as an integer value
      int event_value = (int)(uintptr_t)event;
      sys_printf("Core 1: Received event value=%d\n", event_value);
      g_events_received++;

      // Send a response event back to core 0 via the response queue
      sys_event_t response = (sys_event_t)(uintptr_t)(event_value + 1000);
      sys_event_queue_push(&g_core1_to_core0_queue, response);
    } else {
      sys_printf("Core 1: Timeout waiting for event %d\n", i);
      break;
    }
  }

  sys_printf("Core 1: Consumer finished, received %d events\n",
             g_events_received);
}

// Forward declarations
int test_event_queue_init(void);
int test_event_queue_basic(void);
int test_event_queue_multicore(void);
int test_event_queue_timeout(void);
int test_event_queue_error_handling(void);

// Test 1: Event queue initialization
int test_event_queue_init(void) {
  sys_printf("Test 1: Event queue initialization\n");

  sys_event_queue_t queue = sys_event_queue_init(10); // capacity = 10
  if (queue.items != NULL) {
    sys_printf("  ✓ Event queue initialization works\n");
    sys_event_queue_finalize(&queue);
    return 1;
  } else {
    sys_printf("  ✗ Event queue initialization failed\n");
    return 0;
  }
}

// Test 2: Basic single-threaded event push and pop
int test_event_queue_basic(void) {
  sys_printf("Test 2: Basic event push and pop (single-threaded)\n");

  sys_event_queue_t queue = sys_event_queue_init(10);
  if (queue.items != NULL) {
    // Push an event (simple integer value as pointer)
    sys_event_t test_event = (sys_event_t)(uintptr_t)42;
    bool pushed = sys_event_queue_push(&queue, test_event);

    if (pushed) {
      // Try to pop the event (with short timeout)
      sys_event_t retrieved_event =
          sys_event_queue_timed_pop(&queue, 10); // 10ms timeout

      if (retrieved_event != NULL && (int)(uintptr_t)retrieved_event == 42) {
        sys_printf("  ✓ Event push and pop works\n");
        sys_event_queue_finalize(&queue);
        return 1;
      } else {
        sys_printf("  ✗ Event push and pop failed - retrieved wrong data\n");
      }
    } else {
      sys_printf("  ✗ Event push failed\n");
    }

    sys_event_queue_finalize(&queue);
  } else {
    sys_printf("  ✗ Failed to initialize event queue\n");
  }
  return 0;
}

// Test 3: Multicore event communication (Producer-Consumer)
int test_event_queue_multicore(void) {
  sys_printf("Test 3: Multicore event communication\n");

  // Initialize separate queues for bidirectional communication
  g_core0_to_core1_queue = sys_event_queue_init(10); // Core 0 → Core 1
  g_core1_to_core0_queue = sys_event_queue_init(10); // Core 1 → Core 0
  g_events_received = 0;
  g_events_sent = 0;
  g_core1_ready = false;

  if (g_core0_to_core1_queue.items != NULL &&
      g_core1_to_core0_queue.items != NULL) {
    // Start core 1 as consumer
    bool core1_started = sys_thread_create_on_core(core1_entry, NULL, 1);
    if (!core1_started) {
      sys_printf("  ✗ Failed to start thread on core 1\n");
      sys_event_queue_finalize(&g_core0_to_core1_queue);
      sys_event_queue_finalize(&g_core1_to_core0_queue);
      return 0;
    }

    // Wait for core 1 to be ready
    while (!g_core1_ready) {
      sys_sleep(1);
    }
    sys_printf("Core 0: Core 1 is ready, starting producer\n");

    // Core 0: Producer - send events to core 1
    bool all_sent = true;
    for (int i = 0; i < 5; i++) {
      sys_event_t event =
          (sys_event_t)(uintptr_t)(i + 10); // Simple integer values
      bool sent = sys_event_queue_push(&g_core0_to_core1_queue, event);
      if (sent) {
        g_events_sent++;
        sys_printf("Core 0: Sent event value=%d\n", i + 10);
      } else {
        all_sent = false;
        sys_printf("Core 0: Failed to send event %d\n", i);
      }
      sys_sleep(100); // Small delay between sends
    }

    // Core 0: Now receive response events from core 1
    int responses_received = 0;
    for (int i = 0; i < 5; i++) {
      sys_event_t response = sys_event_queue_timed_pop(
          &g_core1_to_core0_queue, 1000); // 1 second timeout
      if (response != NULL) {
        int response_value = (int)(uintptr_t)response;
        sys_printf("Core 0: Received response value=%d\n", response_value);
        responses_received++;
      } else {
        sys_printf("Core 0: Timeout waiting for response %d\n", i);
        break;
      }
    }

    // Wait for core 1 thread to finish
    sys_sleep(500); // Give core 1 time to finish processing

    if (all_sent && g_events_sent == 5 && g_events_received == 5 &&
        responses_received == 5) {
      sys_printf("  ✓ Multicore event communication works\n");
      sys_event_queue_finalize(&g_core0_to_core1_queue);
      sys_event_queue_finalize(&g_core1_to_core0_queue);
      return 1;
    } else {
      sys_printf("  ✗ Multicore event communication failed (sent:%d, "
                 "received:%d, responses:%d)\n",
                 g_events_sent, g_events_received, responses_received);
    }

    sys_event_queue_finalize(&g_core0_to_core1_queue);
    sys_event_queue_finalize(&g_core1_to_core0_queue);
  } else {
    sys_printf("  ✗ Failed to initialize event queues\n");
  }
  return 0;
}

// Test 4: Timeout behavior
int test_event_queue_timeout(void) {
  sys_printf("Test 4: Timeout behavior\n");

  sys_event_queue_t queue = sys_event_queue_init(10);
  if (queue.items != NULL) {
    // Try to pop from empty queue with short timeout
    sys_event_t event = sys_event_queue_timed_pop(&queue, 10); // 10ms timeout

    if (event == NULL) {
      sys_printf("  ✓ Timeout behavior works correctly\n");
      sys_event_queue_finalize(&queue);
      return 1;
    } else {
      sys_printf("  ✗ Timeout did not occur as expected\n");
    }

    sys_event_queue_finalize(&queue);
  } else {
    sys_printf("  ✗ Failed to initialize event queue\n");
  }
  return 0;
}

// Test 5: Error handling
int test_event_queue_error_handling(void) {
  sys_printf("Test 5: Error handling\n");

  sys_event_t dummy_event = (sys_event_t)(uintptr_t)1;

  // Test with NULL queue
  bool null_push = sys_event_queue_push(NULL, dummy_event);
  sys_event_t null_pop = sys_event_queue_timed_pop(NULL, 10);

  if (!null_push && null_pop == NULL) {
    sys_printf("  ✓ NULL queue handling works\n");
    return 1;
  } else {
    sys_printf("  ✗ NULL queue handling failed\n");
    return 0;
  }
}

int test_sys_13() {
  int test_count = 5; // Total number of tests
  int pass_count = 0;

  sys_printf("=== Event Queue Test Suite ===\n");

  // Run each test individually for debugging
  pass_count += test_event_queue_init();
  pass_count += test_event_queue_basic();
  pass_count += test_event_queue_multicore();
  pass_count += test_event_queue_timeout();
  pass_count += test_event_queue_error_handling();

  sys_printf("=== Test Results ===\n");
  sys_printf("Passed: %d/%d tests\n", pass_count, test_count);

  return (pass_count == test_count) ? 0 : 1;
}
