/**
 * @file hw.h
 * @brief Hardware interface abstraction layer
 * @defgroup Hardware Hardware Interfaces
 * @ingroup System
 *
 * Managing hardware resources, peripherals, and low-level operations.
 */
#pragma once
#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "infrared.h"
#include "led.h"
#include "power.h"
#include "pwm.h"
#include "spi.h"
#include "watchdog.h"
#include "wifi.h"

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

/**
 * @brief Occasional polling function for the hardware system.
 * @ingroup Hardware
 *
 * This function should be called periodically to allow the hardware
 * system to perform any necessary background tasks.
 */
extern void hw_poll(void);
