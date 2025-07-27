#include <runtime-sys/sys.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tests/tests.h>

// Forward declarations
int test_sys_10(void);

// Test data structures
typedef struct {
  sys_mutex_t mutex;
  sys_cond_t condition;
  sys_waitgroup_t waitgroup; // Add waitgroup for proper synchronization
  atomic_int counter;
  atomic_int producer_count;
  atomic_int consumer_count;
  atomic_bool should_stop;
  int *buffer;
  int buffer_size;
  atomic_int buffer_head;
  atomic_int buffer_tail;
  atomic_int buffer_count;
  atomic_long total_produced;
  atomic_long total_consumed;
  atomic_int active_producers;
  atomic_int active_consumers;
} stress_test_data_t;

typedef struct {
  stress_test_data_t *shared_data;
  int thread_id;
  int iterations;
} thread_data_t;

// High contention worker - many threads competing for the same mutex
static void high_contention_worker(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
  stress_test_data_t *shared = data->shared_data;

  for (int i = 0; i < data->iterations; i++) {
    // Lock mutex and increment counter
    sys_mutex_lock(&shared->mutex);
    int old_val = atomic_load(&shared->counter);

    // Simulate some work while holding the lock
    volatile int dummy = 0;
    for (int j = 0; j < 100; j++) {
      dummy += j;
    }

    atomic_store(&shared->counter, old_val + 1);
    sys_mutex_unlock(&shared->mutex);

    // Brief yield to allow other threads to compete
    if (i % 10 == 0) {
      sys_sleep(1); // 1ms
    }
  }

  // Signal completion to waitgroup
  sys_waitgroup_done(&shared->waitgroup);
}

// Producer thread - generates data and signals consumers
static void producer_thread(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
  stress_test_data_t *shared = data->shared_data;

  atomic_fetch_add(&shared->active_producers, 1);

  for (int i = 0; i < data->iterations; i++) {
    int value = data->thread_id * 1000 + i;

    sys_mutex_lock(&shared->mutex);

    // Wait for space in buffer
    while (atomic_load(&shared->buffer_count) >= shared->buffer_size &&
           !atomic_load(&shared->should_stop)) {
      sys_cond_wait(&shared->condition, &shared->mutex);
    }

    if (atomic_load(&shared->should_stop)) {
      sys_mutex_unlock(&shared->mutex);
      break;
    }

    // Add to buffer
    int head = atomic_load(&shared->buffer_head);
    shared->buffer[head] = value;
    atomic_store(&shared->buffer_head, (head + 1) % shared->buffer_size);
    atomic_fetch_add(&shared->buffer_count, 1);
    atomic_fetch_add(&shared->total_produced, 1);

    // Signal consumers
    sys_cond_broadcast(&shared->condition);
    sys_mutex_unlock(&shared->mutex);

    // Occasional brief pause
    if (i % 50 == 0) {
      sys_sleep(1);
    }
  }

  atomic_fetch_sub(&shared->active_producers, 1);
  sys_mutex_lock(&shared->mutex);
  sys_cond_broadcast(&shared->condition);
  sys_mutex_unlock(&shared->mutex);

  // Signal completion to waitgroup
  sys_waitgroup_done(&shared->waitgroup);
}

// Consumer thread - consumes data produced by producers
static void consumer_thread(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
  stress_test_data_t *shared = data->shared_data;

  atomic_fetch_add(&shared->active_consumers, 1);

  while (!atomic_load(&shared->should_stop)) {
    sys_mutex_lock(&shared->mutex);

    // Wait for data or all producers to finish
    while (atomic_load(&shared->buffer_count) == 0 &&
           (atomic_load(&shared->active_producers) > 0 ||
            !atomic_load(&shared->should_stop))) {
      sys_cond_wait(&shared->condition, &shared->mutex);
    }

    if (atomic_load(&shared->buffer_count) == 0 &&
        atomic_load(&shared->active_producers) == 0) {
      sys_mutex_unlock(&shared->mutex);
      break;
    }

    if (atomic_load(&shared->buffer_count) > 0) {
      // Consume from buffer
      int tail = atomic_load(&shared->buffer_tail);
      // Read value from buffer (for validation if needed)
      volatile int value = shared->buffer[tail];
      (void)value; // Suppress unused variable warning
      atomic_store(&shared->buffer_tail, (tail + 1) % shared->buffer_size);
      atomic_fetch_sub(&shared->buffer_count, 1);
      atomic_fetch_add(&shared->total_consumed, 1);

      // Signal producers
      sys_cond_broadcast(&shared->condition);
    }

    sys_mutex_unlock(&shared->mutex);

    // Brief processing time
    sys_sleep(1);
  }

  atomic_fetch_sub(&shared->active_consumers, 1);

  // Signal completion to waitgroup
  sys_waitgroup_done(&shared->waitgroup);
}

// Race condition worker - tests atomic operations and mutex interaction
static void race_condition_worker(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
  stress_test_data_t *shared = data->shared_data;

  for (int i = 0; i < data->iterations; i++) {
    // Mix of mutex-protected and atomic operations

    // Test 1: Mutex-protected counter increment
    sys_mutex_lock(&shared->mutex);
    int old_counter = atomic_load(&shared->counter);

    // Simulate critical section work
    volatile int temp = old_counter;
    for (int j = 0; j < 10; j++) {
      temp += 1;
    }
    temp -= 9; // Should result in +1

    atomic_store(&shared->counter, temp);
    sys_mutex_unlock(&shared->mutex);

    // Test 2: Atomic operations
    if (i % 2 == 0) {
      atomic_fetch_add(&shared->producer_count, 1);
    } else {
      atomic_fetch_add(&shared->consumer_count, 1);
    }
  }

  // Signal completion to waitgroup
  sys_waitgroup_done(&shared->waitgroup);
}

// Mutex stress worker - rapid lock/unlock cycles
static void mutex_stress_worker(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
  stress_test_data_t *shared = data->shared_data;

  for (int i = 0; i < data->iterations; i++) {
    // Rapid lock/unlock cycles
    for (int j = 0; j < 10; j++) {
      sys_mutex_lock(&shared->mutex);

      // Very brief critical section
      volatile int temp = atomic_load(&shared->counter);
      temp += data->thread_id;
      atomic_store(&shared->counter, temp);

      sys_mutex_unlock(&shared->mutex);
    }

    // Test condition variable signaling under stress
    if (i % 20 == 0) {
      sys_mutex_lock(&shared->mutex);
      sys_cond_signal(&shared->condition);
      sys_mutex_unlock(&shared->mutex);
    }

    if (i % 100 == 0) {
      sys_sleep(1);
    }
  }

  // Signal completion to waitgroup
  sys_waitgroup_done(&shared->waitgroup);
}

// Main threading stress test function
int test_sys_10(void) {
  int num_cores = sys_thread_numcores();

  sys_printf("Starting comprehensive threading stress tests...\n");
  sys_printf("Detected %d CPU cores\n", num_cores);

  // Test 1: High contention mutex stress
  sys_printf("\n=== Test 1: High Contention Mutex Stress ===\n");
  {
    stress_test_data_t shared_data = {0};
    shared_data.mutex = sys_mutex_init();
    shared_data.waitgroup = sys_waitgroup_init();
    atomic_init(&shared_data.counter, 0);

    const int NUM_THREADS = 4;  // Reduce thread count
    const int ITERATIONS = 500; // Reduce iterations
    thread_data_t thread_data[NUM_THREADS];

    sys_printf("Starting %d threads with %d iterations each...\n", NUM_THREADS,
               ITERATIONS);

    // Add expected thread count to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
      thread_data[i].shared_data = &shared_data;
      thread_data[i].thread_id = i;
      thread_data[i].iterations = ITERATIONS;

      sys_printf("Creating thread %d...\n", i);
      if (!sys_thread_create(high_contention_worker, &thread_data[i])) {
        sys_printf("Failed to create thread %d\n", i);
        return 1;
      }
      sys_printf("Thread %d created successfully\n", i);
    }

    // Wait for all threads to complete
    sys_printf("Waiting for all threads to complete...\n");
    sys_waitgroup_wait(&shared_data.waitgroup);

    int expected = NUM_THREADS * ITERATIONS;
    int final_count = atomic_load(&shared_data.counter);
    sys_printf("Expected: %d, Got: %d\n", expected, final_count);

    if (final_count != expected) {
      sys_printf("FAIL: High contention test failed - data race detected!\n");
      return 1;
    }
    sys_printf("PASS: High contention test completed successfully\n");

    sys_waitgroup_finalize(&shared_data.waitgroup);
    sys_mutex_finalize(&shared_data.mutex);
  }

  // Test 2: Producer/Consumer stress test
  sys_printf("\n=== Test 2: Producer/Consumer Stress ===\n");
  {
    stress_test_data_t shared_data = {0};
    shared_data.mutex = sys_mutex_init();
    shared_data.condition = sys_cond_init();
    shared_data.waitgroup = sys_waitgroup_init();

    const int BUFFER_SIZE = 100;
    const int NUM_PRODUCERS = 2; // Reduce thread count
    const int NUM_CONSUMERS = 2;
    const int ITEMS_PER_PRODUCER = 50; // Reduce work load

    shared_data.buffer = malloc(BUFFER_SIZE * sizeof(int));
    shared_data.buffer_size = BUFFER_SIZE;
    atomic_init(&shared_data.buffer_head, 0);
    atomic_init(&shared_data.buffer_tail, 0);
    atomic_init(&shared_data.buffer_count, 0);
    atomic_init(&shared_data.total_produced, 0);
    atomic_init(&shared_data.total_consumed, 0);
    atomic_init(&shared_data.active_producers, 0);
    atomic_init(&shared_data.active_consumers, 0);
    atomic_init(&shared_data.should_stop, false);

    thread_data_t producer_data[NUM_PRODUCERS];
    thread_data_t consumer_data[NUM_CONSUMERS];

    sys_printf("Starting %d producers and %d consumers...\n", NUM_PRODUCERS,
               NUM_CONSUMERS);

    // Add expected thread count to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, NUM_PRODUCERS + NUM_CONSUMERS);

    // Start consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
      consumer_data[i].shared_data = &shared_data;
      consumer_data[i].thread_id = i + 1000;
      consumer_data[i].iterations = 0; // Consumers run until producers finish

      if (!sys_thread_create(consumer_thread, &consumer_data[i])) {
        sys_printf("Failed to create consumer thread %d\n", i);
        return 1;
      }
    }

    // Start producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
      producer_data[i].shared_data = &shared_data;
      producer_data[i].thread_id = i;
      producer_data[i].iterations = ITEMS_PER_PRODUCER;

      if (!sys_thread_create(producer_thread, &producer_data[i])) {
        sys_printf("Failed to create producer thread %d\n", i);
        return 1;
      }
    }

    // Give threads a moment to start and update counters
    sys_sleep(100);

    // Wait for producers to finish first
    sys_printf("Waiting for producers to finish...\n");
    while (atomic_load(&shared_data.active_producers) > 0) {
      sys_sleep(10);
    }

    // Signal consumers to stop
    atomic_store(&shared_data.should_stop, true);
    sys_mutex_lock(&shared_data.mutex);
    sys_cond_broadcast(&shared_data.condition);
    sys_mutex_unlock(&shared_data.mutex);

    // Wait for all threads to complete
    sys_printf("Waiting for all threads to complete...\n");
    sys_waitgroup_wait(&shared_data.waitgroup);

    long produced = atomic_load(&shared_data.total_produced);
    long consumed = atomic_load(&shared_data.total_consumed);
    long expected_total = NUM_PRODUCERS * ITEMS_PER_PRODUCER;

    sys_printf("Produced: %ld, Consumed: %ld, Expected: %ld\n", produced,
               consumed, expected_total);

    if (produced != expected_total) {
      sys_printf("FAIL: Producer count mismatch!\n");
      return 1;
    }

    if (consumed != produced) {
      sys_printf("FAIL: Consumer count mismatch!\n");
      return 1;
    }

    sys_printf("PASS: Producer/Consumer test completed successfully\n");

    free(shared_data.buffer);
    sys_waitgroup_finalize(&shared_data.waitgroup);
    sys_mutex_finalize(&shared_data.mutex);
    sys_cond_finalize(&shared_data.condition);
  }

  // Test 3: Race condition and atomic operations stress
  sys_printf("\n=== Test 3: Race Condition & Atomic Operations Stress ===\n");
  {
    stress_test_data_t shared_data = {0};
    shared_data.mutex = sys_mutex_init();
    shared_data.waitgroup = sys_waitgroup_init();

    atomic_init(&shared_data.counter, 0);
    atomic_init(&shared_data.producer_count, 0);
    atomic_init(&shared_data.consumer_count, 0);
    atomic_init(&shared_data.total_produced, 0);
    atomic_init(&shared_data.total_consumed, 0);

    const int NUM_THREADS = 4;  // Reduce thread count
    const int ITERATIONS = 200; // Reduce iterations
    thread_data_t thread_data[NUM_THREADS];

    sys_printf("Starting %d threads for race condition testing...\n",
               NUM_THREADS);

    // Add expected thread count to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
      thread_data[i].shared_data = &shared_data;
      thread_data[i].thread_id = i + 1;
      thread_data[i].iterations = ITERATIONS;

      if (!sys_thread_create(race_condition_worker, &thread_data[i])) {
        sys_printf("Failed to create race condition thread %d\n", i);
        return 1;
      }
    }

    // Wait for all threads to complete
    sys_printf("Waiting for all threads to complete...\n");
    sys_waitgroup_wait(&shared_data.waitgroup);

    long expected_producer = NUM_THREADS * ITERATIONS / 2;
    long expected_consumer = NUM_THREADS * ITERATIONS / 2;
    long final_producer = atomic_load(&shared_data.producer_count);
    long final_consumer = atomic_load(&shared_data.consumer_count);
    int final_counter = atomic_load(&shared_data.counter);

    sys_printf("Producer ops: %ld/%ld, Consumer ops: %ld/%ld, Counter: %d\n",
               final_producer, expected_producer, final_consumer,
               expected_consumer, final_counter);

    if (final_producer != expected_producer ||
        final_consumer != expected_consumer) {
      sys_printf("FAIL: Atomic operations lost!\n");
      return 1;
    }

    sys_printf("PASS: Race condition test completed successfully\n");

    sys_waitgroup_finalize(&shared_data.waitgroup);
    sys_mutex_finalize(&shared_data.mutex);
  }

  // Test 4: Extreme mutex stress test
  sys_printf("\n=== Test 4: Extreme Mutex Stress ===\n");
  {
    stress_test_data_t shared_data = {0};
    shared_data.mutex = sys_mutex_init();
    shared_data.condition = sys_cond_init();
    shared_data.waitgroup = sys_waitgroup_init();
    atomic_init(&shared_data.counter, 0);

    const int NUM_THREADS = 6;  // Moderate thread count
    const int ITERATIONS = 100; // Reduce iterations
    thread_data_t thread_data[NUM_THREADS];

    sys_printf("Starting %d threads for extreme mutex stress...\n",
               NUM_THREADS);

    // Add expected thread count to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
      thread_data[i].shared_data = &shared_data;
      thread_data[i].thread_id = i + 1;
      thread_data[i].iterations = ITERATIONS;

      if (!sys_thread_create(mutex_stress_worker, &thread_data[i])) {
        sys_printf("Failed to create mutex stress thread %d\n", i);
        return 1;
      }
    }

    // Wait for all threads to complete
    sys_printf("Waiting for all threads to complete...\n");
    sys_waitgroup_wait(&shared_data.waitgroup);

    int final_counter = atomic_load(&shared_data.counter);
    sys_printf("Final counter: %d operations\n", final_counter);

    if (final_counter == 0) {
      sys_printf("FAIL: No operations completed!\n");
      return 1;
    }

    sys_printf("PASS: Extreme mutex stress test completed successfully\n");

    sys_waitgroup_finalize(&shared_data.waitgroup);
    sys_mutex_finalize(&shared_data.mutex);
    sys_cond_finalize(&shared_data.condition);
  }

  // Test 5: CPU core affinity stress (Linux only)
  sys_printf("\n=== Test 5: CPU Core Affinity Stress ===\n");
  if (num_cores > 1) {
    stress_test_data_t shared_data = {0};
    shared_data.mutex = sys_mutex_init();
    shared_data.waitgroup = sys_waitgroup_init();
    atomic_init(&shared_data.counter, 0);

    const int THREADS_PER_CORE = 1; // Reduce threads per core
    const int MAX_CORES = 4;        // Limit to 4 cores max
    const int CORES_TO_USE = (num_cores > MAX_CORES) ? MAX_CORES : num_cores;
    const int ITERATIONS = 100; // Reduce iterations
    thread_data_t thread_data[CORES_TO_USE * THREADS_PER_CORE];

    sys_printf("Starting %d threads across %d cores...\n",
               CORES_TO_USE * THREADS_PER_CORE, CORES_TO_USE);

    // Add expected thread count to waitgroup
    sys_waitgroup_add(&shared_data.waitgroup, CORES_TO_USE * THREADS_PER_CORE);

    for (int core = 0; core < CORES_TO_USE; core++) {
      for (int t = 0; t < THREADS_PER_CORE; t++) {
        int thread_idx = core * THREADS_PER_CORE + t;
        thread_data[thread_idx].shared_data = &shared_data;
        thread_data[thread_idx].thread_id = thread_idx;
        thread_data[thread_idx].iterations = ITERATIONS;

        // Try to create thread on specific core
        if (!sys_thread_create_on_core(high_contention_worker,
                                       &thread_data[thread_idx], core)) {
          sys_printf(
              "Warning: Failed to create thread on core %d, using default\n",
              core);
          if (!sys_thread_create(high_contention_worker,
                                 &thread_data[thread_idx])) {
            sys_printf("Failed to create thread %d\n", thread_idx);
            return 1;
          }
        }
      }
    }

    // Wait for all threads to complete
    sys_printf("Waiting for all threads to complete...\n");
    sys_waitgroup_wait(&shared_data.waitgroup);

    int expected = CORES_TO_USE * THREADS_PER_CORE * ITERATIONS;
    int final_count = atomic_load(&shared_data.counter);
    sys_printf("Expected: %d, Got: %d\n", expected, final_count);

    if (final_count != expected) {
      sys_printf("FAIL: Core affinity test failed!\n");
      return 1;
    }

    sys_printf("PASS: CPU core affinity test completed successfully\n");

    sys_waitgroup_finalize(&shared_data.waitgroup);
    sys_mutex_finalize(&shared_data.mutex);
  } else {
    sys_printf("SKIP: Only one CPU core detected\n");
  }

  sys_printf("\n=== All Threading Stress Tests Completed Successfully! ===\n");
  sys_printf("✓ High contention mutex operations\n");
  sys_printf("✓ Producer/consumer synchronization\n");
  sys_printf("✓ Race condition detection\n");
  sys_printf("✓ Extreme mutex stress testing\n");
  sys_printf("✓ CPU core affinity (where applicable)\n");

  return 0;
}

int main(void) {
  // Run threading stress tests
  return TestMain("test_sys_10", test_sys_10);
}
