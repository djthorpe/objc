
/**
 * @file GPIOTypes.h
 * @brief GPIO input types for handling GPIO events.
 *
 * GPIO-related types.
 */
#pragma once
#include <runtime-hw/hw.h>

/**
 * @brief GPIO input types for handling GPIO events.
 * @ingroup Application
 */
typedef enum {
  GPIOEventRising = HW_GPIO_RISING,   ///< Rising edge detected
  GPIOEventFalling = HW_GPIO_FALLING, ///< Falling edge detected
  GPIOEventChanged =
      GPIOEventRising | GPIOEventFalling, ///< Both edges detected
} GPIOEvent;
