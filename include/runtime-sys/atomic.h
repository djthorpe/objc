/**
 * @file atomic.h
 * @brief Simple atomic uint32_t API (get, set, increment, decrement).
 * @ingroup SystemSync
 *
 * Implements atomic values which can be safely updated across threads.
 *
 * This API allows you to maintain a uint32_t value across threads safely.
 * The operations are to get, set, increment and decrement. For the increment
 * and decrement operations return the new value.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sys_atomic {
  // Plain storage; operations use __atomic built-ins for portability
  uint32_t value;
} sys_atomic_t;

/**
 * @brief Initialize an atomic with an initial value.
 */
static inline void sys_atomic_init(sys_atomic_t *a, uint32_t initial) {
  __atomic_store_n(&a->value, initial, __ATOMIC_RELAXED);
}

/**
 * @brief Load the current value atomically.
 */
static inline uint32_t sys_atomic_get(const sys_atomic_t *a) {
  return __atomic_load_n(&a->value, __ATOMIC_RELAXED);
}

/**
 * @brief Store a new value atomically.
 */
static inline void sys_atomic_set(sys_atomic_t *a, uint32_t v) {
  __atomic_store_n(&a->value, v, __ATOMIC_RELAXED);
}

/**
 * @brief Atomically increment and return the new value.
 */
static inline uint32_t sys_atomic_inc(sys_atomic_t *a) {
  return __atomic_add_fetch(&a->value, 1, __ATOMIC_RELAXED);
}

/**
 * @brief Atomically decrement and return the new value.
 */
static inline uint32_t sys_atomic_dec(sys_atomic_t *a) {
  return __atomic_sub_fetch(&a->value, 1, __ATOMIC_RELAXED);
}

#ifdef __cplusplus
}
#endif
