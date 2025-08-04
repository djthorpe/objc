/**
 * @file i2c.h
 * @brief I2C (Inter-Integrated Circuit) interface
 * @defgroup I2C I2C
 * @ingroup Hardware
 *
 * Inter-Integrated Circuit (I2C) interface.
 * This module provides functions to initialize I2C peripherals,
 * and bi-directional data transfer with I2C devicesin master mode.
 */
#pragma once
#include "gpio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief I2C adapter.
 * @ingroup I2C
 * @headerfile i2c.h runtime-hw/hw.h
 */
typedef struct hw_i2c_t {
  uint8_t adapter;     ///< I2C adapter number (0, 1, etc.)
  hw_gpio_t sda;       ///< I2C data pin number
  hw_gpio_t scl;       ///< I2C clock pin number
  uint32_t baudrate;   ///< I2C baud rate in Hz
  uint8_t reserved[3]; ///< Reserved for user data
} hw_i2c_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize an I2C interface using default pins and adapter.
 * @ingroup I2C
 * @param baudrate The desired I2C baud rate (e.g., 100000 for 100kHz).
 * @return An I2C structure representing the initialized interface.
 *
 * This function initializes the default I2C interface using platform-specific
 * default pins and adapter settings. This is the simplest way to get I2C
 * functionality without needing to specify pin assignments.
 */
hw_i2c_t hw_i2c_init_default(uint32_t baudrate);

/**
 * @brief Initialize an I2C interface with specific adapter and pins.
 * @ingroup I2C
 * @param adapter The I2C adapter number to use (0 to hw_i2c_count()-1).
 * @param sda The GPIO pin number to use for I2C data (SDA).
 * @param scl The GPIO pin number to use for I2C clock (SCL).
 * @param baudrate The desired I2C baud rate (e.g., 100000 for 100kHz).
 * @return An I2C structure representing the initialized interface.
 *
 * This function initializes an I2C interface using the specified adapter
 * and GPIO pins for SDA and SCL lines. The adapter number should be valid
 * and the specified pins should be capable of I2C functionality.
 *
 * @note The specified pins must support I2C functionality.
 * @note Pin assignments are platform-dependent.
 */
hw_i2c_t hw_i2c_init(uint8_t adapter, uint8_t sda, uint8_t scl,
                     uint32_t baudrate);

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

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

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
 * @brief Get true if the I2C interface is valid.
 * @ingroup I2C
 * @return True if the I2C interface is valid, false otherwise.
 *
 * The result of hw_i2c_init can return an empty I2C structure if the
 * initialization fails. This function checks if the I2C interface is valid.
 */
static inline bool hw_i2c_valid(hw_i2c_t *i2c) {
  return i2c && i2c->baudrate > 0;
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Detect if an I2C device is present at the specified address.
 * @ingroup I2C
 * @param i2c Pointer to the I2C structure representing the interface.
 * @param addr The 7-bit I2C slave device address (without R/W bit).
 * @return true if the device responds, false if it does not, or the address is
 * invalid.
 *
 * This function checks if an I2C device is present at the specified address
 * by attempting to communicate with it. It returns true if the device responds
 * to a basic read/write operation, false otherwise.
 */
bool hw_i2c_detect(hw_i2c_t *i2c, uint8_t addr);

/**
 * @brief Perform an I2C transfer operation (read, write, or combined).
 * @ingroup I2C
 * @param i2c Pointer to the I2C structure representing the interface.
 * @param addr The 7-bit I2C slave device address (without R/W bit).
 * @param data Pointer to the data buffer for transmission and/or reception.
 * @param tx Number of bytes to transmit from the data buffer.
 * @param rx Number of bytes to receive into the data buffer (after tx bytes).
 * @param timeout_ms Timeout value in milliseconds for each operation. Set to 0
 * for no timeout.
 * @return Number of bytes transferred. Returns zero if the transfer failed or
 * timed out.
 *
 * This function performs I2C transfer operations which can be:
 * - Write-only: tx > 0, rx = 0 (transmit data to slave)
 * - Read-only: tx = 0, rx > 0 (receive data from slave)
 * - Write-then-Read: tx > 0, rx > 0 (write command, then read response)
 *
 * For combined operations, the function first transmits 'tx' bytes from the
 * data buffer, then receives 'rx' bytes into the same buffer starting at
 * offset 'tx'. The total buffer size should be at least (tx + rx) bytes.
 */
size_t hw_i2c_xfr(hw_i2c_t *i2c, uint8_t addr, void *data, size_t tx, size_t rx,
                  uint32_t timeout_ms);

/**
 * @brief Read data from a specific register of an I2C device.
 * @ingroup I2C
 * @param i2c Pointer to the I2C structure representing the interface.
 * @param addr The 7-bit I2C slave device address (without R/W bit).
 * @param reg The register address to read from.
 * @param data Pointer to the buffer where read data will be stored.
 * @param len Number of bytes to read from the register.
 * @param timeout_ms Timeout value in milliseconds for the operation. Set to 0
 * for no timeout.
 * @return Number of bytes successfully read. Returns zero if the operation
 * failed or timed out.
 */
size_t hw_i2c_read(hw_i2c_t *i2c, uint8_t addr, uint8_t reg, void *data,
                   size_t len, uint32_t timeout_ms);

/**
 * @brief Write data to a specific register of an I2C device.
 * @ingroup I2C
 * @param i2c Pointer to the I2C structure representing the interface.
 * @param addr The 7-bit I2C slave device address (without R/W bit).
 * @param reg The register address to write to.
 * @param data Pointer to the buffer containing data to write.
 * @param len Number of bytes to write to the register.
 * @param timeout_ms Timeout value in milliseconds for the operation. Set to 0
 * for no timeout.
 * @return Number of bytes successfully written. Returns zero if the operation
 * failed or timed out.
 */
size_t hw_i2c_write(hw_i2c_t *i2c, uint8_t addr, uint8_t reg, const void *data,
                    size_t len, uint32_t timeout_ms);
