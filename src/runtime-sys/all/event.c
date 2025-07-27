#include <runtime-sys/event.h>
#include <runtime-sys/sys.h>

sys_event_queue_t sys_event_queue_init(size_t capacity) {
  sys_event_queue_t queue = {0};

  // Validate capacity
  if (capacity == 0) {
    return queue;
  }

  // Allocate items array
  queue.items = (sys_event_t *)sys_malloc(capacity * sizeof(sys_event_t));
  if (queue.items == NULL) {
    return queue;
  }

  // Initialize queue state
  queue.capacity = capacity;
  queue.head = 0;
  queue.tail = 0;
  queue.count = 0;
  queue.shutdown = false;

  // Initialize synchronization primitives
  queue.mutex = sys_mutex_init();
  queue.not_empty = sys_cond_init();

  // Check if initialization succeeded
  if (!queue.mutex.init || !queue.not_empty.init) {
    // Cleanup on failure
    if (queue.mutex.init) {
      sys_mutex_finalize(&queue.mutex);
    }
    if (queue.not_empty.init) {
      sys_cond_finalize(&queue.not_empty);
    }
    sys_free(queue.items);
    sys_memset(&queue, 0, sizeof(queue));
  }

  return queue;
}

void sys_event_queue_finalize(sys_event_queue_t *queue) {
  if (queue == NULL || queue->items == NULL) {
    return;
  }

  // Wake up any waiting consumers
  sys_event_queue_shutdown(queue);

  // Cleanup synchronization primitives
  if (queue->mutex.init) {
    sys_mutex_finalize(&queue->mutex);
  }
  if (queue->not_empty.init) {
    sys_cond_finalize(&queue->not_empty);
  }

  // Free the items array
  sys_free(queue->items);

  // Clear the structure
  sys_memset(queue, 0, sizeof(*queue));
}

bool sys_event_queue_push(sys_event_queue_t *queue, sys_event_t event) {
  if (queue == NULL || queue->items == NULL || !queue->mutex.init) {
    return false;
  }

  if (!sys_mutex_lock(&queue->mutex)) {
    return false;
  }

  // Don't allow pushes after shutdown
  if (queue->shutdown) {
    sys_mutex_unlock(&queue->mutex);
    return false;
  }

  // Add event to the queue
  queue->items[queue->head] = event;
  queue->head = (queue->head + 1) % queue->capacity;

  // If queue was full, we overwrote the oldest item
  if (queue->count == queue->capacity) {
    // Move tail forward (oldest item was overwritten)
    queue->tail = (queue->tail + 1) % queue->capacity;
  } else {
    // Queue wasn't full, increment count
    queue->count++;
  }

  // Signal waiting consumers (broadcast for fairness in multi-consumer
  // scenarios)
  bool result = sys_cond_broadcast(&queue->not_empty);
  sys_mutex_unlock(&queue->mutex);
  return result;
}

bool sys_event_queue_try_push(sys_event_queue_t *queue, sys_event_t event) {
  if (queue == NULL || queue->items == NULL || !queue->mutex.init) {
    return false;
  }
  if (!sys_mutex_trylock(&queue->mutex)) {
    return false;
  }

  // Don't allow pushes after shutdown or if queue is full
  if (queue->shutdown || queue->count == queue->capacity) {
    sys_mutex_unlock(&queue->mutex);
    return false;
  }

  // Add event to the queue
  queue->items[queue->head] = event;
  queue->head = (queue->head + 1) % queue->capacity;
  queue->count++;

  // Signal waiting consumers
  bool result = sys_cond_broadcast(&queue->not_empty);
  sys_mutex_unlock(&queue->mutex);
  return result;
}

sys_event_t sys_event_queue_peek(sys_event_queue_t *queue) {
  if (queue == NULL || queue->items == NULL || queue->count == 0) {
    return NULL;
  }

  // Return the event at tail position without removing it
  return queue->items[queue->tail];
}

sys_event_t sys_event_queue_pop(sys_event_queue_t *queue) {
  if (queue == NULL || queue->items == NULL || !queue->mutex.init) {
    return NULL;
  }
  if (!sys_mutex_lock(&queue->mutex)) {
    return NULL;
  }

  // Wait for events or shutdown
  while (queue->count == 0 && !queue->shutdown) {
    if (!sys_cond_wait(&queue->not_empty, &queue->mutex)) {
      sys_mutex_unlock(&queue->mutex);
      return NULL;
    }
  }

  // Check if we woke up due to shutdown
  if (queue->shutdown && queue->count == 0) {
    sys_mutex_unlock(&queue->mutex);
    return NULL;
  }

  // Remove and return the event
  sys_event_t event = queue->items[queue->tail];
  queue->tail = (queue->tail + 1) % queue->capacity;
  queue->count--;

  sys_mutex_unlock(&queue->mutex);
  return event;
}

sys_event_t sys_event_queue_try_pop(sys_event_queue_t *queue) {
  if (queue == NULL || queue->items == NULL || !queue->mutex.init) {
    return NULL;
  }
  if (!sys_mutex_lock(&queue->mutex)) {
    return NULL;
  }

  // Return NULL immediately if empty (or if empty and shut down)
  if (queue->count == 0) {
    sys_mutex_unlock(&queue->mutex);
    return NULL;
  }

  // Remove and return the event
  sys_event_t event = queue->items[queue->tail];
  queue->tail = (queue->tail + 1) % queue->capacity;
  queue->count--;

  sys_mutex_unlock(&queue->mutex);
  return event;
}

sys_event_t sys_event_queue_timed_pop(sys_event_queue_t *queue,
                                      uint32_t timeout_ms) {
  if (queue == NULL || queue->items == NULL || !queue->mutex.init) {
    return NULL;
  }
  if (!sys_mutex_lock(&queue->mutex)) {
    return NULL;
  }

  // If timeout is 0, behave like regular pop
  if (timeout_ms == 0) {
    // Wait for events or shutdown
    while (queue->count == 0 && !queue->shutdown) {
      if (!sys_cond_wait(&queue->not_empty, &queue->mutex)) {
        sys_mutex_unlock(&queue->mutex);
        return NULL;
      }
    }
  } else {
    // Wait with timeout
    if (queue->count == 0 && !queue->shutdown) {
      if (!sys_cond_timedwait(&queue->not_empty, &queue->mutex, timeout_ms)) {
        // Timeout or error occurred
        sys_mutex_unlock(&queue->mutex);
        return NULL;
      }
    }
  }

  // Check if we woke up due to shutdown or timeout
  if (queue->shutdown && queue->count == 0) {
    sys_mutex_unlock(&queue->mutex);
    return NULL;
  }

  // If still no events (timeout case), return NULL
  if (queue->count == 0) {
    sys_mutex_unlock(&queue->mutex);
    return NULL;
  }

  // Remove and return the event
  sys_event_t event = queue->items[queue->tail];
  queue->tail = (queue->tail + 1) % queue->capacity;
  queue->count--;
  sys_mutex_unlock(&queue->mutex);
  return event;
}

size_t sys_event_queue_size(sys_event_queue_t *queue) {
  if (queue == NULL || queue->items == NULL || !queue->mutex.init) {
    return 0;
  }
  if (!sys_mutex_lock(&queue->mutex)) {
    return 0;
  }

  size_t size = queue->count;
  sys_mutex_unlock(&queue->mutex);
  return size;
}

bool sys_event_queue_empty(sys_event_queue_t *queue) {
  return sys_event_queue_size(queue) == 0;
}

void sys_event_queue_shutdown(sys_event_queue_t *queue) {
  if (queue == NULL || queue->items == NULL || !queue->mutex.init) {
    return;
  }
  if (!sys_mutex_lock(&queue->mutex)) {
    return;
  }

  // Indicate shutdown
  queue->shutdown = true;

  // Wake up all waiting consumers, unlock and return
  sys_cond_broadcast(&queue->not_empty);
  sys_mutex_unlock(&queue->mutex);
}

bool sys_event_queue_lock(sys_event_queue_t *queue) {
  if (queue == NULL || !queue->mutex.init) {
    return false;
  }

  return sys_mutex_lock(&queue->mutex);
}

bool sys_event_queue_unlock(sys_event_queue_t *queue) {
  if (queue == NULL || !queue->mutex.init) {
    return false;
  }

  return sys_mutex_unlock(&queue->mutex);
}
