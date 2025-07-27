#include <hardware/sync.h>
#include <pico/sync.h>
#include <runtime-sys/sys.h>

// Ensure the embedded buffer is large enough for mutex_t
_Static_assert(sizeof(mutex_t) <= SYS_MUTEX_CTX_SIZE,
               "SYS_MUTEX_CTX_SIZE too small for mutex_t");

sys_mutex_t sys_mutex_init(void) {
  sys_mutex_t mutex;
  mutex.init = false;
  sys_memset(mutex.ctx, 0, sizeof(mutex.ctx));

  // Initialize the Pico SDK mutex
  mutex_t *pm = (mutex_t *)mutex.ctx;
  mutex_init(pm);
  if (mutex_is_initialized(pm)) {
    mutex.init = true;
  }

  return mutex;
}

bool sys_mutex_lock(sys_mutex_t *mutex) {
  if (mutex == NULL || !mutex->init) {
    return false;
  }

  mutex_t *pm = (mutex_t *)mutex->ctx;
  if (!mutex_is_initialized(pm)) {
    return false;
  }
  mutex_enter_blocking(pm);
  return true;
}

bool sys_mutex_trylock(sys_mutex_t *mutex) {
  if (mutex == NULL || !mutex->init) {
    return false;
  }

  mutex_t *pm = (mutex_t *)mutex->ctx;
  if (!mutex_is_initialized(pm)) {
    return false;
  }
  return mutex_try_enter(pm, NULL);
}

bool sys_mutex_unlock(sys_mutex_t *mutex) {
  if (mutex == NULL || !mutex->init) {
    return false;
  }

  mutex_t *pm = (mutex_t *)mutex->ctx;
  if (!mutex_is_initialized(pm)) {
    return false;
  }
  mutex_exit(pm);
  return true;
}

void sys_mutex_finalize(sys_mutex_t *mutex) {
  if (mutex == NULL || !mutex->init) {
    return;
  }
  mutex->init = false;
  sys_memset(mutex->ctx, 0, sizeof(mutex->ctx));
}
