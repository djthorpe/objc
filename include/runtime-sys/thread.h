/**
 * @file thread.h
 * @brief Thread management and program execution.
 * @defgroup SystemThread Threading
 * @ingroup System
 *
 * Managing threads and program execution.
 *
 * This module declares types and functions for thread management, including
 * thread creation, synchronization, and communication. It is generally aware of
 * the number of "cores" available on the system, and allows creating threads
 * that can run on a specific core, or on any available core.
 *
 * However, it really depends on the platform's capabilities and requirements.
 * as to whether you should call sys_thread_create() or
 * sys_thread_create_on_core(). On the Pico platform, for example,
 * you should use sys_thread_create_on_core() to ensure the thread runs
 * on a specific core. If you call this method when an existing thread is
 * already running on that core, the core will be reset and the new thread
 * will be scheduled to run on that core, so it's important to ensure
 * that there isn't an existing thread already running on the specified core
 * before calling this function.
 *
 * The implementation of threads is like Go's goroutines, where the
 * execution threads are "fire and forget" and do not require joining. If
 * you wish to communicate between threads, use the event queue. If you wish
 * to co-ordinate when threads are running or to synchronize their execution,
 * you can use the provided synchronization primitives like waitgroups.
 *
 * There are also methods for suspending the current thread for a specified
 * duration, and for halting execution of the process immediately, in case
 * of catastrophic failure.
 *
 * @example pico/multicore/main.c
 * This is a complete example showing multicore programming with waitgroup
 * synchronization on the Pico platform.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Thread function signature
 * @ingroup SystemThread
 *
 * Function signature for thread entry points. The function receives
 * a single void pointer argument and should not return a value.
 * The thread terminates when this function returns.
 */
typedef void (*sys_thread_func_t)(void *arg);

/**
 * @brief Returns the number of CPU cores available on the host system.
 * @ingroup SystemThread
 * @return The number of CPU cores available on the system. Returns 1 if the
 *         number of cores cannot be determined or if the system has only one
 *         core.
 *
 * This function queries the system to determine the total number of processing
 * cores available.
 */
extern uint8_t sys_thread_numcores(void);

/**
 * @brief Create a thread on any available core
 * @ingroup SystemThread
 * @param func Function to execute in the new thread
 * @param arg Argument to pass to the thread function
 * @return true if thread was created successfully, false if no cores available
 * or error
 *
 * Creates a new thread that executes the specified function with the given
 * argument. The thread runs independently and terminates when the function
 * returns. No cleanup or joining is required - the thread is fire-and-forget.
 * On systems with multiple cores, the thread may be scheduled on any available
 * core.
 */
bool sys_thread_create(sys_thread_func_t func, void *arg);

/**
 * @brief Create a thread on a specific core
 * @ingroup SystemThread
 * @param func Function to execute in the new thread
 * @param arg Argument to pass to the thread function
 * @param core Core number to run the thread on (0-based)
 * @return true if thread was created on the specified core, false if core
 * unavailable or error
 *
 * Creates a new thread that executes the specified function on a specific CPU
 * core. The thread runs independently and terminates when the function returns.
 * If the specified core is not available or already in use, the function
 * returns false. Core 0 is typically the main/boot core.
 */
bool sys_thread_create_on_core(sys_thread_func_t func, void *arg, uint8_t core);

/**
 * @brief Get the CPU core number the current thread is running on
 * @ingroup SystemThread
 * @return The core number (0-based) that the current thread is executing on.
 *         Returns 0 if the core cannot be determined or on single-core systems.
 *
 * This function queries the system to determine which CPU core the calling
 * thread is currently scheduled on. Note that threads may migrate between
 * cores, so this value may change over time unless thread affinity is set.
 */
extern uint8_t sys_thread_core(void);

/**
 * @brief Pauses the execution of the current thread for a specified time.
 * @ingroup SystemThread
 * @param msec The number of milliseconds to sleep.
 */
extern void sys_sleep(int32_t msec);

#ifdef __cplusplus
}
#endif
