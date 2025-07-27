/**
 * @file cond.c
 * @brief Pico-specific condition variable implementation using semaphores
 * @ingroup System
 */

#include "hardware/platform_defs.h"
#include "pico/mutex.h"
#include "pico/sem.h"
#include <pico/stdlib.h>
#include <runtime-sys/sys.h>
#include <stdio.h>

/**
 * @brief Internal condition variable data structure
 * 
 * Uses semaphore for signaling and mutex for protecting waiter count.
 * This implementation follows the "signal and continue" semantics where
 * the signaling thread continues and the waiting thread runs when the
 * signaling thread releases the associated mutex.
 */
typedef struct {
  semaphore_t sem;      ///< Semaphore for blocking/waking waiters
  mutex_t waiters_lock; ///< Protects the waiters_count field
  int waiters_count;    ///< Number of threads currently waiting
} pico_cond_data_t;

// Ensure the embedded buffer is large enough for our condition variable structure
_Static_assert(sizeof(pico_cond_data_t) <= SYS_COND_CTX_SIZE,
               "SYS_COND_CTX_SIZE too small for pico_cond_data_t");

/**
 * @brief Initialize a new condition variable
 */
sys_cond_t sys_cond_init(void) {
  sys_cond_t cond;
  cond.init = false;
  sys_memset(cond.ctx, 0, sizeof(cond.ctx));

  // Get pointer to the embedded condition variable data
  pico_cond_data_t *cd = (pico_cond_data_t *)cond.ctx;

  // Initialize semaphore with 0 permits (no waiters initially)
  // Max permits should accommodate reasonable number of waiting threads
  sem_init(&cd->sem, 0, 1024);
  
  // Initialize mutex for protecting waiters count
  mutex_init(&cd->waiters_lock);
  cd->waiters_count = 0;

  // Check if initialization succeeded
  if (mutex_is_initialized(&cd->waiters_lock)) {
    cond.init = true;
  }

  return cond;
}

/**
 * @brief Wait on a condition variable
 */
bool sys_cond_wait(sys_cond_t *cond, sys_mutex_t *mutex) {
  if (!cond || !cond->init || !mutex || !mutex->init) {
    return false;
  }

  pico_cond_data_t *cd = (pico_cond_data_t *)cond->ctx;
  mutex_t *user_mutex = (mutex_t *)mutex->ctx;

  // Increment waiter count
  mutex_enter_blocking(&cd->waiters_lock);
  cd->waiters_count++;
  mutex_exit(&cd->waiters_lock);

  // Release the user mutex
  mutex_exit(user_mutex);

  // Wait for signal (blocking)
  sem_acquire_blocking(&cd->sem);

  // Decrement waiter count
  mutex_enter_blocking(&cd->waiters_lock);
  cd->waiters_count--;
  mutex_exit(&cd->waiters_lock);

  // Reacquire the user mutex
  mutex_enter_blocking(user_mutex);

  return true;
}

/**
 * @brief Wait on a condition variable with timeout
 */
bool sys_cond_timedwait(sys_cond_t *cond, sys_mutex_t *mutex, uint32_t timeout_ms) {
  if (!cond || !cond->init || !mutex || !mutex->init) {
    return false;
  }

  // If timeout is 0, behave like regular wait
  if (timeout_ms == 0) {
    return sys_cond_wait(cond, mutex);
  }

  pico_cond_data_t *cd = (pico_cond_data_t *)cond->ctx;
  mutex_t *user_mutex = (mutex_t *)mutex->ctx;

  // Increment waiter count
  mutex_enter_blocking(&cd->waiters_lock);
  cd->waiters_count++;
  mutex_exit(&cd->waiters_lock);

  // Release the user mutex
  mutex_exit(user_mutex);

  // Calculate absolute timeout
  absolute_time_t timeout = make_timeout_time_ms(timeout_ms);
  
  // Wait for signal with timeout
  bool signaled = sem_acquire_timeout(&cd->sem, timeout);

  // Decrement waiter count
  mutex_enter_blocking(&cd->waiters_lock);
  cd->waiters_count--;
  mutex_exit(&cd->waiters_lock);

  // Reacquire the user mutex
  mutex_enter_blocking(user_mutex);

  return signaled;
}

/**
 * @brief Signal one waiting thread
 */
bool sys_cond_signal(sys_cond_t *cond) {
  if (!cond || !cond->init) {
    return false;
  }

  pico_cond_data_t *cd = (pico_cond_data_t *)cond->ctx;

  // Check if there are any waiters
  mutex_enter_blocking(&cd->waiters_lock);
  bool has_waiters = (cd->waiters_count > 0);
  mutex_exit(&cd->waiters_lock);

  // Only release a permit if there are waiters
  if (has_waiters) {
    sem_release(&cd->sem);
  }

  return true;
}

/**
 * @brief Signal all waiting threads
 */
bool sys_cond_broadcast(sys_cond_t *cond) {
  if (!cond || !cond->init) {
    return false;
  }

  pico_cond_data_t *cd = (pico_cond_data_t *)cond->ctx;

  // Get current number of waiters
  mutex_enter_blocking(&cd->waiters_lock);
  int waiters = cd->waiters_count;
  mutex_exit(&cd->waiters_lock);

  // Release permits for all waiting threads
  for (int i = 0; i < waiters; i++) {
    sem_release(&cd->sem);
  }

  return true;
}

/**
 * @brief Finalize and cleanup a condition variable
 */
void sys_cond_finalize(sys_cond_t *cond) {
  if (!cond || !cond->init) {
    return;
  }

  pico_cond_data_t *cd = (pico_cond_data_t *)cond->ctx;

  // Wake up any remaining waiters before cleanup
  sys_cond_broadcast(cond);

  // No explicit cleanup needed for Pico semaphore/mutex
  // Just clear the structure
  cond->init = false;
  sys_memset(cond->ctx, 0, sizeof(cond->ctx));
}
