#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a SPI interface using default pins and adapter.
 */
hw_spi_t hw_spi_init_default(bool cs_active_low, uint32_t baudrate) {
  // SPI not implemented in stub
  (void)cs_active_low; // Suppress unused parameter warning
  (void)baudrate;      // Suppress unused parameter warning
  hw_spi_t spi = {0};
  return spi;
}

/**
 * @brief Initialize a SPI interface with specific adapter and pins.
 */
hw_spi_t hw_spi_init(uint8_t adapter, uint8_t sck, uint8_t tx, uint8_t rx,
                     uint8_t cs, bool cs_active_low, uint32_t baudrate) {
  // SPI not implemented in stub
  (void)adapter;       // Suppress unused parameter warning
  (void)sck;           // Suppress unused parameter warning
  (void)tx;            // Suppress unused parameter warning
  (void)rx;            // Suppress unused parameter warning
  (void)cs;            // Suppress unused parameter warning
  (void)cs_active_low; // Suppress unused parameter warning
  (void)baudrate;      // Suppress unused parameter warning
  hw_spi_t spi = {0};
  return spi;
}

/**
 * @brief Finalize and release a SPI interface.
 */
void hw_spi_finalize(hw_spi_t *spi) {
  sys_assert(spi);
  // SPI not implemented in stub
  (void)spi; // Suppress unused parameter warning
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Get the total number of available SPI adapters.
 */
uint8_t hw_spi_count(void) {
  // SPI not implemented in stub
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Perform a SPI transfer operation (read, write, or combined).
 */
size_t hw_spi_xfr(hw_spi_t *spi, void *data, size_t tx, size_t rx) {
  sys_assert(spi);
  // SPI not implemented in stub
  (void)data; // Suppress unused parameter warning
  (void)tx;   // Suppress unused parameter warning
  (void)rx;   // Suppress unused parameter warning
  return 0;   // Always return 0 since transfer is not implemented
}

/**
 * @brief Read data from a specific register of a SPI device.
 */
size_t hw_spi_read(hw_spi_t *spi, uint8_t reg, void *data, size_t len) {
  sys_assert(spi);
  sys_assert(data || len == 0);
  // SPI not implemented in stub
  (void)reg;  // Suppress unused parameter warning
  (void)data; // Suppress unused parameter warning
  (void)len;  // Suppress unused parameter warning
  return 0;   // Always return 0 since read is not implemented
}

/**
 * @brief Write data to a specific register of a SPI device.
 */
size_t hw_spi_write(hw_spi_t *spi, uint8_t reg, const void *data, size_t len) {
  sys_assert(spi);
  // SPI not implemented in stub
  (void)reg;  // Suppress unused parameter warning
  (void)data; // Suppress unused parameter warning
  (void)len;  // Suppress unused parameter warning
  return 0;   // Always return 0 since write is not implemented
}
