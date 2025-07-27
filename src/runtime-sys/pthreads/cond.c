#include <errno.h>
#include <pthread.h>
#include <runtime-sys/sys.h>
#include <time.h>

// Ensure the embedded buffer is large enough for pthread_cond_t
_Static_assert(sizeof(pthread_cond_t) <= SYS_COND_CTX_SIZE,
               "SYS_COND_CTX_SIZE too small for pthread_cond_t");

// Ensure proper alignment for pthread_cond_t
_Static_assert(
    _Alignof(pthread_cond_t) <= _Alignof(uint64_t),
    "pthread_cond_t alignment requirements too strict for embedded buffer");

sys_cond_t sys_cond_init(void) {
  sys_cond_t cond;
  cond.init = false;
  sys_memset(cond.ctx, 0, sizeof(cond.ctx));

  // Get pointer to the embedded pthread_cond_t
  pthread_cond_t *pc = (pthread_cond_t *)cond.ctx;

  // Initialize the pthread condition variable
  int result = pthread_cond_init(pc, NULL);
  if (result == 0) {
    cond.init = true;
  }

  return cond;
}

bool sys_cond_wait(sys_cond_t *cond, sys_mutex_t *mutex) {
  if (cond == NULL || !cond->init || mutex == NULL || !mutex->init) {
    return false;
  }

  pthread_cond_t *pc = (pthread_cond_t *)cond->ctx;
  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;

  // pthread_cond_wait returns 0 on success
  int result = pthread_cond_wait(pc, pm);
  return result == 0;
}

bool sys_cond_timedwait(sys_cond_t *cond, sys_mutex_t *mutex,
                        uint32_t timeout_ms) {
  if (cond == NULL || !cond->init || mutex == NULL || !mutex->init) {
    return false;
  }

  pthread_cond_t *pc = (pthread_cond_t *)cond->ctx;
  pthread_mutex_t *pm = (pthread_mutex_t *)mutex->ctx;

  // If timeout is 0, behave like regular wait
  if (timeout_ms == 0) {
    return sys_cond_wait(cond, mutex);
  }

  // Calculate absolute timeout
  struct timespec abs_timeout;
  clock_gettime(CLOCK_REALTIME, &abs_timeout);

  // Add timeout milliseconds
  abs_timeout.tv_sec += timeout_ms / 1000;
  abs_timeout.tv_nsec += (timeout_ms % 1000) * 1000000;

  // Handle nanosecond overflow
  if (abs_timeout.tv_nsec >= 1000000000) {
    abs_timeout.tv_sec += 1;
    abs_timeout.tv_nsec -= 1000000000;
  }

  // Return true if signaled, false if timeout or error
  return pthread_cond_timedwait(pc, pm, &abs_timeout) == 0;
}

bool sys_cond_signal(sys_cond_t *cond) {
  if (cond == NULL || !cond->init) {
    return false;
  }

  pthread_cond_t *pc = (pthread_cond_t *)cond->ctx;
  return pthread_cond_signal(pc) == 0;
}

bool sys_cond_broadcast(sys_cond_t *cond) {
  if (cond == NULL || !cond->init) {
    return false;
  }

  pthread_cond_t *pc = (pthread_cond_t *)cond->ctx;
  return pthread_cond_broadcast(pc) == 0;
}

void sys_cond_finalize(sys_cond_t *cond) {
  if (cond == NULL || !cond->init) {
    return;
  }

  pthread_cond_t *pc = (pthread_cond_t *)cond->ctx;
  pthread_cond_destroy(pc);

  // Clear the structure
  cond->init = false;
  sys_memset(cond->ctx, 0, sizeof(cond->ctx));
}
