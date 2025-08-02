/**
 * @file hw.h
 * @brief Hardware interface abstraction layer
 * @defgroup Hardware Hardware Interfaces
 * @ingroup System
 *
 * Managing hardware resources, peripherals, and low-level operations.
 */
#pragma once
#include "gpio.h"

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes the hardware system on startup.
 * @ingroup Hardware
 *
 * This function must be called at the start of the program to initialize
 * the hardware environment.
 */
extern void hw_init(void);

/**
 * @brief Cleans up the hardware system on shutdown.
 * @ingroup Hardware
 *
 * This function should be called at the end of the program to perform any
 * necessary cleanup tasks.
 */
extern void hw_exit(void);
