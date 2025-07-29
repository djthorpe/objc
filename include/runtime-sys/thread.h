/**
 * @file thread.h
 * @brief Thread management and program execution.
 * @defgroup SystemThread Threading
 * @ingroup System
 *
 * Managing threads and program execution.
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

/**
 * @brief Aborts the current process immediately.
 * @ingroup SystemThread
 * @note This function does not return to the caller.
 *
 *  This function terminates the current process abnormally and
 *  immediately. It does not perform any cleanup operations and
 *  does not call exit handlers or destructors.
 */
extern void sys_abort(void);

/**
 * @brief Prints a formatted panic message and aborts the process.
 * @ingroup SystemThread
 * @param fmt A printf-style format string specifying the panic message.
 * @param ... Additional arguments corresponding to format specifiers in fmt.
 * @note This function does not return to the caller.
 */
extern void sys_panicf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
