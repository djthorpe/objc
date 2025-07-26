/**
 * @file thread.h
 * @brief Defines thread management functions.
 *
 * This file declares various system methods for managing threads.
 */
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the number of CPU cores available on the host system.
 * @return The number of CPU cores available on the system. Returns 1 if the
 *         number of cores cannot be determined or if the system has only one
 *         core.
 *
 * This function queries the system to determine the total number of processing
 * cores available.
 */
extern uint8_t sys_thread_numcores(void);

/**
 * @brief Pauses the execution of the current thread for a specified time.
 * @param msec The number of milliseconds to sleep.
 */
extern void sys_sleep(int32_t msec);

#ifdef __cplusplus
}
#endif
