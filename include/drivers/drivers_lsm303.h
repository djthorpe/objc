/**
 * @file drivers_lsm303.h
 * @brief LSM303D 3D Accelerometer and Magnetometer Driver
 * @defgroup LSM303 LSM303D Driver
 * @ingroup Drivers
 *
 * Driver for the LSM303D 3-axis accelerometer and 3-axis magnetometer with
 * integrated temperature sensor.
 */
#pragma once
#include <runtime-hw/hw.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Accelerometer full scale range
 * @ingroup LSM303
 */
typedef enum {
  DRIVER_LSM303_ACCEL_SCALE_2G = 0, ///< ±2g (default)
  DRIVER_LSM303_ACCEL_SCALE_4G,     ///< ±4g
  DRIVER_LSM303_ACCEL_SCALE_6G,     ///< ±6g
  DRIVER_LSM303_ACCEL_SCALE_8G,     ///< ±8g
  DRIVER_LSM303_ACCEL_SCALE_16G     ///< ±16g
} driver_lsm303_accel_scale_t;

/**
 * @brief Magnetometer full scale range
 * @ingroup LSM303
 */
typedef enum {
  DRIVER_LSM303_MAG_SCALE_2GAUSS = 0, ///< ±2 gauss (default)
  DRIVER_LSM303_MAG_SCALE_4GAUSS,     ///< ±4 gauss
  DRIVER_LSM303_MAG_SCALE_8GAUSS,     ///< ±8 gauss
  DRIVER_LSM303_MAG_SCALE_12GAUSS     ///< ±12 gauss
} driver_lsm303_mag_scale_t;

/**
 * @brief 3-axis vector data structure
 * @ingroup LSM303
 */
typedef struct {
  float x; ///< X-axis value
  float y; ///< Y-axis value
  float z; ///< Z-axis value
} driver_lsm303_vec3_t;

/**
 * @brief LSM303D driver instance
 * @ingroup LSM303
 */
typedef struct {
  hw_i2c_t i2c;                            ///< I2C interface
  driver_lsm303_accel_scale_t accel_scale; ///< Current accelerometer scale
  driver_lsm303_mag_scale_t mag_scale;     ///< Current magnetometer scale
} driver_lsm303_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize LSM303D with default settings
 * @ingroup LSM303
 *
 * Initializes the sensor with:
 * - Accelerometer: ±2g, 50Hz
 * - Magnetometer: ±2 gauss, 50Hz, continuous mode
 * - Temperature sensor enabled
 *
 * @param driver Pointer to driver structure
 * @param i2c Pointer to initialized I2C interface
 * @return true if initialization succeeded, false otherwise
 */
bool driver_lsm303_init(driver_lsm303_t *driver, hw_i2c_t *i2c);

/**
 * @brief Initialize LSM303D with custom scale settings
 * @ingroup LSM303
 *
 * @param driver Pointer to driver structure
 * @param i2c Pointer to initialized I2C interface
 * @param accel_scale Accelerometer full-scale range
 * @param mag_scale Magnetometer full-scale range
 * @return true if initialization succeeded, false otherwise
 */
bool driver_lsm303_init_with_config(driver_lsm303_t *driver, hw_i2c_t *i2c,
                                    driver_lsm303_accel_scale_t accel_scale,
                                    driver_lsm303_mag_scale_t mag_scale);

/**
 * @brief Finalize and power down LSM303D
 * @ingroup LSM303
 *
 * @param driver Pointer to driver structure
 */
void driver_lsm303_finalize(driver_lsm303_t *driver);

///////////////////////////////////////////////////////////////////////////////
// DATA ACQUISITION

/**
 * @brief Read accelerometer data
 * @ingroup LSM303
 *
 * Returns acceleration in g (Earth gravity = 1g ≈ 9.81 m/s²)
 *
 * @param driver Pointer to driver structure
 * @param data Pointer to vector structure to receive x, y, z values in g
 * @return true if read succeeded, false otherwise
 */
bool driver_lsm303_read_accel(driver_lsm303_t *driver,
                              driver_lsm303_vec3_t *data);

/**
 * @brief Read magnetometer data
 * @ingroup LSM303
 *
 * Returns magnetic field strength in gauss
 *
 * @param driver Pointer to driver structure
 * @param data Pointer to vector structure to receive x, y, z values in gauss
 * @return true if read succeeded, false otherwise
 */
bool driver_lsm303_read_mag(driver_lsm303_t *driver,
                            driver_lsm303_vec3_t *data);

/**
 * @brief Read temperature
 * @ingroup LSM303
 *
 * @param driver Pointer to driver structure
 * @param temp Pointer to float to receive temperature in degrees Celsius
 * @return true if read succeeded, false otherwise
 */
bool driver_lsm303_read_temp(driver_lsm303_t *driver, float *temp);

///////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS

/**
 * @brief Calculate compass heading from magnetometer data
 * @ingroup LSM303
 *
 * Calculates the compass heading (direction) from magnetometer readings.
 * Note: This assumes the sensor is held level (horizontal). For accurate
 * readings when tilted, tilt compensation using accelerometer data is required.
 *
 * @param mag Pointer to magnetometer vector data in gauss
 * @return Heading in degrees (0-360, where 0=North, 90=East, 180=South,
 * 270=West) Returns -1.0 if mag is NULL
 */
float driver_lsm303_get_heading(const driver_lsm303_vec3_t *mag);
