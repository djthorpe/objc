#include <runtime-sys/printf.h>
#include <runtime-sys/sys.h>

typedef struct {
  sys_atomic_t *counter;
  sys_atomic_t *done; // 0 = running, 1 = done
  uint32_t iterations;
  int direction; // +1 = inc, -1 = dec
} worker_args_t;

static void worker_fn(void *arg) {
  worker_args_t *w = (worker_args_t *)arg;
#ifdef SYSTEM_NAME_PICO
  sys_printf("sys_17: worker start dir=%d iterations=%u\n", w->direction,
             (unsigned)w->iterations);
#endif
  for (uint32_t i = 0; i < w->iterations; i++) {
    if (w->direction > 0) {
      (void)sys_atomic_inc(w->counter);
    } else {
      (void)sys_atomic_dec(w->counter);
    }

#ifdef SYSTEM_NAME_PICO
    // Occasionally yield to avoid starving other work on Pico
    if ((i & 0x3FFFu) == 0) {
      sys_sleep(0);
    }
    // Sparse progress log
    if ((i % 50000u) == 0) {
      sys_printf("sys_17: worker dir=%d i=%u counter=%u\n", w->direction,
                 (unsigned)i, (unsigned)sys_atomic_get(w->counter));
    }
#endif
  }
  sys_atomic_set(w->done, 1);
#ifdef SYSTEM_NAME_PICO
  sys_printf("sys_17: worker end dir=%d done=1 counter=%u\n", w->direction,
             (unsigned)sys_atomic_get(w->counter));
#endif
}

int test_sys_17(void) {
  // Baseline API checks
  sys_atomic_t a;
  sys_atomic_init(&a, 0);
  if (sys_atomic_get(&a) != 0)
    return 1;
  sys_atomic_set(&a, 41);
  if (sys_atomic_get(&a) != 41)
    return 2;
  if (sys_atomic_inc(&a) != 42)
    return 3;
  if (sys_atomic_dec(&a) != 41)
    return 4;

  // Concurrent increment/decrement
  sys_atomic_set(&a, 0);
  const uint32_t iterations = 100000;
#ifdef SYSTEM_NAME_PICO
  sys_printf("sys_17: starting concurrency test, iterations=%u\n",
             (unsigned)iterations);
#endif

  sys_atomic_t inc_done;
  sys_atomic_t dec_done;
  sys_atomic_init(&inc_done, 0);
  sys_atomic_init(&dec_done, 0);
  worker_args_t inc_args = {&a, &inc_done, iterations, +1};
  worker_args_t dec_args = {&a, &dec_done, iterations, -1};

#ifdef SYSTEM_NAME_PICO
  // Run dec on core 1, inc on main thread
  sys_printf("sys_17: creating decrement thread on core 1\n");
  if (!sys_thread_create_on_core(worker_fn, &dec_args, 1)) {
    sys_printf("sys_17: failed to create decrement thread on core 1\n");
    return 5;
  }
  sys_printf("sys_17: starting increment worker on core 0\n");
  worker_fn(&inc_args);
  sys_printf("sys_17: increment worker finished, waiting for decrement\n");

  int timeout_ms = 5000;
  int last_log = timeout_ms;
  while (!sys_atomic_get(&dec_done) && timeout_ms-- > 0) {
    sys_sleep(1);
    if ((timeout_ms % 500) == 0 && timeout_ms != last_log) {
      last_log = timeout_ms;
      sys_printf("sys_17: waiting... dec_done=%u counter=%u remaining=%dms\n",
                 (unsigned)sys_atomic_get(&dec_done),
                 (unsigned)sys_atomic_get(&a), timeout_ms);
    }
  }
  if (!sys_atomic_get(&dec_done)) {
    sys_printf("sys_17: decrement thread timeout\n");
    return 6;
  }
#else
  // Desktop: both threads
  bool inc_ok = sys_thread_create(worker_fn, &inc_args);
  bool dec_ok = sys_thread_create(worker_fn, &dec_args);
  if (!inc_ok || !dec_ok) {
    sys_printf("sys_17: failed to create threads (inc=%d, dec=%d)\n",
               (int)inc_ok, (int)dec_ok);
    return 7;
  }
  int timeout_ms = 2000;
  while ((!(sys_atomic_get(&inc_done) && sys_atomic_get(&dec_done))) &&
         timeout_ms-- > 0) {
    sys_sleep(1);
  }
  if (!(sys_atomic_get(&inc_done) && sys_atomic_get(&dec_done))) {
    sys_printf("sys_17: threads timeout (inc_done=%u, dec_done=%u)\n",
               sys_atomic_get(&inc_done), sys_atomic_get(&dec_done));
    return 8;
  }
#endif

  // Expect 0 after equal increments and decrements
  uint32_t final = sys_atomic_get(&a);
#ifdef SYSTEM_NAME_PICO
  sys_printf("sys_17: final counter=%u (expected 0)\n", (unsigned) final);
#endif
  if (final != 0) {
    sys_printf("sys_17: final value mismatch: expected 0, got %u\n", final);
    return 9;
  }
  return 0;
}
