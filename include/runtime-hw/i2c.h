/**
 * @file i2c.h
 * @brief I2C (Inter-Integrated Circuit) interface
 * @defgroup I2C I2C
 * @ingroup Hardware
 *
 * Inter-Integrated Circuit (I2C) interface for hardware platforms.
 * This module provides functions to initialize I2C peripherals.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

#define HW_I2C_CTX_SIZE 32 ///< Size of the I2C context structure

/**
 * @brief GPIO logical pin structure.
 * @ingroup GPIO
 */
typedef struct hw_i2c_t {
  uint8_t adapter;              ///< I2C adapter number (0, 1, etc.)
  uint8_t sda;                  ///< I2C data pin number
  uint8_t scl;                  ///< I2C clock pin number
  uint8_t ctx[HW_I2C_CTX_SIZE]; ///< Context for I2C operations
} hw_i2c_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Get the total number of available I2C adapters.
 * @ingroup I2C
 * @return The number of I2C adapters available on the hardware platform.
 *
 * Returns the number of logical I2C adapters that can be used in the system.
 * These are usually numbered from 0 to hw_i2c_count() - 1.
 * If zero is returned, it indicates that I2C functionality is not available.
 */
uint8_t hw_i2c_count(void);

/**
 * @brief Initialize an I2C interface using default pins and adapter.
 * @ingroup I2C
 * @return An I2C structure representing the initialized interface.
 *
 * This function initializes the default I2C interface using platform-specific
 * default pins and adapter settings. This is the simplest way to get I2C
 * functionality without needing to specify pin assignments.
 */
hw_i2c_t hw_i2c_init_default();

/**
 * @brief Initialize an I2C interface with specific adapter and pins.
 * @ingroup I2C
 * @param adapter The I2C adapter number to use (0 to hw_i2c_count()-1).
 * @param sda The GPIO pin number to use for I2C data (SDA).
 * @param scl The GPIO pin number to use for I2C clock (SCL).
 * @return An I2C structure representing the initialized interface.
 *
 * This function initializes an I2C interface using the specified adapter
 * and GPIO pins for SDA and SCL lines. The adapter number should be valid
 * and the specified pins should be capable of I2C functionality.
 *
 * @note The specified pins must support I2C functionality.
 * @note Pin assignments are platform-dependent.
 */
hw_i2c_t hw_i2c_init(uint8_t adapter, uint8_t sda, uint8_t scl);

/**
 * @brief Finalize and release an I2C interface.
 * @ingroup I2C
 * @param i2c Pointer to the I2C structure to finalize.
 *
 * This function releases the I2C interface and frees any associated resources.
 * After calling this function, the I2C interface should not be used for
 * further operations.
 */
void hw_i2c_finalize(hw_i2c_t *i2c);