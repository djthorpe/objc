/**
 * @file waitgroup.c
 * @brief Pico-specific waitgroup implementation using multicore primitives
 * @ingroup System
 */

#include "hardware/platform_defs.h"
#include "pico/multicore.h"
#include "pico/mutex.h"
#include "pico/sem.h"
#include <pico/stdlib.h>
#include <runtime-sys/sys.h>
#include <stdio.h>

/**
 * @brief Internal waitgroup data structure using semaphore permits as counter
 */
typedef struct {
  semaphore_t sem; ///< Semaphore where permits represent pending work
  int total_added; ///< Total number of permits added (for wait operation)
  int done_count;  ///< Number of done() calls made so far
} waitgroup_data_t;

// Ensure the embedded buffer is large enough for our waitgroup structure
_Static_assert(sizeof(waitgroup_data_t) <= SYS_WAITGROUP_CTX_SIZE,
               "SYS_WAITGROUP_CTX_SIZE too small for waitgroup_data_t");

/**
 * @brief Initialize a new waitgroup
 */
sys_waitgroup_t sys_waitgroup_init(void) {
  sys_waitgroup_t wg;

  // Initialize to uninitialized state
  wg.init = false;

  // Get pointer to the embedded waitgroup data
  waitgroup_data_t *wgd = (waitgroup_data_t *)wg.ctx;

  // Initialize semaphore with 0 permits, max permits = large number for
  // flexibility
  sem_init(&wgd->sem, 0,
           32767); // 0 permits initially, max allows for many workers
  wgd->total_added = 0;
  wgd->done_count = 0;

  // Mark as initialized
  wg.init = true;

  return wg;
}

/**
 * @brief Add to the waitgroup counter
 */
bool sys_waitgroup_add(sys_waitgroup_t *wg, int delta) {
  if (!wg || !wg->init) {
    return false;
  }

  if (delta <= 0) {
    return false; // Only positive deltas allowed for add
  }

  waitgroup_data_t *wgd = (waitgroup_data_t *)wg->ctx;

  // Track how many work units we're adding, but don't add permits yet
  // The semaphore will get permits when work is done, not when work is added
  wgd->total_added += delta;

  return true;
}

/**
 * @brief Signal that one waiter is done (decrement by 1)
 */
bool sys_waitgroup_done(sys_waitgroup_t *wg) {
  if (!wg || !wg->init) {
    return false;
  }

  waitgroup_data_t *wgd = (waitgroup_data_t *)wg->ctx;

  // Check if we can call done() - must not exceed total_added
  if (wgd->done_count >= wgd->total_added) {
    return false;
  }

  // Increment done count and release a permit when work is done
  wgd->done_count++;
  sem_release(&wgd->sem);
  return true;
}

/**
 * @brief Finalize and cleanup a waitgroup - wait for completion then cleanup
 */
void sys_waitgroup_finalize(sys_waitgroup_t *wg) {
  if (!wg || !wg->init) {
    return;
  }

  waitgroup_data_t *wgd = (waitgroup_data_t *)wg->ctx;

  // Wait for all work to complete by acquiring permits equal to total_added
  // Each done() call releases one permit, so we wait for all of them
  for (int i = 0; i < wgd->total_added; i++) {
    sem_acquire_blocking(&wgd->sem); // Block until a done() releases a permit
  }

  // No explicit cleanup needed for Pico semaphore
  // Mark as uninitialized
  wg->init = false;
}
