/**
 * @file timer.h
 * @brief Creating periodic and one-shot timers.
 * @defgroup SystemTimer Timers
 * @ingroup System
 *
 * Periodic and one-shot timers for scheduling tasks.
 *
 * @example pico/runloop/main.c
 * An example of multiple producers and
 * consumers using the event queue in a runloop style on the Pico platform.
 *
 * @example clock/main.c
 * Uses a timer to print out the current system time
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of the timer context buffer.
 * @ingroup SystemTimer
 *
 * This defines the size of the context buffer used for timer operations.
 * It should be large enough to hold any platform-specific timer context.
 */
#define SYS_TIMER_CTX_SIZE 32

/**
 * @brief Timer context structure.
 * @ingroup SystemTimer
 *
 * Contains the state and configuration for timer operations.
 */
typedef struct sys_timer_t {
  void (*callback)(struct sys_timer_t *);
  uint32_t interval; // Timer interval in milliseconds
  void *userdata;
  union {
    void *ptr; // Pointer to the timer context (platform-specific)
    uint8_t
        ctx[SYS_TIMER_CTX_SIZE]; // Context buffer large enough for any platform
  } ctx;
} sys_timer_t;

/**
 * @brief Initializes a new timer context.
 * @ingroup SystemTimer
 * @param interval_ms The periodic interval for the timer in milliseconds.
 * @param userdata Optional user data to pass to the callback.
 * @param callback The function to call on each timer event.
 * @return A new sys_timer_t instance initialized with the specified parameters.
 *
 * This function returns a sys_timer_t instance for the
 * specified timer configuration, ready for use in starting and stopping the
 * timer. Use sys_timer_start() to start the timer and
 * sys_timer_finalize() to stop and clean up the timer. If you want to
 * define a one-shot timer, call sys_timer_finalize() in your callback
 * function.
 */
extern sys_timer_t sys_timer_init(uint32_t interval_ms, void *userdata,
                                  void (*callback)(sys_timer_t *));

/**
 * @brief Starts a timer.
 * @ingroup SystemTimer
 * @param timer The timer context to start.
 * @return true on success, false on failure.
 *
 * This function starts the timer with the specified interval and callback. If
 * the timer is invalid, or cannot be started, the function returns false.
 */
extern bool sys_timer_start(sys_timer_t *timer);

/**
 * @brief Cancels and finalizes a timer.
 * @ingroup SystemTimer
 * @param timer The timer context to finalize.
 * @return true on success, false on failure.
 * This function stops the timer and cleans up any resources associated with it.
 * After calling this function, the timer context becomes invalid and should not
 * be used again. To use a timer with the same configuration, initialize a new
 * timer using sys_timer_init().
 */
extern bool sys_timer_finalize(sys_timer_t *timer);

#ifdef __cplusplus
}
#endif
