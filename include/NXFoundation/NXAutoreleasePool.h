/**
 * @file NXAutoreleasePool.h
 * @brief Defines the NXAutoreleasePool class for managing autoreleased objects.
 *
 * This file provides the definition for NXAutoreleasePool, which is used
 * to manage objects that are to be released at a later time.
 */
#pragma once
#include "NXObject.h"

/**
 * @brief A class for managing autorelease pools.
 * @ingroup Foundation
 *
 * Autorelease pools provide a way to defer sending `release` messages to
 * objects. When an autorelease pool is drained, it sends a `release` message to
 * all objects that have been added to it. This is particularly useful in loops
 * where many temporary objects are created.
 *
 * \headerfile NXAutoreleasePool.h NXFoundation/NXFoundation.h
 */
@interface NXAutoreleasePool : NXObject {
@private
  /**
   * @var _prev
   * @brief A pointer to the previous autorelease pool in the stack.
   */
  id _prev;

  /**
   * @var _tail
   * @brief A pointer to the last NXObject added to the stack.
   */
  id _tail;
}

/**
 * @brief The current autorelease pool.
 *
 * This variable holds the current autorelease pool that is being used.
 * It is automatically set when an autorelease pool is created.
 */
+ (id)currentPool;

/**
 * @brief Adds an object to the autorelease pool.
 * @param object The object to be added to the pool. This object will be sent a
 *               `release` message when the pool is deallocated.
 */
- (void)addObject:(id)object;

/**
 * @brief Drain the autorelease pool.
 */
- (void)drain;

@end
