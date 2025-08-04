#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Get the total number of available I2C adapters.
 */
uint8_t hw_i2c_count(void) {
  // I2C not implemented in stub
  return 0;
}

/**
 * @brief Initialize an I2C interface using default pins and adapter.
 */
hw_i2c_t hw_i2c_init_default(uint32_t baudrate) {
  // I2C not implemented in stub
  (void)baudrate; // Suppress unused parameter warning
  hw_i2c_t i2c = {0};
  return i2c;
}

/**
 * @brief Initialize an I2C interface with specific adapter and pins.
 */
hw_i2c_t hw_i2c_init(uint8_t adapter, uint8_t sda, uint8_t scl,
                     uint32_t baudrate) {
  // I2C not implemented in stub
  (void)adapter;  // Suppress unused parameter warning
  (void)sda;      // Suppress unused parameter warning
  (void)scl;      // Suppress unused parameter warning
  (void)baudrate; // Suppress unused parameter warning
  hw_i2c_t i2c = {0};
  return i2c;
}

/**
 * @brief Finalize and release an I2C interface.
 */
void hw_i2c_finalize(hw_i2c_t *i2c) {
  sys_assert(i2c);
  // I2C not implemented in stub
  (void)i2c; // Suppress unused parameter warning
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Detect if an I2C device is present at the specified address.
 */
bool hw_i2c_detect(hw_i2c_t *i2c, uint8_t addr) {
  sys_assert(i2c);
  // I2C not implemented in stub
  (void)addr;   // Suppress unused parameter warning
  return false; // Always return false since detection is not implemented
}

/**
 * @brief Perform an I2C transfer operation (read, write, or combined).
 */
size_t hw_i2c_xfr(hw_i2c_t *i2c, uint8_t addr, void *data, size_t tx, size_t rx,
                  uint32_t timeout_ms) {
  sys_assert(i2c);
  // I2C not implemented in stub
  (void)addr;       // Suppress unused parameter warning
  (void)data;       // Suppress unused parameter warning
  (void)tx;         // Suppress unused parameter warning
  (void)rx;         // Suppress unused parameter warning
  (void)timeout_ms; // Suppress unused parameter warning
  return 0;         // Always return 0 since transfer is not implemented
}

/**
 * @brief Read data from a specific register of an I2C device.
 */
size_t hw_i2c_read(hw_i2c_t *i2c, uint8_t addr, uint8_t reg, void *data,
                   size_t len, uint32_t timeout_ms) {
  sys_assert(i2c);
  // I2C not implemented in stub
  (void)addr;       // Suppress unused parameter warning
  (void)reg;        // Suppress unused parameter warning
  (void)data;       // Suppress unused parameter warning
  (void)len;        // Suppress unused parameter warning
  (void)timeout_ms; // Suppress unused parameter warning
  return 0;         // Always return 0 since read is not implemented
}

/**
 * @brief Write data to a specific register of an I2C device.
 */
size_t hw_i2c_write(hw_i2c_t *i2c, uint8_t addr, uint8_t reg, const void *data,
                    size_t len, uint32_t timeout_ms) {
  sys_assert(i2c);
  // I2C not implemented in stub
  (void)addr;       // Suppress unused parameter warning
  (void)reg;        // Suppress unused parameter warning
  (void)data;       // Suppress unused parameter warning
  (void)len;        // Suppress unused parameter warning
  (void)timeout_ms; // Suppress unused parameter warning
  return 0;         // Always return 0 since write is not implemented
}
