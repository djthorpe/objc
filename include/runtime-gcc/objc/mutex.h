/**
 * @file mutex.h
 * @brief Object synchronization and locking functions for the Objective-C
 * runtime.
 */
#pragma once
#include "runtime.h"

/**
 * @brief Acquire a lock on the specified object for thread synchronization.
 * @ingroup objc   
 * @param obj The object to lock. Must not be nil.
 * @return 0 on success, or a non-zero error code on failure.
 */
int objc_sync_enter(id obj);

/**
 * @brief Release a lock on the specified object.
 * @ingroup objc   
 * @param obj The object to unlock. Must be the same object passed to
 * objc_sync_enter().
 * @return 0 on success, or a non-zero error code on failure.
 */
int objc_sync_exit(id obj);
