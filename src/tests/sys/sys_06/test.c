#include <runtime-sys/sys.h>
#include <runtime-sys/timer.h>
#include <string.h>
#include <tests/tests.h>

// Test callback functions
static volatile int callback_count = 0;
static volatile sys_timer_t *callback_timer = NULL;
static volatile void *received_userdata = NULL;

static void simple_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
}

static void userdata_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
  received_userdata = timer->userdata;
}

static void oneshot_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
  // Make it one-shot by finalizing immediately
  sys_timer_finalize(timer);
}

static void periodic_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
  // Let it continue running for periodic behavior
}

static void delayed_oneshot_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
  // Only finalize after 3 calls to test delayed one-shot
  if (callback_count >= 3) {
    sys_timer_finalize(timer);
  }
}

static void counting_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
  // Used for precise counting tests
}

int test_sys_06(void) {
  sys_printf("test_sys_06\n");

  // Reset callback state
  callback_count = 0;
  callback_timer = NULL;
  received_userdata = NULL;

  sys_printf("Test 1: Timer initialization with basic parameters\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, simple_callback);
    // Timer was never started, so finalize should return false
    test_assert(sys_timer_finalize(&timer) == false);
  }
  sys_printf("  ✓ Basic timer initialization works\n");

  sys_printf("Test 2: Timer initialization with userdata\n");
  {
    int test_data = 42;
    callback_count = 0;
    received_userdata = NULL;

    sys_timer_t timer = sys_timer_init(50, &test_data, userdata_callback);
    test_assert(sys_timer_start(&timer) == true);

    // Wait for callback to execute
    sys_sleep(100);

    test_assert(callback_count > 0); // Callback should have been called
    test_assert(received_userdata == &test_data); // Userdata should match
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf(
      "  ✓ Timer initialization with userdata works and callback executes\n");

  sys_printf("Test 3: Timer initialization with NULL callback\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, NULL);
    // Invalid timer should have NULL context, so finalize should return false
    test_assert(sys_timer_finalize(&timer) == false);
  }
  sys_printf("  ✓ Timer initialization with NULL callback works\n");

  sys_printf("Test 4: Timer start and callback execution\n");
  {
    callback_count = 0;
    sys_timer_t timer = sys_timer_init(30, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);

    // Wait for callback to execute
    sys_sleep(100);

    test_assert(callback_count > 0); // Callback should have been called
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Timer start and callback execution works\n");

  sys_printf("Test 5: Multiple timer operations\n");
  {
    callback_count = 0;
    sys_timer_t timer1 = sys_timer_init(25, NULL, simple_callback);
    sys_timer_t timer2 = sys_timer_init(40, NULL, userdata_callback);

    test_assert(sys_timer_start(&timer1) == true);
    test_assert(sys_timer_start(&timer2) == true);

    // Wait for callbacks to execute
    sys_sleep(100);

    test_assert(callback_count > 0); // At least one callback should have fired

    test_assert(sys_timer_finalize(&timer1) == true);
    test_assert(sys_timer_finalize(&timer2) == true);
  }
  sys_printf("  ✓ Multiple timer operations work and callbacks execute\n");

  sys_printf("Test 6: Timer with very short interval\n");
  {
    callback_count = 0;
    sys_timer_t timer = sys_timer_init(10, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);

    sys_sleep(50); // Give it time to fire multiple times

    test_assert(callback_count > 0); // Should have fired at least once
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Very short interval timer works and fires callbacks\n");

  sys_printf("Test 7: Timer with long interval\n");
  {
    sys_timer_t timer = sys_timer_init(5000, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    // Don't wait for it to fire, just clean up
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Long interval timer works\n");

  sys_printf("Test 8: One-shot timer behavior (immediate finalization)\n");
  {
    callback_count = 0;
    sys_timer_t timer = sys_timer_init(20, NULL, oneshot_callback);
    test_assert(sys_timer_start(&timer) == true);

    // Wait for the timer to fire and self-destruct
    sys_sleep(100);

    // The callback should have been called exactly once (one-shot)
    test_assert(callback_count == 1);

    // Timer should already be finalized by the callback
    // Trying to finalize again should return false
    test_assert(sys_timer_finalize(&timer) == false);
  }
  sys_printf("  ✓ One-shot timer behavior works (immediate finalization in "
             "callback)\n");

  sys_printf("Test 9: One-shot timer behavior (delayed finalization)\n");
  {
    callback_count = 0;
    sys_timer_t timer = sys_timer_init(15, NULL, delayed_oneshot_callback);
    test_assert(sys_timer_start(&timer) == true);

    // Wait for the timer to fire 3 times before self-destructing
    sys_sleep(100);

    // The callback should have been called exactly 3 times before finalizing
    test_assert(callback_count == 3);

    // Timer should already be finalized by the callback
    test_assert(sys_timer_finalize(&timer) == false);
  }
  sys_printf("  ✓ One-shot timer behavior works (delayed finalization after N "
             "calls)\n");

  sys_printf("Test 10: Periodic timer behavior (precise counting)\n");
  {
    callback_count = 0;
    sys_timer_t timer = sys_timer_init(20, NULL, counting_callback);
    test_assert(sys_timer_start(&timer) == true);

    // Wait for exactly 100ms (should fire ~5 times at 20ms intervals)
    sys_sleep(100);

    int first_count = callback_count;
    test_assert(first_count >= 3 && first_count <= 7); // Should fire 3-7 times

    // Wait another 100ms to verify it keeps running
    sys_sleep(100);

    int second_count = callback_count;
    test_assert(second_count > first_count); // Should have fired more times
    test_assert(second_count >= 6); // Should have at least 6 total calls

    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf(
      "  ✓ Periodic timer behavior works (continuous firing verified)\n");

  sys_printf("Test 11: Periodic timer behavior (long duration)\n");
  {
    callback_count = 0;
    sys_timer_t timer = sys_timer_init(30, NULL, periodic_callback);
    test_assert(sys_timer_start(&timer) == true);

    // Wait for multiple periods over longer duration
    sys_sleep(200);

    // Should have fired multiple times (periodic)
    test_assert(callback_count >= 5); // Should fire at least 5-6 times in 200ms

    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Periodic timer behavior works (long duration test)\n");

  sys_printf("Test 12: Error handling - NULL timer start\n");
  {
    test_assert(sys_timer_start(NULL) == false);
  }
  sys_printf("  ✓ NULL timer start handling works\n");

  sys_printf("Test 13: Error handling - NULL timer finalize\n");
  {
    test_assert(sys_timer_finalize(NULL) == false);
  }
  sys_printf("  ✓ NULL timer finalize handling works\n");

  sys_printf("Test 14: Timer restart after finalization\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    test_assert(sys_timer_finalize(&timer) == true);

    // Try to start again after finalization - should work with new design
    bool restart_result = sys_timer_start(&timer);
    test_assert(restart_result ==
                true); // Should succeed - creates new dispatch source

    // Clean up
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Timer restart after finalization works (creates new dispatch "
             "source)\n");

  sys_printf("Test 15: Rapid start/stop cycles\n");
  {
    callback_count = 0;

    for (int i = 0; i < 3; i++) {
      sys_timer_t timer = sys_timer_init(20, NULL, simple_callback);
      test_assert(sys_timer_start(&timer) == true);
      sys_sleep(60); // Longer pause to ensure callback fires
      test_assert(sys_timer_finalize(&timer) == true);
    }

    test_assert(callback_count >
                0); // At least some callbacks should have fired
  }
  sys_printf("  ✓ Rapid start/stop cycles work\n");

  sys_printf("Test 16: Edge case - zero interval\n");
  {
    sys_timer_t timer = sys_timer_init(0, NULL, simple_callback);
    // Zero interval is invalid, so start should return false
    test_assert(sys_timer_start(&timer) == false);
    test_assert(sys_timer_finalize(&timer) == false);
  }
  sys_printf("  ✓ Zero interval timer handling works\n");

  sys_printf("Test 17: Large interval value\n");
  {
    sys_timer_t timer = sys_timer_init(UINT32_MAX, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Large interval timer handling works\n");

  sys_printf("Test 18: Timer state consistency\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, simple_callback);

    // Multiple starts should return false for the second attempt
    test_assert(sys_timer_start(&timer) == true);
    bool second_start = sys_timer_start(&timer);
    test_assert(second_start == false); // Should fail since already running

    // Multiple finalizes should work - first succeeds, second fails
    test_assert(sys_timer_finalize(&timer) == true);
    bool second_finalize = sys_timer_finalize(&timer);
    test_assert(second_finalize ==
                false); // Should fail since already finalized
  }
  sys_printf("  ✓ Timer state consistency tested\n");

  sys_printf("Test 19: Periodic to one-shot transition\n");
  {
    callback_count = 0;
    sys_timer_t timer = sys_timer_init(25, NULL, periodic_callback);
    test_assert(sys_timer_start(&timer) == true);

    // Let it run periodically for a while
    sys_sleep(80);

    int periodic_count = callback_count;
    test_assert(periodic_count >= 2); // Should have fired multiple times

    // Now finalize it externally (transition to "one-shot" behavior)
    test_assert(sys_timer_finalize(&timer) == true);

    // Wait a bit more - should not fire anymore
    sys_sleep(60);

    int final_count = callback_count;
    test_assert(final_count ==
                periodic_count); // Count should not increase after finalization
  }
  sys_printf(
      "  ✓ Periodic to one-shot transition works (external finalization)\n");

  sys_printf("All timer function tests completed successfully!\n");
  return 0;
}
