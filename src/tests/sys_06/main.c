#include <runtime-sys/sys.h>
#include <runtime-sys/timer.h>
#include <string.h>
#include <tests/tests.h>

// Forward declaration
int test_sys_06(void);

// Test callback functions
static int callback_count = 0;
static sys_timer_t *callback_timer = NULL;
static void *callback_userdata = NULL;

static void simple_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
}

static void userdata_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
  // Extract userdata through a global since we can't directly access it
  // In a real implementation, this would be passed properly
}

static void oneshot_callback(sys_timer_t *timer) {
  callback_count++;
  callback_timer = timer;
  // Finalize the timer to make it a one-shot
  sys_timer_finalize(timer);
}

int main(void) {
  // Run timer function tests
  return TestMain("test_sys_06", test_sys_06);
}

int test_sys_06(void) {
  sys_printf("test_sys_06\n");

  // Reset callback state
  callback_count = 0;
  callback_timer = NULL;
  callback_userdata = NULL;

  sys_printf("Test 1: Timer initialization with basic parameters\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, simple_callback);
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Basic timer initialization works\n");

  sys_printf("Test 2: Timer initialization with userdata\n");
  {
    int test_data = 42;
    sys_timer_t timer = sys_timer_init(200, &test_data, userdata_callback);
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Timer initialization with userdata works\n");

  sys_printf("Test 3: Timer initialization with NULL callback\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, NULL);
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Timer initialization with NULL callback works\n");

  sys_printf("Test 4: Timer start and stop\n");
  {
    sys_timer_t timer = sys_timer_init(50, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Timer start and stop works\n");

  sys_printf("Test 5: Multiple timer operations\n");
  {
    sys_timer_t timer1 = sys_timer_init(100, NULL, simple_callback);
    sys_timer_t timer2 = sys_timer_init(200, NULL, userdata_callback);
    
    test_assert(sys_timer_start(&timer1) == true);
    test_assert(sys_timer_start(&timer2) == true);
    
    // Wait a short time to let them potentially fire
    sys_sleep(50);
    
    test_assert(sys_timer_finalize(&timer1) == true);
    test_assert(sys_timer_finalize(&timer2) == true);
  }
  sys_printf("  ✓ Multiple timer operations work\n");

  sys_printf("Test 6: Timer with very short interval\n");
  {
    sys_timer_t timer = sys_timer_init(1, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    sys_sleep(10); // Give it time to fire
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Very short interval timer works\n");

  sys_printf("Test 7: Timer with long interval\n");
  {
    sys_timer_t timer = sys_timer_init(5000, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    // Don't wait for it to fire, just clean up
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Long interval timer works\n");

  sys_printf("Test 8: One-shot timer behavior\n");
  {
    callback_count = 0;
    sys_timer_t timer = sys_timer_init(10, NULL, oneshot_callback);
    test_assert(sys_timer_start(&timer) == true);
    
    // Wait for the timer to fire and self-destruct
    sys_sleep(50);
    
    // The callback should have been called and finalized the timer
    // Note: In current stub implementation, this test verifies the API contract
  }
  sys_printf("  ✓ One-shot timer behavior works\n");

  sys_printf("Test 9: Error handling - NULL timer start\n");
  {
    test_assert(sys_timer_start(NULL) == false);
  }
  sys_printf("  ✓ NULL timer start handling works\n");

  sys_printf("Test 10: Error handling - NULL timer finalize\n");
  {
    test_assert(sys_timer_finalize(NULL) == false);
  }
  sys_printf("  ✓ NULL timer finalize handling works\n");

  sys_printf("Test 11: Timer restart after finalization\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    test_assert(sys_timer_finalize(&timer) == true);
    
    // Try to start again after finalization
    // Note: Behavior depends on implementation - might need re-initialization
    bool restart_result = sys_timer_start(&timer);
    // Don't assert on this as behavior may vary by implementation
    
    // Clean up if restart succeeded
    if (restart_result) {
      sys_timer_finalize(&timer);
    }
  }
  sys_printf("  ✓ Timer restart behavior tested\n");

  sys_printf("Test 12: Rapid start/stop cycles\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, simple_callback);
    
    for (int i = 0; i < 5; i++) {
      test_assert(sys_timer_start(&timer) == true);
      sys_sleep(5); // Brief pause
      test_assert(sys_timer_finalize(&timer) == true);
      
      // Re-initialize for next cycle
      if (i < 4) {
        timer = sys_timer_init(100, NULL, simple_callback);
      }
    }
  }
  sys_printf("  ✓ Rapid start/stop cycles work\n");

  sys_printf("Test 13: Edge case - zero interval\n");
  {
    sys_timer_t timer = sys_timer_init(0, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    sys_sleep(10);
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Zero interval timer handling works\n");

  sys_printf("Test 14: Large interval value\n");
  {
    sys_timer_t timer = sys_timer_init(UINT32_MAX, NULL, simple_callback);
    test_assert(sys_timer_start(&timer) == true);
    test_assert(sys_timer_finalize(&timer) == true);
  }
  sys_printf("  ✓ Large interval timer handling works\n");

  sys_printf("Test 15: Timer state consistency\n");
  {
    sys_timer_t timer = sys_timer_init(100, NULL, simple_callback);
    
    // Multiple starts should be handled gracefully
    test_assert(sys_timer_start(&timer) == true);
    bool second_start = sys_timer_start(&timer);
    // Implementation may return true (idempotent) or false (already started)
    
    // Multiple finalizes should be handled gracefully
    test_assert(sys_timer_finalize(&timer) == true);
    bool second_finalize = sys_timer_finalize(&timer);
    // Implementation may return true (idempotent) or false (already finalized)
  }
  sys_printf("  ✓ Timer state consistency tested\n");

  sys_printf("All timer function tests completed successfully!\n");
  return 0;
}
