#include <pthread.h>
#include <runtime-sys/sys.h>

/**
 * @brief Internal waitgroup data structure using pthread primitives
 */
typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int counter;
} waitgroup_data_t;

// Ensure the embedded buffer is large enough for our waitgroup structure
_Static_assert(sizeof(waitgroup_data_t) <= SYS_WAITGROUP_CTX_SIZE,
               "SYS_WAITGROUP_CTX_SIZE too small for waitgroup_data_t");

/**
 * @brief Initialize a new waitgroup
 */
sys_waitgroup_t sys_waitgroup_init(void) {
  sys_waitgroup_t wg;
  wg.init = false;
  sys_memset(wg.ctx, 0, sizeof(wg.ctx));

  // Get pointer to the embedded waitgroup data
  waitgroup_data_t *wgd = (waitgroup_data_t *)wg.ctx;

  // Initialize the mutex with error-checking attributes
  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
  int mutex_result = pthread_mutex_init(&wgd->mutex, &mutex_attr);
  pthread_mutexattr_destroy(&mutex_attr);

  if (mutex_result != 0) {
    return wg; // Return uninitialized waitgroup on error
  }

  // Initialize the condition variable
  int cond_result = pthread_cond_init(&wgd->cond, NULL);
  if (cond_result != 0) {
    pthread_mutex_destroy(&wgd->mutex);
    return wg; // Return uninitialized waitgroup on error
  }

  // Initialize the counter to 0
  wgd->counter = 0;

  // Mark as successfully initialized
  wg.init = true;
  return wg;
}

/**
 * @brief Add to the waitgroup counter
 */
bool sys_waitgroup_add(sys_waitgroup_t *wg, int delta) {
  if (wg == NULL || !wg->init || delta < 0) {
    return false;
  }

  waitgroup_data_t *wgd = (waitgroup_data_t *)wg->ctx;

  // Lock the mutex for thread safety
  if (pthread_mutex_lock(&wgd->mutex) != 0) {
    return false;
  }

  // Add to the counter
  int old_value = wgd->counter;
  wgd->counter += delta;
  int new_value = wgd->counter;

  // Unlock the mutex
  pthread_mutex_unlock(&wgd->mutex);

  // Check for overflow or invalid state
  if (new_value < 0) {
    // This is an error condition - counter went negative
    // Try to restore the counter (though this is a programming error)
    pthread_mutex_lock(&wgd->mutex);
    wgd->counter = old_value;
    pthread_mutex_unlock(&wgd->mutex);
    return false;
  }

  return true;
}

/**
 * @brief Decrement the waitgroup counter
 */
bool sys_waitgroup_done(sys_waitgroup_t *wg) {
  if (wg == NULL || !wg->init) {
    return false;
  }

  waitgroup_data_t *wgd = (waitgroup_data_t *)wg->ctx;

  // Lock the mutex for thread safety
  if (pthread_mutex_lock(&wgd->mutex) != 0) {
    return false;
  }

  // Decrement the counter
  int old_value = wgd->counter;
  wgd->counter--;
  int new_value = wgd->counter;

  // If counter reached 0, wake up all waiting threads
  if (new_value == 0) {
    pthread_cond_broadcast(&wgd->cond);
  }

  // Unlock the mutex
  pthread_mutex_unlock(&wgd->mutex);

  // Check for underflow
  if (new_value < 0) {
    // This is an error condition - too many Done() calls
    // Try to restore the counter
    pthread_mutex_lock(&wgd->mutex);
    wgd->counter = old_value;
    pthread_mutex_unlock(&wgd->mutex);
    return false;
  }

  return true;
}

/**
 * @brief Finalize and cleanup a waitgroup - wait for completion then cleanup
 */
void sys_waitgroup_finalize(sys_waitgroup_t *wg) {
  if (wg == NULL || !wg->init) {
    return;
  }

  waitgroup_data_t *wgd = (waitgroup_data_t *)wg->ctx;

  // Lock the mutex
  if (pthread_mutex_lock(&wgd->mutex) != 0) {
    return;
  }

  // Wait while counter is greater than 0
  while (wgd->counter > 0) {
    if (pthread_cond_wait(&wgd->cond, &wgd->mutex) != 0) {
      pthread_mutex_unlock(&wgd->mutex);
      return;
    }
  }

  // Unlock the mutex
  pthread_mutex_unlock(&wgd->mutex);

  // Clean up pthread resources and mark as uninitialized
  pthread_cond_destroy(&wgd->cond);
  pthread_mutex_destroy(&wgd->mutex);
  wg->init = false;
  sys_memset(wg->ctx, 0, sizeof(wg->ctx));
}
