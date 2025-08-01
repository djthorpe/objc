/**
 * @file sys.h
 * @brief System management functions.
 * @defgroup System System Runtime
 *
 * The "system runtime" layer provides low-level functionality specific to the
 * platform
 *
 * The system runtime includes memory, process and thread management,
 * synchronization primitives, hashes, string formatting, random numbers and
 * timers. When developing for a new platform, you will need to implement the
 * functions in these modules. The implementation will depend on the specific
 * platform's capabilities and requirements.
 *
 * When using this module, you should include this header file and link against
 * the appropriate implementation for your platform. In your entrypoint file,
 * you should call `sys_init()` to initialize the system, and `sys_exit()` to
 * clean up resources before exiting.
 *
 * There is a set of tests that can be run to verify the functionality of
 * the system runtime. These tests can be found in the `tests` directory.
 */
#pragma once
#include "assert.h"
#include "date.h"
#include "event.h"
#include "hash.h"
#include "hashmap.h"
#include "hashtable.h"
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
