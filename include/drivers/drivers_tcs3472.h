/**
 * @file drivers_tcs3472.h
 * @brief TCS3472 RGB Color Sensor Driver
 * @defgroup TCS3472 TCS3472 Color Sensor
 * @ingroup Drivers
 *
 * Driver for the TCS3472 RGB color light-to-digital converter with IR blocking
 * filter.
 */
#pragma once
#include <runtime-hw/hw.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief TCS3472 gain settings
 * @ingroup TCS3472
 */
typedef enum {
  TCS3472_GAIN_1X = 0x00,  ///< 1x gain
  TCS3472_GAIN_4X = 0x01,  ///< 4x gain
  TCS3472_GAIN_16X = 0x02, ///< 16x gain
  TCS3472_GAIN_60X = 0x03  ///< 60x gain
} driver_tcs3472_gain_t;

/**
 * @brief TCS3472 integration time presets
 * @ingroup TCS3472
 */
typedef enum {
  TCS3472_INTEGRATION_2_4MS = 0xFF, ///< 2.4ms - fastest
  TCS3472_INTEGRATION_24MS = 0xF6,  ///< 24ms
  TCS3472_INTEGRATION_50MS = 0xEB,  ///< 50ms
  TCS3472_INTEGRATION_101MS = 0xD5, ///< 101ms
  TCS3472_INTEGRATION_154MS = 0xC0, ///< 154ms
  TCS3472_INTEGRATION_700MS = 0x00  ///< 700ms - max sensitivity
} driver_tcs3472_integration_time_t;

/**
 * @brief TCS3472 RGBC color data
 * @ingroup TCS3472
 */
typedef struct {
  uint16_t red;   ///< Red channel value (0-65535)
  uint16_t green; ///< Green channel value (0-65535)
  uint16_t blue;  ///< Blue channel value (0-65535)
  uint16_t clear; ///< Clear (unfiltered) channel value (0-65535)
} driver_tcs3472_rgbc_t;

/**
 * @brief TCS3472 sensor driver structure
 * @ingroup TCS3472
 */
typedef struct {
  hw_i2c_t i2c;                                  ///< I2C bus handle
  driver_tcs3472_integration_time_t integration; ///< Integration time setting
} driver_tcs3472_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize the TCS3472 sensor with default settings
 * @ingroup TCS3472
 * @param driver Pointer to the TCS3472 driver structure
 * @param i2c Pointer to an initialized I2C bus handle
 * @return true if initialization was successful, false otherwise
 *
 * Initializes the sensor with:
 * - Power on
 * - RGBC enabled
 * - Default gain (1x)
 * - Default integration time (154ms)
 */
bool driver_tcs3472_init(driver_tcs3472_t *driver, hw_i2c_t *i2c);

/**
 * @brief Initialize the TCS3472 sensor with custom settings
 * @ingroup TCS3472
 * @param driver Pointer to the TCS3472 driver structure
 * @param i2c Pointer to an initialized I2C bus handle
 * @param gain Gain setting
 * @param integration Integration time setting
 * @return true if initialization was successful, false otherwise
 */
bool driver_tcs3472_init_with_config(
    driver_tcs3472_t *driver, hw_i2c_t *i2c, driver_tcs3472_gain_t gain,
    driver_tcs3472_integration_time_t integration);

/**
 * @brief Finalize and power down the TCS3472 sensor
 * @ingroup TCS3472
 * @param driver Pointer to the TCS3472 driver structure
 */
void driver_tcs3472_finalize(driver_tcs3472_t *driver);

///////////////////////////////////////////////////////////////////////////////
// DATA ACQUISITION

/**
 * @brief Read RGBC color data
 * @ingroup TCS3472
 * @param driver Pointer to the TCS3472 driver structure
 * @param data Pointer to structure to receive RGBC data
 * @return true if read was successful, false otherwise
 *
 * @note This function checks for valid data (AVALID bit) before reading
 */
bool driver_tcs3472_read(driver_tcs3472_t *driver, driver_tcs3472_rgbc_t *data);
