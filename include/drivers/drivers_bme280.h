/**
 * @file drivers_bme280.h
 * @brief BME280 Temperature, Pressure, and Humidity Sensor Driver
 * @defgroup BME280 BME280 Driver
 * @ingroup Drivers
 *
 * Standalone driver for the Bosch BME280 environmental sensor.
 * Provides temperature, pressure, and humidity measurements.
 */
#pragma once
#include <runtime-hw/hw.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief BME280 oversampling settings
 * @ingroup BME280
 */
typedef enum {
  DRIVER_BME280_OVERSAMPLING_NONE = 0, ///< No oversampling (output set to 0)
  DRIVER_BME280_OVERSAMPLING_1X = 1,   ///< Oversampling ×1
  DRIVER_BME280_OVERSAMPLING_2X = 2,   ///< Oversampling ×2
  DRIVER_BME280_OVERSAMPLING_4X = 3,   ///< Oversampling ×4
  DRIVER_BME280_OVERSAMPLING_8X = 4,   ///< Oversampling ×8
  DRIVER_BME280_OVERSAMPLING_16X = 5   ///< Oversampling ×16
} driver_bme280_oversampling_t;

/**
 * @brief BME280 IIR filter coefficients
 * @ingroup BME280
 */
typedef enum {
  DRIVER_BME280_FILTER_OFF = 0, ///< Filter off
  DRIVER_BME280_FILTER_2 = 1,   ///< Filter coefficient 2
  DRIVER_BME280_FILTER_4 = 2,   ///< Filter coefficient 4
  DRIVER_BME280_FILTER_8 = 3,   ///< Filter coefficient 8
  DRIVER_BME280_FILTER_16 = 4   ///< Filter coefficient 16
} driver_bme280_filter_t;

/**
 * @brief BME280 measurement data
 * @ingroup BME280
 */
typedef struct {
  float temperature; ///< Temperature in °C
  float pressure;    ///< Pressure in Pa
  float humidity;    ///< Relative humidity in %
} driver_bme280_data_t;

/**
 * @brief BME280 driver instance
 * @ingroup BME280
 */
typedef struct {
  hw_i2c_t i2c;          ///< I2C interface
  uint8_t addr;          ///< I2C address (0x76 or 0x77)
  uint8_t _reserved[64]; ///< Internal calibration data
} driver_bme280_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize BME280 with I2C interface and default settings
 * @ingroup BME280
 *
 * Initializes with default settings: 1x oversampling for all sensors, filter
 * off.
 *
 * @param driver Pointer to driver structure
 * @param i2c Pointer to initialized I2C interface
 * @param addr_offset Address offset (0 for 0x76, 1 for 0x77)
 * @return true if initialization succeeded, false otherwise
 */
bool driver_bme280_init_i2c(driver_bme280_t *driver, hw_i2c_t *i2c,
                            uint8_t addr_offset);

/**
 * @brief Initialize BME280 with I2C interface and custom settings
 * @ingroup BME280
 *
 * Allows configuration of oversampling and filter settings.
 *
 * @param driver Pointer to driver structure
 * @param i2c Pointer to initialized I2C interface
 * @param addr_offset Address offset (0 for 0x76, 1 for 0x77)
 * @param temp_os Temperature oversampling
 * @param press_os Pressure oversampling
 * @param hum_os Humidity oversampling
 * @param filter IIR filter coefficient
 * @return true if initialization succeeded, false otherwise
 */
bool driver_bme280_init_i2c_with_config(driver_bme280_t *driver, hw_i2c_t *i2c,
                                        uint8_t addr_offset,
                                        driver_bme280_oversampling_t temp_os,
                                        driver_bme280_oversampling_t press_os,
                                        driver_bme280_oversampling_t hum_os,
                                        driver_bme280_filter_t filter);

/**
 * @brief Check if driver instance is valid
 * @ingroup BME280
 *
 * @param driver Pointer to driver structure
 * @return true if valid, false otherwise
 */
bool driver_bme280_valid(driver_bme280_t *driver);

/**
 * @brief Finalize and power down BME280
 * @ingroup BME280
 *
 * @param driver Pointer to driver structure
 */
void driver_bme280_finalize(driver_bme280_t *driver);

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

/**
 * @brief Set temperature offset for calibration
 * @ingroup BME280
 *
 * Use this to compensate for self-heating or other temperature offsets.
 * For example, if the sensor reads 33°C but ambient is 22°C, use -11°C offset.
 *
 * @param driver Pointer to driver structure
 * @param offset Temperature offset in °C (can be negative)
 */
void driver_bme280_set_temp_offset(driver_bme280_t *driver, float offset);

///////////////////////////////////////////////////////////////////////////////
// DATA ACQUISITION

/**
 * @brief Read all sensor data (temperature, pressure, humidity)
 * @ingroup BME280
 *
 * Triggers a forced measurement (single shot), waits for completion, and
 * reads all three sensor values in a single transaction. The sensor returns
 * to sleep mode after the measurement.
 *
 * @param driver Pointer to driver structure
 * @param data Pointer to structure to receive measurement data
 * @return true if successful, false otherwise
 */
bool driver_bme280_read_data(driver_bme280_t *driver,
                             driver_bme280_data_t *data);

///////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS

/**
 * @brief Calculate altitude from pressure
 * @ingroup BME280
 *
 * Uses the international barometric formula.
 *
 * @param data Pointer to measurement data containing pressure
 * @param sea_level_pressure Sea level pressure in Pa (default: 101325 Pa)
 * @return Altitude in meters
 */
float driver_bme280_calculate_altitude(const driver_bme280_data_t *data,
                                       float sea_level_pressure);

/**
 * @brief Calculate sea level pressure from altitude
 * @ingroup BME280
 *
 * @param data Pointer to measurement data containing pressure
 * @param altitude Current altitude in meters
 * @return Sea level pressure in Pa
 */
float driver_bme280_calculate_sea_level_pressure(
    const driver_bme280_data_t *data, float altitude);
