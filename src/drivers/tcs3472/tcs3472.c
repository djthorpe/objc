/**
 * @file tcs3472.c
 * @brief TCS3472 RGB Color Sensor Driver implementation
 * @ingroup TCS3472
 */
#include <drivers/drivers_tcs3472.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "tcs3472_registers.h"

///////////////////////////////////////////////////////////////////////////////
// CONSTANTS

// Default settings
#define TCS3472_DEFAULT_GAIN TCS3472_GAIN_1X
#define TCS3472_DEFAULT_INTEGRATION TCS3472_INTEGRATION_154MS

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * @brief Calculate integration time in milliseconds from ATIME value
 */
static uint32_t tcs3472_integration_time_ms(uint8_t atime) {
  // Integration time = (256 - ATIME) × 2.4ms
  return (256 - atime) * 24 / 10; // Calculate in tenths to avoid float
}

/**
 * @brief Write a single byte to a TCS3472 register
 */
static bool tcs3472_write_register(driver_tcs3472_t *driver, uint8_t reg,
                                   uint8_t value) {
  uint8_t buffer[2];
  buffer[0] = TCS3472_COMMAND_BIT | reg;
  buffer[1] = value;
  return hw_i2c_xfr(&driver->i2c, TCS3472_I2C_ADDR, buffer, 2, 0, 0) == 2;
}

/**
 * @brief Read a single byte from a TCS3472 register
 */
static bool tcs3472_read_register(driver_tcs3472_t *driver, uint8_t reg,
                                  uint8_t *value) {
  uint8_t buffer[2];
  buffer[0] = TCS3472_COMMAND_BIT | reg;
  if (hw_i2c_xfr(&driver->i2c, TCS3472_I2C_ADDR, buffer, 1, 1, 0) == 2) {
    *value = buffer[1];
    return true;
  }
  return false;
}

/**
 * @brief Read multiple bytes from TCS3472 registers
 */
static bool tcs3472_read_registers(driver_tcs3472_t *driver, uint8_t reg,
                                   uint8_t *buffer, uint8_t len) {
  uint8_t temp_buffer[1 + 8]; // cmd + max 8 data bytes
  temp_buffer[0] = TCS3472_COMMAND_BIT | reg;
  size_t expected = (size_t)(1 + len);
  if (hw_i2c_xfr(&driver->i2c, TCS3472_I2C_ADDR, temp_buffer, 1, len, 0) ==
      expected) {
    // Copy the received data (starts at offset 1 after command byte)
    for (uint8_t i = 0; i < len; i++) {
      buffer[i] = temp_buffer[1 + i];
    }
    return true;
  }
  return false;
}

/**
 * @brief Verify device ID
 */
static bool tcs3472_verify_id(driver_tcs3472_t *driver) {
  uint8_t id;
  if (!tcs3472_read_register(driver, TCS3472_ID, &id)) {
    return false;
  }

  if (id != TCS3472_ID_VALUE && id != TCS34727_ID_VALUE) {
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

bool driver_tcs3472_init(driver_tcs3472_t *driver, hw_i2c_t *i2c) {
  return driver_tcs3472_init_with_config(driver, i2c, TCS3472_DEFAULT_GAIN,
                                         TCS3472_DEFAULT_INTEGRATION);
}

bool driver_tcs3472_init_with_config(
    driver_tcs3472_t *driver, hw_i2c_t *i2c, driver_tcs3472_gain_t gain,
    driver_tcs3472_integration_time_t integration) {
  sys_assert(driver);
  sys_assert(i2c);

  // Zero out the structure
  memset(driver, 0, sizeof(driver_tcs3472_t));

  // Copy I2C handle
  memcpy(&driver->i2c, i2c, sizeof(hw_i2c_t));

  // Store integration time
  driver->integration = integration;

  // Verify device ID
  if (!tcs3472_verify_id(driver)) {
    return false;
  }

  // Set integration time (while device is powered off)
  if (!tcs3472_write_register(driver, TCS3472_ATIME, integration)) {
    return false;
  }

  // Set gain (while device is powered off)
  if (!tcs3472_write_register(driver, TCS3472_CONTROL, gain)) {
    return false;
  }

  // Power on (PON only, no AEN yet)
  if (!tcs3472_write_register(driver, TCS3472_ENABLE, TCS3472_ENABLE_PON)) {
    return false;
  }

  // Wait for oscillator to start (2.4ms according to datasheet)
  sys_sleep(3);

  // Enable RGBC ADC (PON | AEN)
  if (!tcs3472_write_register(driver, TCS3472_ENABLE,
                              TCS3472_ENABLE_PON | TCS3472_ENABLE_AEN)) {
    return false;
  }

  // Wait for RGBC Init (2.4ms) + first integration cycle to complete
  // Per datasheet state diagram: AEN triggers RGBC Init (2.4ms), then RGBC ADC
  // (integration time)
  uint32_t wait_ms = 3 + tcs3472_integration_time_ms(integration);
  sys_sleep(wait_ms);

  return true;
}

void driver_tcs3472_finalize(driver_tcs3472_t *driver) {
  sys_assert(driver);

  // Power off the sensor
  tcs3472_write_register(driver, TCS3472_ENABLE, 0x00);

  // Zero out the structure
  memset(driver, 0, sizeof(driver_tcs3472_t));
}

///////////////////////////////////////////////////////////////////////////////
// DATA ACQUISITION

bool driver_tcs3472_read(driver_tcs3472_t *driver,
                         driver_tcs3472_rgbc_t *data) {
  sys_assert(driver);
  sys_assert(data);

  // Calculate timeout based on integration time (add 50ms margin)
  uint32_t integration_ms = tcs3472_integration_time_ms(driver->integration);
  uint32_t timeout_ms = integration_ms + 50;
  uint32_t elapsed_ms = 0;

  // Wait for data to be valid (poll with timeout)
  uint8_t status;

  while (elapsed_ms < timeout_ms) {
    if (!tcs3472_read_register(driver, TCS3472_STATUS, &status)) {
      return false;
    }

    if (status & TCS3472_STATUS_AVALID) {
      break; // Data is valid
    }

    sys_sleep(10); // Wait 10ms before checking again
    elapsed_ms += 10;
  }

  if (!(status & TCS3472_STATUS_AVALID)) {
    return false;
  }

  // Read all 8 bytes (clear, red, green, blue - 2 bytes each)
  // Note: Reading CDATAL should clear the AVALID bit according to datasheet
  uint8_t buffer[8];
  if (!tcs3472_read_registers(driver, TCS3472_CDATAL, buffer, 8)) {
    return false;
  }

  // Parse the data (little-endian)
  data->clear = ((uint16_t)buffer[1] << 8) | buffer[0];
  data->red = ((uint16_t)buffer[3] << 8) | buffer[2];
  data->green = ((uint16_t)buffer[5] << 8) | buffer[4];
  data->blue = ((uint16_t)buffer[7] << 8) | buffer[6];

  return true;
}
