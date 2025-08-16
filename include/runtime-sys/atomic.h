/**
 * @file atomic.h
 * @brief Simple atomic uint32_t API (get, set, increment, decrement).
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

// Some embedded targets (e.g., ARMv6-M / RP2040) have no lock-free atomics.
// Clang then warns that builtins may not be lock-free (-Watomic-alignment).
// We accept the fallback and suppress the warning locally.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Watomic-alignment"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Underlying atomic uint32_t value
 * @ingroup SystemSync
 * @headerfile atomic.h runtime-sys/sys.h
 *
 * Do not access the value directly; use the sys_atomic_* APIs.
 * in order to maintain thread safety.
 */
typedef struct sys_atomic_t {
  uint32_t value;
} sys_atomic_t __attribute__((aligned(4)));

/**
 * @brief Initialize an atomic with an initial value.
 * @ingroup SystemSync
 *
 * @param a Pointer to the atomic value to initialize (must be non-NULL).
 * @param initial The initial 32-bit value to store.
 *
 * This performs a relaxed store; it does not impose ordering with
 * other memory operations. Use only as an initialization step before
 * the atomic is shared with other threads.
 */
static inline void sys_atomic_init(sys_atomic_t *a, uint32_t initial) {
  __atomic_store_n(&a->value, initial, __ATOMIC_RELAXED);
}

/**
 * @brief Load the current value atomically.
 * @ingroup SystemSync
 *
 * @param a Pointer to the atomic value (must be non-NULL).
 * @return The current 32-bit value.
 *
 * This is a relaxed load; it does not establish synchronization with
 * other memory operations. If you need acquire semantics, introduce
 * an explicit fence in the caller.
 */
static inline uint32_t sys_atomic_get(const sys_atomic_t *a) {
  return __atomic_load_n(&a->value, __ATOMIC_RELAXED);
}

/**
 * @brief Store a new value atomically.
 * @ingroup SystemSync
 *
 * @param a Pointer to the atomic value (must be non-NULL).
 * @param v The value to store.
 *
 * This is a relaxed store; it does not order or publish other writes.
 * If you need release semantics, introduce an explicit fence in the caller.
 */
static inline void sys_atomic_set(sys_atomic_t *a, uint32_t v) {
  __atomic_store_n(&a->value, v, __ATOMIC_RELAXED);
}

/**
 * @brief Atomically increment and return the new value.
 * @ingroup SystemSync
 *
 * @param a Pointer to the atomic value (must be non-NULL).
 * @return The incremented value after the operation.
 *
 * This uses relaxed ordering and wraps modulo 2^32.
 */
static inline uint32_t sys_atomic_inc(sys_atomic_t *a) {
  return __atomic_add_fetch(&a->value, 1, __ATOMIC_RELAXED);
}

/**
 * @brief Atomically decrement and return the new value.
 * @ingroup SystemSync
 *
 * @param a Pointer to the atomic value (must be non-NULL).
 * @return The decremented value after the operation.
 *
 * This uses relaxed ordering and wraps modulo 2^32.
 */
static inline uint32_t sys_atomic_dec(sys_atomic_t *a) {
  return __atomic_sub_fetch(&a->value, 1, __ATOMIC_RELAXED);
}

/**
 * @brief Atomically set bits (OR with mask).
 * @ingroup SystemSync
 *
 * @param a Pointer to the atomic value (must be non-NULL).
 * @param mask Bit mask of bits to set.
 *
 * Performs a relaxed fetch-or; no ordering is implied.
 */
static inline void sys_atomic_set_bits(sys_atomic_t *a, uint32_t mask) {
  (void)__atomic_fetch_or(&a->value, mask, __ATOMIC_RELAXED);
}

/**
 * @brief Atomically clear bits (AND with ~mask).
 * @ingroup SystemSync
 *
 * @param a Pointer to the atomic value (must be non-NULL).
 * @param mask Bit mask of bits to clear.
 *
 * Performs a relaxed fetch-and with the complement of mask; no ordering is
 * implied.
 */
static inline void sys_atomic_clear_bits(sys_atomic_t *a, uint32_t mask) {
  (void)__atomic_fetch_and(&a->value, ~mask, __ATOMIC_RELAXED);
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifdef __cplusplus
}
#endif
