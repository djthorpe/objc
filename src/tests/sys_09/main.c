#include <runtime-sys/sys.h>
#include <runtime-sys/thread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <tests/tests.h>

// Test data structure
typedef struct {
  atomic_int counter;
  atomic_bool finished;
  int expected_value;
} thread_test_data_t;

// Forward declarations
int test_sys_09(void);
static void simple_thread_func(void *arg);
static void counter_thread_func(void *arg);
static void core_specific_thread_func(void *arg);

int main(void) {
  // Run threading function tests
  return TestMain("test_sys_09", test_sys_09);
}

// Thread function implementations
static void simple_thread_func(void *arg) {
  atomic_bool *finished = (atomic_bool *)arg;
  // Simple thread that just sets a flag
  atomic_store(finished, true);
}

static void counter_thread_func(void *arg) {
  thread_test_data_t *data = (thread_test_data_t *)arg;
  // Increment counter by expected value
  atomic_fetch_add(&data->counter, data->expected_value);
  atomic_store(&data->finished, true);
}

static void core_specific_thread_func(void *arg) {
  thread_test_data_t *data = (thread_test_data_t *)arg;
  // Just mark as finished - we can't easily verify which core we're on
  atomic_store(&data->finished, true);
}

int test_sys_09(void) {
  int test_count = 0;
  int passed = 0;

  // Test 1: sys_thread_numcores returns reasonable value
  test_count++;
  uint8_t num_cores = sys_thread_numcores();
  if (num_cores >= 1 && num_cores <= 128) { // Reasonable range
    printf("✓ Test %d: sys_thread_numcores() returned %d cores\n", test_count,
           num_cores);
    passed++;
  } else {
    printf("✗ Test %d: sys_thread_numcores() returned unreasonable value: %d\n",
           test_count, num_cores);
  }

  // Test 2: sys_thread_core returns valid core number
  test_count++;
  uint8_t current_core = sys_thread_core();
  if (current_core < num_cores) {
    printf(
        "✓ Test %d: sys_thread_core() returned valid core %d (of %d cores)\n",
        test_count, current_core, num_cores);
    passed++;
  } else {
    printf("✗ Test %d: sys_thread_core() returned invalid core %d (only %d "
           "cores available)\n",
           test_count, current_core, num_cores);
  }

  // Test 3: sys_thread_create with simple function
  test_count++;
  atomic_bool simple_finished = false;
  bool created = sys_thread_create(simple_thread_func, &simple_finished);
  if (created) {
    // Wait for thread to finish (with timeout)
    int timeout = 100; // 100ms timeout
    while (!atomic_load(&simple_finished) && timeout > 0) {
      sys_sleep(1);
      timeout--;
    }

    if (atomic_load(&simple_finished)) {
      printf("✓ Test %d: sys_thread_create() successfully created and ran "
             "thread\n",
             test_count);
      passed++;
    } else {
      printf("✗ Test %d: sys_thread_create() thread did not complete in time\n",
             test_count);
    }
  } else {
    printf("✗ Test %d: sys_thread_create() failed to create thread\n",
           test_count);
  }

  // Test 4: sys_thread_create with NULL function should fail
  test_count++;
  bool null_result = sys_thread_create(NULL, NULL);
  if (!null_result) {
    printf("✓ Test %d: sys_thread_create() properly rejected NULL function\n",
           test_count);
    passed++;
  } else {
    printf(
        "✗ Test %d: sys_thread_create() incorrectly accepted NULL function\n",
        test_count);
  }

  // Test 4: Multiple threads with counter
  test_count++;
  thread_test_data_t data1 = {
      .counter = 0, .finished = false, .expected_value = 10};
  thread_test_data_t data2 = {
      .counter = 0, .finished = false, .expected_value = 20};

  bool thread1_created = sys_thread_create(counter_thread_func, &data1);
  bool thread2_created = sys_thread_create(counter_thread_func, &data2);

  if (thread1_created && thread2_created) {
    // Wait for both threads to finish
    int timeout = 100;
    while ((!atomic_load(&data1.finished) || !atomic_load(&data2.finished)) &&
           timeout > 0) {
      sys_sleep(1);
      timeout--;
    }

    if (atomic_load(&data1.finished) && atomic_load(&data2.finished)) {
      int counter1 = atomic_load(&data1.counter);
      int counter2 = atomic_load(&data2.counter);

      if (counter1 == 10 && counter2 == 20) {
        printf("✓ Test %d: Multiple threads executed correctly (counters: %d, "
               "%d)\n",
               test_count, counter1, counter2);
        passed++;
      } else {
        printf("✗ Test %d: Multiple threads had incorrect counter values "
               "(expected 10,20, got %d,%d)\n",
               test_count, counter1, counter2);
      }
    } else {
      printf("✗ Test %d: Multiple threads did not complete in time\n",
             test_count);
    }
  } else {
    printf("✗ Test %d: Failed to create multiple threads\n", test_count);
  }

  // Test 5: sys_thread_create_on_core with valid core
  test_count++;
  if (num_cores > 1) {
    thread_test_data_t core_data = {
        .counter = 0, .finished = false, .expected_value = 0};
    bool core_created =
        sys_thread_create_on_core(core_specific_thread_func, &core_data, 0);

    if (core_created) {
      int timeout = 100;
      while (!atomic_load(&core_data.finished) && timeout > 0) {
        sys_sleep(1);
        timeout--;
      }

      if (atomic_load(&core_data.finished)) {
        printf("✓ Test %d: sys_thread_create_on_core() successfully created "
               "thread on core 0\n",
               test_count);
        passed++;
      } else {
        printf("✗ Test %d: sys_thread_create_on_core() thread did not complete "
               "in time\n",
               test_count);
      }
    } else {
      printf("✗ Test %d: sys_thread_create_on_core() failed to create thread "
             "on core 0\n",
             test_count);
    }
  } else {
    printf("~ Test %d: Skipped sys_thread_create_on_core() test (single core "
           "system)\n",
           test_count);
    passed++; // Count as passed since it's not applicable
  }

  // Test 6: sys_thread_create_on_core with invalid core should fail
  test_count++;
  bool invalid_core_result =
      sys_thread_create_on_core(core_specific_thread_func, NULL, 255);
  if (!invalid_core_result) {
    printf("✓ Test %d: sys_thread_create_on_core() properly rejected invalid "
           "core 255\n",
           test_count);
    passed++;
  } else {
    printf("✗ Test %d: sys_thread_create_on_core() incorrectly accepted "
           "invalid core 255\n",
           test_count);
  }

  // Test 7: sys_thread_create_on_core with NULL function should fail
  test_count++;
  bool null_core_result = sys_thread_create_on_core(NULL, NULL, 0);
  if (!null_core_result) {
    printf("✓ Test %d: sys_thread_create_on_core() properly rejected NULL "
           "function\n",
           test_count);
    passed++;
  } else {
    printf("✗ Test %d: sys_thread_create_on_core() incorrectly accepted NULL "
           "function\n",
           test_count);
  }

  // Test 8: sys_sleep basic functionality
  test_count++;
  sys_sleep(0);  // Should not crash with 0
  sys_sleep(-1); // Should not crash with negative
  printf("✓ Test %d: sys_sleep() handled edge cases without crashing\n",
         test_count);
  passed++;

  printf("\nThreading Tests Summary: %d/%d tests passed\n", passed, test_count);
  return passed == test_count ? 0 : 1;
}
