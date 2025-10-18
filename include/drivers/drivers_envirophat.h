/**
 * @file drivers_envirophat.h
 * @brief Enviro pHAT sensor interface
 * @defgroup Envirophat Enviro pHAT
 * @ingroup Drivers
 *
 * Enviro pHAT interface for reading temperature, humidity, pressure,
 * light, and color data using the BME280 and TCS34725 sensors,
 * and LSM303 accelerometer/magnetometer, plus ADS1015 ADC.
 */
#pragma once
#include <drivers/drivers_bme280.h>
#include <drivers/drivers_lsm303.h>
#include <drivers/drivers_tcs3472.h>
#include <runtime-hw/hw.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Enviro pHAT sensor driver structure.
 * @ingroup Envirophat
 * @headerfile drivers_envirophat.h drivers/drivers.h
 *
 * This structure contains the configuration and state information for the
 * Enviro pHAT sensor instance, which includes TCS3472 color sensor,
 * LSM303 accelerometer/magnetometer, and BME280 environmental sensor.
 */
typedef struct driver_envirophat_t {
  hw_gpio_t led;            ///< LED GPIO pin
  hw_i2c_t i2c;             ///< I2C bus handle
  driver_tcs3472_t tcs3472; ///< TCS3472 color sensor
  driver_lsm303_t lsm303;   ///< LSM303 accelerometer/magnetometer
  driver_bme280_t bme280;   ///< BME280 environmental sensor
} driver_envirophat_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a Enviro pHAT driver using the I2C interface.
 * @ingroup Envirophat
 * @param driver Pointer to the Enviro pHAT driver structure to initialize.
 * @return true if the initialization was successful, false otherwise.
 *
 * This function initializes a Enviro pHAT driver.
 */
bool driver_envirophat_init(driver_envirophat_t *driver);

/**
 * @brief Finalize and release a Enviro pHAT driver.
 * @ingroup Envirophat
 * @param driver Pointer to the Enviro pHAT driver structure to finalize.
 */
void driver_envirophat_finalize(driver_envirophat_t *driver);

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Set the state of the Enviro pHAT LED.
 * @ingroup Envirophat
 * @param driver Pointer to the Enviro pHAT driver structure.
 * @param state true to turn the LED on, false to turn it off.
 */
void driver_envirophat_set_led(driver_envirophat_t *driver, bool state);

///////////////////////////////////////////////////////////////////////////////
// COLOR SENSOR

/**
 * @brief Read RGBC color data from the TCS3472 sensor.
 * @ingroup Envirophat
 * @param driver Pointer to the Enviro pHAT driver structure.
 * @param data Pointer to structure to receive RGBC data.
 * @return true if read was successful, false otherwise.
 */
bool driver_envirophat_read_color(driver_envirophat_t *driver,
                                  driver_tcs3472_rgbc_t *data);

///////////////////////////////////////////////////////////////////////////////
// ACCELEROMETER / MAGNETOMETER

/**
 * @brief Read accelerometer data from the LSM303 sensor.
 * @ingroup Envirophat
 * @param driver Pointer to the Enviro pHAT driver structure.
 * @param data Pointer to vector structure to receive x, y, z acceleration in g.
 * @return true if read was successful, false otherwise.
 */
bool driver_envirophat_read_accel(driver_envirophat_t *driver,
                                  driver_lsm303_vec3_t *data);

/**
 * @brief Read magnetometer data from the LSM303 sensor.
 * @ingroup Envirophat
 * @param driver Pointer to the Enviro pHAT driver structure.
 * @param data Pointer to vector structure to receive x, y, z magnetic field in
 * gauss.
 * @return true if read was successful, false otherwise.
 */
bool driver_envirophat_read_mag(driver_envirophat_t *driver,
                                driver_lsm303_vec3_t *data);

/**
 * @brief Read temperature from the LSM303 sensor.
 * @ingroup Envirophat
 * @param driver Pointer to the Enviro pHAT driver structure.
 * @param temp Pointer to float to receive temperature in degrees Celsius.
 * @return true if read was successful, false otherwise.
 */
bool driver_envirophat_read_temp(driver_envirophat_t *driver, float *temp);

///////////////////////////////////////////////////////////////////////////////
// BME280 ENVIRONMENTAL SENSOR

/**
 * @brief Read environmental data from BME280 sensor.
 * @ingroup Envirophat
 * @param driver Pointer to the Enviro pHAT driver structure.
 * @param data Pointer to structure to receive temperature, pressure, humidity.
 * @return true if read was successful, false otherwise (sensor not present or
 * read failed).
 */
bool driver_envirophat_read_environmental(driver_envirophat_t *driver,
                                          driver_bme280_data_t *data);
