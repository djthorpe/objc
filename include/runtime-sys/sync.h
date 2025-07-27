/**
 * @file sync.h
 * @brief Defines the ability to create and respond to synchronization
 * primitives.
 *
 * This file declares various system methods for synchronization management.
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Buffer size for platform-specific mutex context data */
#define SYS_MUTEX_CTX_SIZE 128 // Adjust based on platform requirements

/**
 * @brief Mutex context structure.
 * @ingroup System
 *
 * Contains the state and configuration for mutex operations.
 */
typedef struct {
  void *ptr; ///< Pointer for platforms using pointer-based mutexes
  uint8_t ctx[SYS_MUTEX_CTX_SIZE]; ///< Embedded buffer for
                                   ///< platform-specific data
} sys_mutex_t;

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

#ifdef __cplusplus
}
#endif