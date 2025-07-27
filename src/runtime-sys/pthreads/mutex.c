#include <pthread.h>
#include <runtime-sys/sys.h>

// Ensure the embedded buffer is large enough for pthread_mutex_t
_Static_assert(sizeof(pthread_mutex_t) <= SYS_MUTEX_CTX_SIZE,
               "SYS_MUTEX_CTX_SIZE too small for pthread_mutex_t");

sys_mutex_t sys_mutex_init(void) {
  sys_mutex_t mutex;
  mutex.init = false;
  sys_memset(mutex.ctx, 0, sizeof(mutex.ctx));

  // Get pointer to the embedded pthread_mutex_t
  pthread_mutex_t *pm = (pthread_mutex_t *)mutex.ctx;

  // Initialize the pthread mutex with error-checking attributes
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  int result = pthread_mutex_init(pm, &attr);
  pthread_mutexattr_destroy(&attr);
  if (result == 0) {
    // Set init flag to indicate successful initialization
    mutex.init = true;
  }

  return mutex;
}

bool sys_mutex_lock(sys_mutex_t *mutex) {
  if (mutex == NULL || !mutex->init) {
    return false;
  }
  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;
  return pthread_mutex_lock(pm) == 0;
}

bool sys_mutex_trylock(sys_mutex_t *mutex) {
  if (mutex == NULL || !mutex->init) {
    return false;
  }
  // EBUSY means the mutex is already locked, which is expected behavior
  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;
  return pthread_mutex_trylock(pm) == 0;
}

bool sys_mutex_unlock(sys_mutex_t *mutex) {
  if (mutex == NULL || !mutex->init) {
    return false;
  }
  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;
  // Unlock the mutex and log an error if it fails
  int result = pthread_mutex_unlock(pm);
  if (result != 0) {
    sys_log_error("pthread_mutex_unlock failed with error code: %d", result);
  }
  return true;
}

void sys_mutex_finalize(sys_mutex_t *mutex) {
  if (mutex == NULL || !mutex->init) {
    return;
  }

  // Destroy the pthread mutex
  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;
  pthread_mutex_destroy(pm);

  // Clear the structure
  mutex->init = false;
  sys_memset(mutex->ctx, 0, sizeof(mutex->ctx));
}
