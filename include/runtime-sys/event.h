/**
 * @file event.h
 * @brief Defines event queue functionality for producer/consumer patterns.
 * @defgroup SystemEvents Events
 * @ingroup System
 *
 * Defines event queue functionality for producer/consumer patterns.
 *
 * This file declares types and functions for thread-safe event queues that
 * support multiple producers and consumers with peek-before-acquire semantics.
 *
 * It is intended to support event channels, for a single producer and
 * single consumer, all the way to multiple producers and consumers.
 *
 * The event type sys_event_t is a simple pointer type that can be used to
 * hold any data, but that data is not managed by the queue itself; If you
 * want to manage the data, you must do so outside the queue. The queue supports
 * operations to push events, pop events, and peek at events without removing
 * them.
 *
 * If you peek events you also need to acquire the queue lock before doing so,
 * and release it after you are done. This is to ensure that the queue state
 * is not modified while you are inspecting it.
 *
 * You set a queue capacity when you initialize it, and the queue will
 * use a circular buffer to store events. When the buffer is full, new
 * events will overwrite the oldest events if you use the sys_event_queue_push()
 * function. If you want to avoid overwriting, you can use
 * sys_event_queue_try_push() function instead, which will return false
 * if the queue is full.
 *
 * When you wish to shutdown the queue, you can call sys_event_queue_finalize().
 * Consumers will be woken up and any further attempts to push events will fail,
 * but existing events can still be popped, until the queue is empty.
 *
 * @example pico/simplequeue/main.c
 * This is a complete example showing how a simple use of an event queue works
 * in a multicore programming context on the Pico platform.
 *
 * @example pico/runloop/main.c
 * This is a complete example showing an example of multiple producers and
 * consumers using the event queue in a runloop style on the Pico platform.
 */
#pragma once
#include "sync.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event type for queue items.
 * @ingroup SystemEvents
 *
 * Simple event type that can hold any pointer data. This can be enhanced
 * in the future to include metadata like timestamps, priorities, etc.
 */
typedef void *sys_event_t;

/**
 * @brief Event queue structure for producer/consumer patterns.
 * @ingroup SystemEvents
 *
 * Thread-safe queue that supports multiple producers and consumers.
 * Producers can always add items (with overflow handling), while consumers
 * can wait for items or peek before acquiring them.
 */
typedef struct {
  sys_event_t *items;   ///< Array of event items
  size_t capacity;      ///< Maximum queue size
  size_t head;          ///< Write position (producer index)
  size_t tail;          ///< Read position (consumer index)
  size_t count;         ///< Current number of items
  sys_mutex_t mutex;    ///< Protects all queue state
  sys_cond_t not_empty; ///< Signals consumers when items available
  bool shutdown;        ///< Flag for graceful shutdown
} sys_event_queue_t;

/**
 * @brief Initialize a new event queue
 * @ingroup SystemEvents
 * @param capacity Maximum number of events the queue can hold
 * @return Initialized event queue structure
 *
 * Creates a new thread-safe event queue with the specified capacity.
 * The queue uses circular buffer semantics - when full, new items will
 * overwrite the oldest items. The returned queue must be finalized
 * with sys_event_queue_finalize().
 */
sys_event_queue_t sys_event_queue_init(size_t capacity);

/**
 * @brief Finalize and cleanup an event queue
 * @ingroup SystemEvents
 * @param queue Pointer to the queue to finalize
 *
 * Finalizes the event queue and releases all associated resources.
 * Any threads waiting on the queue will be woken up. The queue
 * should not be used after this call.
 */
void sys_event_queue_finalize(sys_event_queue_t *queue);

/**
 * @brief Push an event to the queue (always succeeds)
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @param event Event to add to the queue
 * @return true if successful, false on error
 *
 * Adds an event to the queue. This operation always succeeds - if the
 * queue is full, the oldest event is overwritten. This ensures producers
 * are never blocked.
 */
bool sys_event_queue_push(sys_event_queue_t *queue, sys_event_t event);

/**
 * @brief Try to push an event without overwriting
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @param event Event to add to the queue
 * @return true if successful, false if queue is full or error
 *
 * Attempts to add an event to the queue. Returns false if the queue
 * is full, ensuring no existing events are overwritten.
 */
bool sys_event_queue_try_push(sys_event_queue_t *queue, sys_event_t event);

/**
 * @brief Peek at the next event without removing it
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @return Next event or NULL if queue is empty
 *
 * Returns the next event that would be returned by sys_event_queue_pop()
 * without removing it from the queue. This allows consumers to inspect
 * events before deciding whether to acquire them. The queue must be
 * locked by the caller.
 */
sys_event_t sys_event_queue_peek(sys_event_queue_t *queue);

/**
 * @brief Remove and return the next event (blocking)
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @return Next event, or NULL if queue is shut down
 *
 * Removes and returns the next event from the queue. If the queue is
 * empty, this function blocks until an event becomes available or
 * the queue is shut down.
 */
sys_event_t sys_event_queue_pop(sys_event_queue_t *queue);

/**
 * @brief Try to remove and return the next event (non-blocking)
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @return Next event, or NULL if queue is empty or error
 *
 * Attempts to remove and return the next event from the queue.
 * Returns NULL immediately if the queue is empty.
 */
sys_event_t sys_event_queue_try_pop(sys_event_queue_t *queue);

/**
 * @brief Remove and return the next event with timeout
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return Next event, or NULL if timeout or queue shut down
 *
 * Removes and returns the next event from the queue. If the queue is
 * empty, waits up to timeout_ms milliseconds for an event to become
 * available.
 */
sys_event_t sys_event_queue_timed_pop(sys_event_queue_t *queue,
                                      uint32_t timeout_ms);

/**
 * @brief Get the current number of events in the queue
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @return Current number of events, or 0 on error
 *
 * Returns the current number of events in the queue. This is a
 * snapshot value that may change immediately after the call returns
 * in a multi-threaded environment.
 */
size_t sys_event_queue_size(sys_event_queue_t *queue);

/**
 * @brief Check if the queue is empty
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @return true if empty, false otherwise
 *
 * Returns true if the queue contains no events. This is a snapshot
 * value that may change immediately after the call returns in a
 * multi-threaded environment.
 */
bool sys_event_queue_empty(sys_event_queue_t *queue);

/**
 * @brief Shut down the queue gracefully
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 *
 * Sets the shutdown flag and wakes up all waiting consumers.
 * After shutdown, push operations may fail and pop operations
 * will return NULL once the queue is empty.
 */
void sys_event_queue_shutdown(sys_event_queue_t *queue);

/**
 * @brief Lock the queue for manual synchronization
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @return true if successful, false on error
 *
 * Manually locks the queue mutex. This allows for atomic sequences
 * of peek/pop operations. Must be paired with sys_event_queue_unlock().
 */
bool sys_event_queue_lock(sys_event_queue_t *queue);

/**
 * @brief Unlock the queue after manual synchronization
 * @ingroup SystemEvents
 * @param queue Pointer to the event queue
 * @return true if successful, false on error
 *
 * Unlocks the queue mutex. Must be called after sys_event_queue_lock().
 */
bool sys_event_queue_unlock(sys_event_queue_t *queue);

#ifdef __cplusplus
}
#endif
