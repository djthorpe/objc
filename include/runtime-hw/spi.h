/**
 * @file spi.h
 * @brief SPI (Serial Peripheral Interface) interface
 * @defgroup SPI SPI
 * @ingroup Hardware
 *
 * Serial Peripheral Interface (SPI) interface.
 * This module provides functions to initialize SPI peripherals,
 * and bi-directional data transfer with SPI devices in master mode.
 */
#pragma once
#include "gpio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief SPI adapter.
 * @ingroup SPI
 */
typedef struct hw_spi_t {
  uint8_t adapter;     ///< SPI adapter (0, 1, etc.)
  hw_gpio_t sck;       ///< SPI Clock pin
  hw_gpio_t tx;        ///< SPI Transmit pin (Master Out Slave In)
  hw_gpio_t rx;        ///< SPI Receive pin (Master In Slave Out)
  hw_gpio_t cs;        ///< SPI Chip Select pin
  bool cs_active;      ///< CS active state (false if active low)
  uint32_t baudrate;   ///< SPI baud rate in Hz
  uint8_t reserved[3]; ///< Reserved for user data
} hw_spi_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a SPI interface using default pins and adapter.
 * @ingroup SPI
 * @param cs_active_low True if the Chip Select (CS) pin is active low, false
 * otherwise.
 * @param baudrate The desired SPI baud rate (e.g., 100000 for 100kHz).
 * @return A SPI structure representing the initialized interface.
 *
 * This function initializes the default SPI interface using platform-specific
 * default pins and adapter settings. This is the simplest way to get SPI
 * functionality without needing to specify pin assignments.
 */
hw_spi_t hw_spi_init_default(bool cs_active_low, uint32_t baudrate);

/**
 * @brief Initialize a SPI interface with specific adapter and pins.
 * @ingroup SPI
 * @param adapter The SPI adapter number to use (0 to hw_spi_count()-1).
 * @param sck The GPIO pin number to use for SPI Clock (SCK).
 * @param tx The GPIO pin number to use for SPI Master Out Slave In (MOSI).
 * @param rx The GPIO pin number to use for SPI Master In Slave Out (MISO).
 * @param cs The GPIO pin number to use for SPI Chip Select (CS).
 * @param cs_active_low True if the Chip Select (CS) pin is active low, false
 * otherwise.
 * @param baudrate The desired SPI baud rate (e.g., 100000 for 100kHz).
 * @return A SPI structure representing the initialized interface.
 *
 * This function initializes a SPI interface using the specified adapter
 * and GPIO pins for MOSI, MISO, and SCK lines. The adapter number should be
 * valid and the specified pins should be capable of SPI functionality.
 *
 * If the CS value is set to 0, it will not be used for reading or writing.
 *
 * @note The specified pins must support SPI functionality.
 * @note Pin assignments are platform-dependent.
 */
hw_spi_t hw_spi_init(uint8_t adapter, uint8_t sck, uint8_t tx, uint8_t rx,
                     uint8_t cs, bool cs_active_low, uint32_t baudrate);

/**
 * @brief Finalize and release a SPI interface.
 * @ingroup SPI
 * @param spi Pointer to the SPI structure to finalize.
 *
 * This function releases the SPI interface and frees any associated resources.
 * After calling this function, the SPI interface should not be used for
 * further operations.
 */
void hw_spi_finalize(hw_spi_t *spi);

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Get the total number of available SPI adapters.
 * @ingroup SPI
 * @return The number of SPI adapters available on the hardware platform.
 *
 * Returns the number of logical SPI adapters that can be used in the system.
 * These are usually numbered from 0 to hw_spi_count() - 1.
 * If zero is returned, it indicates that SPI functionality is not available.
 */
uint8_t hw_spi_count(void);

/**
 * @brief Get true if the SPI interface is valid.
 * @ingroup SPI
 * @return True if the SPI interface is valid, false otherwise.
 *
 * The result of hw_spi_init can return an empty SPI structure if the
 * initialization fails. This function checks if the SPI interface is valid.
 */
static inline bool hw_spi_valid(hw_spi_t *spi) {
  return spi && spi->baudrate > 0;
}

///////////////////////////////////////////////////////////////////////////////
// DATA TRANSFER

/**
 * @brief Perform a SPI transfer operation (read, write, or combined).
 * @ingroup SPI
 * @param spi Pointer to the SPI structure.
 * @param data Pointer to the data buffer for read/write operations.
 * @param tx Number of bytes to transmit from the data buffer.
 * @param rx Number of bytes to receive into the data buffer (after tx bytes).
 * @return Number of bytes successfully transferred, or 0 on failure.
 *
 * This function performs a generic SPI transfer with automatic CS control.
 * For combined operations, transmit data is sent first, then receive data
 * is read into the buffer starting at offset tx.
 *
 * @note The CS pin is automatically asserted before and deasserted after the
 * transfer.
 * @note For combined transfers, ensure the data buffer is large enough for tx +
 * rx bytes.
 */
size_t hw_spi_xfr(hw_spi_t *spi, void *data, size_t tx, size_t rx);

/**
 * @brief Read data from a specific register of a SPI device.
 * @ingroup SPI
 * @param spi Pointer to the SPI structure.
 * @param reg Register address to read from.
 * @param data Pointer to the buffer where read data will be stored.
 * @param len Number of bytes to read.
 * @return Number of bytes successfully read, or 0 on failure.
 *
 * This function writes the register address and then reads the specified
 * number of bytes with automatic CS control.
 */
size_t hw_spi_read(hw_spi_t *spi, uint8_t reg, void *data, size_t len);

/**
 * @brief Write data to a specific register of a SPI device.
 * @ingroup SPI
 * @param spi Pointer to the SPI structure.
 * @param reg Register address to write to.
 * @param data Pointer to the data to write (can be NULL if len is 0).
 * @param len Number of bytes to write.
 * @return Number of bytes successfully written, or 0 on failure.
 *
 * This function writes the register address followed by the data
 * with automatic CS control.
 */
size_t hw_spi_write(hw_spi_t *spi, uint8_t reg, const void *data, size_t len);
