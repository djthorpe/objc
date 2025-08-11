#include <runtime-sys/sys.h>
#include <string.h>
#include <tests/tests.h>

// Forward declaration
int test_sys_07(void);

int main(void) {
  // Run mutex function tests
  return TestMain("test_sys_07", test_sys_07);
}

int test_sys_07(void) {
  sys_printf("Test 1: Mutex initialization\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true); // Should be initialized
    sys_mutex_finalize(&mutex);
    test_assert(mutex.init == false); // Should be cleared after finalization
    sys_printf("  ✓ Mutex initialization and finalization works\n");
  }

  sys_printf("Test 2: Basic lock and unlock\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true);

    // Should be able to lock
    test_assert(sys_mutex_lock(&mutex) == true);

    // Should be able to unlock
    test_assert(sys_mutex_unlock(&mutex) == true);

    sys_mutex_finalize(&mutex);
    sys_printf("  ✓ Basic lock/unlock works\n");
  }

  sys_printf("Test 3: Try lock success\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true);

    // Should be able to trylock an unlocked mutex
    test_assert(sys_mutex_trylock(&mutex) == true);

    // Should be able to unlock
    test_assert(sys_mutex_unlock(&mutex) == true);

    sys_mutex_finalize(&mutex);
    sys_printf("  ✓ Trylock on unlocked mutex works\n");
  }

  sys_printf("Test 4: Try lock failure on locked mutex\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true);

    // Lock the mutex first
    test_assert(sys_mutex_lock(&mutex) == true);

    // Trylock should fail since it's already locked
    test_assert(sys_mutex_trylock(&mutex) == false);

    // Should still be able to unlock with the original lock
    test_assert(sys_mutex_unlock(&mutex) == true);

    sys_mutex_finalize(&mutex);
    sys_printf("  ✓ Trylock on locked mutex fails correctly\n");
  }

  sys_printf("Test 5: Multiple lock/unlock cycles\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true);

    for (int i = 0; i < 10; i++) {
      test_assert(sys_mutex_lock(&mutex) == true);
      test_assert(sys_mutex_unlock(&mutex) == true);
    }

    sys_mutex_finalize(&mutex);
    sys_printf("  ✓ Multiple lock/unlock cycles work\n");
  }

  sys_printf("Test 6: Error handling - NULL mutex\n");
  {
    // All operations should fail with NULL mutex
    test_assert(sys_mutex_lock(NULL) == false);
    test_assert(sys_mutex_trylock(NULL) == false);
    test_assert(sys_mutex_unlock(NULL) == false);

    // Finalize should handle NULL gracefully
    sys_mutex_finalize(NULL);

    sys_printf("  ✓ NULL mutex handling works\n");
  }

  sys_printf("Test 7: Error handling - uninitialized mutex\n");
  {
    sys_mutex_t mutex = {0}; // Uninitialized mutex

    // All operations should fail with uninitialized mutex
    test_assert(sys_mutex_lock(&mutex) == false);
    test_assert(sys_mutex_trylock(&mutex) == false);
    test_assert(sys_mutex_unlock(&mutex) == false);

    // Finalize should handle uninitialized mutex gracefully
    sys_mutex_finalize(&mutex);

    sys_printf("  ✓ Uninitialized mutex handling works\n");
  }

  sys_printf("Test 8: Error handling - operations after finalization\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true);

    // Finalize the mutex
    sys_mutex_finalize(&mutex);
    test_assert(mutex.init == false);

    // All operations should fail after finalization
    test_assert(sys_mutex_lock(&mutex) == false);
    test_assert(sys_mutex_trylock(&mutex) == false);
    test_assert(sys_mutex_unlock(&mutex) == false);

    // Multiple finalizations should be safe
    sys_mutex_finalize(&mutex);

    sys_printf("  ✓ Operations after finalization fail correctly\n");
  }

  sys_printf("Test 9: Multiple mutex instances\n");
  {
    sys_mutex_t mutex1 = sys_mutex_init();
    sys_mutex_t mutex2 = sys_mutex_init();

    test_assert(mutex1.init == true);
    test_assert(mutex2.init == true);

    // Should be able to lock both independently
    test_assert(sys_mutex_lock(&mutex1) == true);
    test_assert(sys_mutex_lock(&mutex2) == true);

    // Should be able to unlock both
    test_assert(sys_mutex_unlock(&mutex1) == true);
    test_assert(sys_mutex_unlock(&mutex2) == true);

    sys_mutex_finalize(&mutex1);
    sys_mutex_finalize(&mutex2);

    sys_printf("  ✓ Multiple mutex instances work independently\n");
  }

  sys_printf("Test 10: Lock/trylock combinations\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true);

    // Lock with lock()
    test_assert(sys_mutex_lock(&mutex) == true);

    // Trylock should fail
    test_assert(sys_mutex_trylock(&mutex) == false);

    // Unlock
    test_assert(sys_mutex_unlock(&mutex) == true);

    // Now trylock should succeed
    test_assert(sys_mutex_trylock(&mutex) == true);

    // Regular unlock should work
    test_assert(sys_mutex_unlock(&mutex) == true);

    sys_mutex_finalize(&mutex);
    sys_printf("  ✓ Lock/trylock combinations work correctly\n");
  }

  sys_printf("Test 11: Mutex state consistency\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true);

    // Initially should be unlocked (trylock succeeds)
    test_assert(sys_mutex_trylock(&mutex) == true);
    test_assert(sys_mutex_unlock(&mutex) == true);

    // After lock/unlock cycle, should be unlocked again
    test_assert(sys_mutex_lock(&mutex) == true);
    test_assert(sys_mutex_unlock(&mutex) == true);
    test_assert(sys_mutex_trylock(&mutex) == true);
    test_assert(sys_mutex_unlock(&mutex) == true);

    sys_mutex_finalize(&mutex);
    sys_printf("  ✓ Mutex state consistency maintained\n");
  }

  sys_printf("Test 12: Mutex operation sequencing\n");
  {
    sys_mutex_t mutex = sys_mutex_init();
    test_assert(mutex.init == true);

    // Test lock -> unlock -> lock -> unlock sequence
    test_assert(sys_mutex_lock(&mutex) == true);
    test_assert(sys_mutex_unlock(&mutex) == true);
    test_assert(sys_mutex_lock(&mutex) == true);
    test_assert(sys_mutex_unlock(&mutex) == true);

    sys_mutex_finalize(&mutex);
    sys_printf("  ✓ Mutex operation sequencing works\n");
  }

  sys_printf("All mutex tests completed successfully!\n");
  return 0;
}
