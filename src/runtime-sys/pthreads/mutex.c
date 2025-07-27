#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <runtime-sys/sync.h>
#include <runtime-sys/sys.h>
#include <string.h>

// Ensure the embedded buffer is large enough for pthread_mutex_t
_Static_assert(sizeof(pthread_mutex_t) <= SYS_MUTEX_CTX_SIZE,
               "SYS_MUTEX_CTX_SIZE too small for pthread_mutex_t");

// Simple counter to give each mutex a unique ID
static uintptr_t mutex_counter = 1;

sys_mutex_t sys_mutex_init(void) {
  sys_mutex_t mutex = {0};

  // Get pointer to the embedded pthread_mutex_t
  pthread_mutex_t *pm = (pthread_mutex_t *)mutex.ctx;

  // Create error-checking mutex attributes for better error detection
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

  // Initialize the pthread mutex with error-checking attributes
  int result = pthread_mutex_init(pm, &attr);

  // Clean up attributes
  pthread_mutexattr_destroy(&attr);

  if (result == 0) {
    // Set ptr to a unique value to indicate successful initialization
    mutex.ptr = (void *)mutex_counter++;
  }

  return mutex;
}

bool sys_mutex_lock(sys_mutex_t *mutex) {
  if (mutex == NULL) {
    return false;
  }

  if (mutex->ptr == NULL) {
    return false;
  }

  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;
  int result = pthread_mutex_lock(pm);

  return (result == 0);
}

bool sys_mutex_trylock(sys_mutex_t *mutex) {
  if (mutex == NULL || mutex->ptr == NULL) {
    return false;
  }

  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;
  int result = pthread_mutex_trylock(pm);

  // EBUSY means the mutex is already locked, which is expected behavior
  return (result == 0);
}

bool sys_mutex_unlock(sys_mutex_t *mutex) {
  if (mutex == NULL || mutex->ptr == NULL) {
    return false;
  }

  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;
  int result = pthread_mutex_unlock(pm);

  return (result == 0);
}

void sys_mutex_finalize(sys_mutex_t *mutex) {
  if (mutex == NULL || mutex->ptr == NULL) {
    return;
  }

  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;

  // Destroy the pthread mutex
  pthread_mutex_destroy(pm);

  // Clear the structure
  mutex->ptr = NULL;
  sys_memset(mutex->ctx, 0, sizeof(mutex->ctx));
}
