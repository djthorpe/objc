/**
 * @file sys.h
 * @brief System management functions.
 * @defgroup System System Management
 *
 *
 * This module (and submodules) declare types and functions for system
 * management, including memory management, thread management, synchronization
 * primitives, and timer management.
 *
 * When developing for a new platform, you will need to implement the
 * functions declared here. The implementation will depend on the specific
 * platform's capabilities and requirements.
 *
 * When using this module, you should include this header file and link against
 * the appropriate implementation for your platform. In your entrypoint file,
 * you should call `sys_init()` to initialize the system, and `sys_exit()` to
 * clean up resources before exiting.
 */
#pragma once
#include "date.h"
#include "event.h"
#include "hash.h"
#include "memory.h"
#include "printf.h"
#include "random.h"
#include "sync.h"
#include "thread.h"
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the system on startup.
 * @ingroup System
 *
 * This function must be called at the start of the program to initialize
 * the system environment, including standard input/output streams and any
 * necessary subsystems. It prepares the system for normal operation.
 * It may include platform-specific initialization steps.
 */
extern void sys_init(void);

/**
 * @brief Cleans up the system on shutdown.
 * @ingroup System
 *
 * This function should be called at the end of the program to perform any
 * necessary cleanup tasks, such as releasing resources and shutting down
 * subsystems. It prepares the system for termination.
 */
extern void sys_exit(void);

#ifdef __cplusplus
}
#endif
