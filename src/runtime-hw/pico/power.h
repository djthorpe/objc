#pragma once
#include <stdint.h>

/**
 * @brief Poll the hardware power management system.
 */
void _hw_power_poll();

/**
 * @brief Notify power management system of reset.
 */
void _hw_power_notify_reset(uint32_t delay_ms);
