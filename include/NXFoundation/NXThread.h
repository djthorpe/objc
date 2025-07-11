/**
 * @file NXThread.h
 * @brief Defines the NXThread class for thread management.
 *
 * This file provides the definition for the NXThread class, which offers
 * basic threading capabilities such as pausing thread execution.
 */

#pragma once
#include "NXTimeInterval.h"

/**
 * @brief A class for managing and interacting with threads.
 *
 * The NXThread class provides a high-level interface for managing
 * threads of execution in an application.
 *
 * \headerfile NXThread.h NXFoundation/NXFoundation.h
 */
@interface NXThread : NXObject

/**
 * @brief Blocks the current thread for a given time interval.
 * @param interval The number of seconds to block the thread.
 */
+ (void)sleepForTimeInterval:(NXTimeInterval)interval;

@end
