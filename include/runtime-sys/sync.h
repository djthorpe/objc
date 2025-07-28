/**
 * @file sync.h
 * @brief Defines the ability to create and respond to synchronization
 * primitives.
 *
 * This file declares various system methods for synchronization management.
 *
 * @example pico/multicore/main.c
 * This is a complete example showing multicore programming with waitgroup
 * synchronization on the Pico platform.
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Buffer size for platform-specific mutex context data */
#define SYS_MUTEX_CTX_SIZE 64 // Adjust based on platform requirements

/** @brief Buffer size for platform-specific condition variable context data */
#define SYS_COND_CTX_SIZE 64 // Adjust based on platform requirements

/** @brief Buffer size for platform-specific waitgroup context data */
#define SYS_WAITGROUP_CTX_SIZE 128 // Larger to hold mutex + cond + counter

/**
 * @brief Mutex context structure.
 * @ingroup System
 *
 * Contains the state and configuration for mutex operations.
 */
typedef struct {
  bool init; ///< Indicates if the mutex is initialized
  union {
    uint8_t
        ctx[SYS_MUTEX_CTX_SIZE]; ///< Embedded buffer for platform-specific data
    uint64_t align;              ///< Force 8-byte alignment
  };
} sys_mutex_t;

/**
 * @brief Condition variable context structure.
 * @ingroup System
 *
 * Contains the state and configuration for condition variable operations.
 */
typedef struct {
  bool init; ///< Indicates if the condition variable is initialized
  union {
    uint8_t
        ctx[SYS_COND_CTX_SIZE]; ///< Embedded buffer for platform-specific data
    uint64_t align;             ///< Force 8-byte alignment
  };
} sys_cond_t;

/**
 * @brief Waitgroup context structure.
 * @ingroup System
 *
 * Contains the state and configuration for waitgroup operations.
 * A waitgroup allows one goroutine to wait for a collection of goroutines
 * to finish executing.
 */
typedef struct {
  bool init; ///< Indicates if the waitgroup is initialized
  union {
    uint8_t ctx[SYS_WAITGROUP_CTX_SIZE]; ///< Embedded buffer for
                                         ///< platform-specific data
    uint64_t align;                      ///< Force 8-byte alignment
  };
} sys_waitgroup_t;

/**
 * @brief Initialize a new mutex
 * @ingroup System
 * @return Initialized mutex structure
 *
 * Creates and initializes a new mutex for thread synchronization.
 * The mutex is initially unlocked and ready for use. The returned mutex must
 * be finalized with sys_mutex_finalize()
 */
sys_mutex_t sys_mutex_init(void);

/**
 * @brief Lock a mutex, by blocking
 * @ingroup System
 * @param mutex Pointer to the mutex to lock
 * @return true if the mutex was successfully locked, false on error
 *
 * Attempts to lock the specified mutex. If the mutex is already locked
 * by another thread, this function will block until the mutex becomes
 * available. Every successful lock must be paired with sys_mutex_unlock()
 * Attempting to lock an already owned mutex may result in deadlock
 */
bool sys_mutex_lock(sys_mutex_t *mutex);

/**
 * @brief Try to lock a mutex
 * @ingroup System
 * @param mutex Pointer to the mutex to try locking
 * @return true if the mutex was successfully locked, false if already locked
 * or on error
 *
 * Attempts to lock the specified mutex without blocking. If the mutex
 * is already locked, this function returns immediately with false.
 * Every successful trylock must be paired with sys_mutex_unlock()
 */
bool sys_mutex_trylock(sys_mutex_t *mutex);

/**
 * @brief Unlock a mutex
 * @ingroup System
 * @param mutex Pointer to the mutex to unlock
 * @return true if the mutex was successfully unlocked, false on error
 *
 * Releases a previously acquired mutex lock, allowing other threads
 * to acquire the mutex. Only the thread that locked the mutex should unlock
 * it.
 */
bool sys_mutex_unlock(sys_mutex_t *mutex);

/**
 * @brief Finalize and cleanup a mutex
 * @ingroup System
 * @param mutex Pointer to the mutex to finalize
 *
 * Releases all resources associated with the mutex and renders it
 * unusable. The mutex should not be locked when this function is called.
 */
void sys_mutex_finalize(sys_mutex_t *mutex);

/**
 * @brief Initialize a new condition variable
 * @ingroup System
 * @return Initialized condition variable structure
 *
 * Creates and initializes a new condition variable for thread synchronization.
 * The condition variable is ready for use with wait/signal operations.
 * The returned condition variable must be finalized with sys_cond_finalize()
 */
sys_cond_t sys_cond_init(void);

/**
 * @brief Wait on a condition variable
 * @ingroup System
 * @param cond Pointer to the condition variable to wait on
 * @param mutex Pointer to the mutex that must be locked by the calling thread
 * @return true if the wait completed successfully, false on error
 *
 * Atomically releases the mutex and waits for the condition variable to be
 * signaled. Upon return, the mutex is reacquired. The mutex must be locked
 * by the calling thread before calling this function.
 */
bool sys_cond_wait(sys_cond_t *cond, sys_mutex_t *mutex);

/**
 * @brief Wait on a condition variable with timeout
 * @ingroup System
 * @param cond Pointer to the condition variable to wait on
 * @param mutex Pointer to the mutex that must be locked by the calling thread
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return true if signaled, false if timeout or error
 *
 * Like sys_cond_wait() but returns after timeout_ms milliseconds if not
 * signaled. Returns true if signaled, false if timeout or error occurred.
 */
bool sys_cond_timedwait(sys_cond_t *cond, sys_mutex_t *mutex,
                        uint32_t timeout_ms);

/**
 * @brief Signal one waiting thread
 * @ingroup System
 * @param cond Pointer to the condition variable to signal
 * @return true if successful, false on error
 *
 * Wakes up one thread waiting on the condition variable. If no threads
 * are waiting, this function has no effect. The associated mutex should
 * be locked when calling this function for predictable behavior.
 */
bool sys_cond_signal(sys_cond_t *cond);

/**
 * @brief Signal all waiting threads
 * @ingroup System
 * @param cond Pointer to the condition variable to broadcast
 * @return true if successful, false on error
 *
 * Wakes up all threads waiting on the condition variable. If no threads
 * are waiting, this function has no effect. The associated mutex should
 * be locked when calling this function for predictable behavior.
 */
bool sys_cond_broadcast(sys_cond_t *cond);

/**
 * @brief Finalize and cleanup a condition variable
 * @ingroup System
 * @param cond Pointer to the condition variable to finalize
 *
 * Releases all resources associated with the condition variable and renders
 * it unusable. No threads should be waiting on the condition variable when
 * this function is called.
 */
void sys_cond_finalize(sys_cond_t *cond);

/**
 * @brief Initialize a new waitgroup
 * @ingroup System
 * @return Initialized waitgroup structure
 *
 * Creates and initializes a new waitgroup for thread synchronization.
 * The waitgroup counter starts at 0. The returned waitgroup must be
 * finalized with sys_waitgroup_finalize()
 */
sys_waitgroup_t sys_waitgroup_init(void);

/**
 * @brief Add to the waitgroup counter
 * @ingroup System
 * @param wg Pointer to the waitgroup
 * @param delta Number to add to the counter (must be positive)
 * @return true if successful, false on error
 *
 * Increments the waitgroup counter by delta. This should be called before
 * starting goroutines that the waitgroup should wait for.
 */
bool sys_waitgroup_add(sys_waitgroup_t *wg, int delta);

/**
 * @brief Decrement the waitgroup counter
 * @ingroup System
 * @param wg Pointer to the waitgroup
 * @return true if successful, false on error
 *
 * Decrements the waitgroup counter by 1. This should be called when a
 * goroutine finishes its work. If the counter reaches 0, all threads
 * waiting on sys_waitgroup_wait() will be woken up.
 */
bool sys_waitgroup_done(sys_waitgroup_t *wg);

/**
 * @brief Finalize and cleanup a waitgroup - wait for completion then cleanup
 * @ingroup System
 * @param wg Pointer to the waitgroup to finalize
 *
 * Blocks until the waitgroup counter reaches 0, then releases all resources
 * associated with the waitgroup and renders it unusable. The waitgroup counter
 * should reach 0 through done() calls from worker threads.
 */
void sys_waitgroup_finalize(sys_waitgroup_t *wg);

#ifdef __cplusplus
}
#endif