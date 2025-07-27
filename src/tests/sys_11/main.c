#include <runtime-sys/sys.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tests/tests.h>

// Forward declarations
int test_sys_11(void);

// Test data structures
typedef struct {
  sys_waitgroup_t waitgroup;
  atomic_int counter;
  atomic_int thread_count;
  atomic_bool should_fail;
  int *results;
  int expected_results;
} waitgroup_test_data_t;

typedef struct {
  waitgroup_test_data_t *shared_data;
  int thread_id;
  int work_amount;
  int delay_ms;
} waitgroup_thread_data_t;

// Simple worker that increments a counter
static void simple_worker(void *arg) {
  waitgroup_thread_data_t *data = (waitgroup_thread_data_t *)arg;
  waitgroup_test_data_t *shared = data->shared_data;

  // Do some work
  for (int i = 0; i < data->work_amount; i++) {
    atomic_fetch_add(&shared->counter, 1);
  }

  // Signal completion
  sys_waitgroup_done(&shared->waitgroup);
}

// Worker with delay to test timing
static void delayed_worker(void *arg) {
  waitgroup_thread_data_t *data = (waitgroup_thread_data_t *)arg;
  waitgroup_test_data_t *shared = data->shared_data;

  // Wait for specified delay
  sys_sleep(data->delay_ms);

  // Do work
  atomic_fetch_add(&shared->counter, data->work_amount);

  // Record thread completion
  atomic_fetch_add(&shared->thread_count, 1);

  // Signal completion
  sys_waitgroup_done(&shared->waitgroup);
}

// Worker that stores result in array
static void result_worker(void *arg) {
  waitgroup_thread_data_t *data = (waitgroup_thread_data_t *)arg;
  waitgroup_test_data_t *shared = data->shared_data;

  // Store result at thread_id position
  shared->results[data->thread_id] = data->work_amount * 10 + data->thread_id;

  // Signal completion
  sys_waitgroup_done(&shared->waitgroup);
}

// Worker for stress testing
static void stress_worker(void *arg) {
  waitgroup_thread_data_t *data = (waitgroup_thread_data_t *)arg;
  waitgroup_test_data_t *shared = data->shared_data;

  // Rapid increment operations
  for (int i = 0; i < data->work_amount; i++) {
    atomic_fetch_add(&shared->counter, 1);

    // Brief yield every 100 operations
    if (i % 100 == 0) {
      sys_sleep(1);
    }
  }

  // Signal completion
  sys_waitgroup_done(&shared->waitgroup);
}

int main(void) {
  // Run threading stress tests
  return TestMain("test_sys_11", test_sys_11);
}

int test_sys_11(void) {
  sys_printf("Starting waitgroup-specific tests...\n");

  // Test 1: Basic waitgroup functionality
  sys_printf("\n=== Test 1: Basic Waitgroup Functionality ===\n");
  {
    waitgroup_test_data_t shared_data = {0};
    shared_data.waitgroup = sys_waitgroup_init();
    atomic_init(&shared_data.counter, 0);

#ifdef SYSTEM_NAME_PICO
    // On Pico, only one worker thread can run on core 1
    const int NUM_THREADS = 1;
#else
    // On other platforms, use all available cores
    const int NUM_THREADS = sys_thread_numcores();
#endif
    const int WORK_PER_THREAD = 100;
    waitgroup_thread_data_t thread_data[NUM_THREADS];

    sys_printf("Creating %d threads, each doing %d increments...\n",
               NUM_THREADS, WORK_PER_THREAD);

    // Add threads to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, NUM_THREADS);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
      thread_data[i].shared_data = &shared_data;
      thread_data[i].thread_id = i;
      thread_data[i].work_amount = WORK_PER_THREAD;
      thread_data[i].delay_ms = 0;

#ifdef SYSTEM_NAME_PICO
      if (!sys_thread_create_on_core(simple_worker, &thread_data[i], 1)) {
        sys_printf("Failed to create thread %d on core 1\n", i);
        return 1;
      }
#else
      if (!sys_thread_create(simple_worker, &thread_data[i])) {
        sys_printf("Failed to create thread %d\n", i);
        return 1;
      }
#endif
    }

    // Wait for all threads to complete
    sys_printf("Waiting for all threads to complete...\n");
    sys_waitgroup_finalize(&shared_data.waitgroup);

    int expected = NUM_THREADS * WORK_PER_THREAD;
    int final_count = atomic_load(&shared_data.counter);
    sys_printf("Expected: %d, Got: %d\n", expected, final_count);

    if (final_count != expected) {
      sys_printf("FAIL: Basic waitgroup test failed!\n");
      return 1;
    }
    sys_printf("PASS: Basic waitgroup test completed successfully\n");

    // Note: waitgroup is automatically finalized
  }

  // Test 2: Waitgroup with delayed threads
  sys_printf("\n=== Test 2: Waitgroup with Delayed Threads ===\n");
  {
    waitgroup_test_data_t shared_data = {0};
    shared_data.waitgroup = sys_waitgroup_init();
    atomic_init(&shared_data.counter, 0);
    atomic_init(&shared_data.thread_count, 0);

#ifdef SYSTEM_NAME_PICO
    // On Pico, only one worker thread can run on core 1
    const int NUM_THREADS = 1;
#else
    // On other platforms, use all available cores
    const int NUM_THREADS = sys_thread_numcores();
#endif
    waitgroup_thread_data_t thread_data[NUM_THREADS];
    int delays[] = {50, 100, 150, 200}; // Different delays for each thread
    int work_amounts[] = {10, 20, 30, 40};

    sys_printf("Creating %d threads with different delays...\n", NUM_THREADS);

    // Add threads to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, NUM_THREADS);

    // Create threads with different delays
    for (int i = 0; i < NUM_THREADS; i++) {
      thread_data[i].shared_data = &shared_data;
      thread_data[i].thread_id = i;
      thread_data[i].work_amount =
          work_amounts[i % 4];                 // Cycle through work amounts
      thread_data[i].delay_ms = delays[i % 4]; // Cycle through delays

#ifdef SYSTEM_NAME_PICO
      if (!sys_thread_create_on_core(delayed_worker, &thread_data[i], 1)) {
        sys_printf("Failed to create delayed thread %d on core 1\n", i);
        return 1;
      }
#else
      if (!sys_thread_create(delayed_worker, &thread_data[i])) {
        sys_printf("Failed to create delayed thread %d\n", i);
        return 1;
      }
#endif
    }

    // Wait for all threads to complete
    sys_printf("Waiting for all delayed threads to complete...\n");
    sys_waitgroup_finalize(&shared_data.waitgroup);

    // Calculate expected work (sum of work amounts for each thread)
    int expected_work = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
      expected_work += work_amounts[i % 4];
    }
    int expected_threads = NUM_THREADS;
    int final_count = atomic_load(&shared_data.counter);
    int thread_count = atomic_load(&shared_data.thread_count);

    sys_printf("Expected work: %d, Got: %d\n", expected_work, final_count);
    sys_printf("Expected threads: %d, Got: %d\n", expected_threads,
               thread_count);

    if (final_count != expected_work || thread_count != expected_threads) {
      sys_printf("FAIL: Delayed waitgroup test failed!\n");
      return 1;
    }
    sys_printf("PASS: Delayed waitgroup test completed successfully\n");

    // Note: waitgroup is automatically finalized
  }

  // Test 3: Waitgroup with result collection
  sys_printf("\n=== Test 3: Waitgroup with Result Collection ===\n");
  {
    waitgroup_test_data_t shared_data = {0};
    shared_data.waitgroup = sys_waitgroup_init();

#ifdef SYSTEM_NAME_PICO
    // On Pico, only one worker thread can run on core 1
    const int NUM_THREADS = 1;
#else
    // On other platforms, use all available cores
    const int NUM_THREADS = sys_thread_numcores();
#endif
    shared_data.results = sys_malloc(NUM_THREADS * sizeof(int));
    shared_data.expected_results = NUM_THREADS;
    waitgroup_thread_data_t thread_data[NUM_THREADS];

    // Initialize results array
    for (int i = 0; i < NUM_THREADS; i++) {
      shared_data.results[i] = -1; // Sentinel value
    }

    sys_printf("Creating %d threads to collect results...\n", NUM_THREADS);

    // Add threads to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, NUM_THREADS);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
      thread_data[i].shared_data = &shared_data;
      thread_data[i].thread_id = i;
      thread_data[i].work_amount =
          i + 1; // Each thread has different work amount
      thread_data[i].delay_ms = 0;

#ifdef SYSTEM_NAME_PICO
      if (!sys_thread_create_on_core(result_worker, &thread_data[i], 1)) {
        sys_printf("Failed to create result thread %d on core 1\n", i);
        return 1;
      }
#else
      if (!sys_thread_create(result_worker, &thread_data[i])) {
        sys_printf("Failed to create result thread %d\n", i);
        return 1;
      }
#endif
    }

    // Wait for all threads to complete
    sys_printf("Waiting for all result threads to complete...\n");
    sys_waitgroup_finalize(&shared_data.waitgroup);

    // Verify results
    bool all_results_correct = true;
    for (int i = 0; i < NUM_THREADS; i++) {
      int expected = (i + 1) * 10 + i; // work_amount * 10 + thread_id
      if (shared_data.results[i] != expected) {
        sys_printf("Thread %d: Expected %d, Got %d\n", i, expected,
                   shared_data.results[i]);
        all_results_correct = false;
      }
    }

    if (!all_results_correct) {
      sys_printf("FAIL: Result collection test failed!\n");
      return 1;
    }
    sys_printf("PASS: Result collection test completed successfully\n");

    sys_free(shared_data.results);
    // Note: waitgroup is automatically finalized
  }

  // Test 4: Stress test with many threads
  sys_printf("\n=== Test 4: Waitgroup Stress Test ===\n");
  {
    waitgroup_test_data_t shared_data = {0};
    shared_data.waitgroup = sys_waitgroup_init();
    atomic_init(&shared_data.counter, 0);

#ifdef SYSTEM_NAME_PICO
    // On Pico, only one worker thread can run on core 1
    const int NUM_THREADS = 1;
#else
    // On other platforms, use all available cores
    const int NUM_THREADS = sys_thread_numcores();
#endif
    const int WORK_PER_THREAD = 500;
    waitgroup_thread_data_t thread_data[NUM_THREADS];

    sys_printf("Creating %d threads for stress test...\n", NUM_THREADS);

    // Add threads to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, NUM_THREADS);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
      thread_data[i].shared_data = &shared_data;
      thread_data[i].thread_id = i;
      thread_data[i].work_amount = WORK_PER_THREAD;
      thread_data[i].delay_ms = 0;

#ifdef SYSTEM_NAME_PICO
      if (!sys_thread_create_on_core(stress_worker, &thread_data[i], 1)) {
        sys_printf("Failed to create stress thread %d on core 1\n", i);
        return 1;
      }
#else
      if (!sys_thread_create(stress_worker, &thread_data[i])) {
        sys_printf("Failed to create stress thread %d\n", i);
        return 1;
      }
#endif
    }

    // Wait for all threads to complete
    sys_printf("Waiting for all stress threads to complete...\n");
    sys_waitgroup_finalize(&shared_data.waitgroup);

    int expected = NUM_THREADS * WORK_PER_THREAD;
    int final_count = atomic_load(&shared_data.counter);
    sys_printf("Expected: %d, Got: %d\n", expected, final_count);

    if (final_count != expected) {
      sys_printf("FAIL: Stress test failed!\n");
      return 1;
    }
    sys_printf("PASS: Stress test completed successfully\n");

    // Note: waitgroup is automatically finalized
  }

  // Test 5: Error handling
  sys_printf("\n=== Test 5: Waitgroup Error Handling ===\n");
  {
    sys_waitgroup_t wg = sys_waitgroup_init();

    // Test calling Done() without Add()
    sys_printf("Testing Done() without Add()...\n");
    bool result = sys_waitgroup_done(&wg);
    if (result) {
      sys_printf("FAIL: Done() should fail when counter is 0\n");
      return 1;
    }
    sys_printf("PASS: Done() correctly failed when counter is 0\n");

    // Test adding negative value
    sys_printf("Testing Add() with negative value...\n");
    result = sys_waitgroup_add(&wg, -1);
    if (result) {
      sys_printf("FAIL: Add() should fail with negative value\n");
      return 1;
    }
    sys_printf("PASS: Add() correctly failed with negative value\n");

    // Test normal operation with finalize
    sys_printf("Testing normal Add/Done/Finalize cycle...\n");
    if (!sys_waitgroup_add(&wg, 1)) {
      sys_printf("FAIL: Add(1) should succeed\n");
      return 1;
    }
    if (!sys_waitgroup_done(&wg)) {
      sys_printf("FAIL: Done() should succeed after Add(1)\n");
      return 1;
    }
    sys_waitgroup_finalize(&wg);
    sys_printf("PASS: Normal Add/Done/Finalize cycle works correctly\n");

    // Note: waitgroup is automatically finalized
  }

  sys_printf("\n=== All Waitgroup Tests Completed Successfully! ===\n");
  sys_printf("✓ Basic waitgroup functionality\n");
  sys_printf("✓ Delayed thread synchronization\n");
  sys_printf("✓ Result collection coordination\n");
  sys_printf("✓ High-load stress testing\n");
  sys_printf("✓ Error condition handling\n");

  return 0;
}
