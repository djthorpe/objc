/**
 * @file drivers_bme280.h
 * @brief BME280 sensor interface
 * @defgroup BME280 BME280
 * @ingroup Drivers
 *
 * BME280 sensor interface for reading temperature, humidity, and pressure.
 */
#pragma once
#include <runtime-hw/hw.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

#define DRIVER_BME280_RESERVED_SIZE 96 ///< Reserved size for internal use

/**
 * @brief BME280 sensor driver structure.
 * @ingroup BME280
 * @headerfile drivers_bme280.h drivers/drivers.h
 *
 * This structure contains the configuration and state information for a BME280
 * sensor instance. The BME280 is a combined digital humidity, pressure and
 * temperature sensor from Bosch Sensortec.
 */
typedef struct driver_bme280_t {
  hw_i2c_t *i2c;       ///< Pointer to the I2C interface used for communication
  uint8_t address;     ///< I2C address of the BME280 sensor
  uint8_t timeout_ms;  ///< Read/write timeout in milliseconds
  uint8_t _padding[6]; ///< Padding to ensure proper alignment
  uint8_t reserved[DRIVER_BME280_RESERVED_SIZE]
      __attribute__((aligned(8))); ///< Reserved for internal use
} driver_bme280_t;

typedef enum {
  DRIVER_BME280_MODE_SLEEP = 1, ///< Sleep mode
  DRIVER_BME280_MODE_FORCED,    ///< Forced mode
  DRIVER_BME280_MODE_NORMAL     ///< Normal mode
} driver_bme280_mode_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a BME280 sensor driver using the I2C interface.
 * @ingroup BME280
 * @param i2c Pointer to an initialized I2C interface for sensor communication.
 * @param address The I2C address of the BME280 sensor (0x76 or 0x77). If set to
 * 0, the default address (0x76) will be used.
 * @return A BME280 driver structure representing the initialized sensor.
 *
 * This function initializes a BME280 sensor driver for communication over I2C.
 * The BME280 sensor supports two I2C addresses: 0x76 (when SDO pin is connected
 * to GND) and 0x77 (when SDO pin is connected to VDD).
 *
 * The function performs initial sensor detection and configuration, including:
 * - Verifying sensor presence at the specified address
 * - Reading calibration coefficients from sensor EEPROM
 * - Setting default measurement configuration
 *
 * @note The i2c pointer must be valid and the I2C interface must be properly
 * initialized.
 * @see hw_i2c_init() for I2C interface initialization.
 * @see driver_bme280_finalize() for cleanup and resource release.
 */
driver_bme280_t driver_bme280_i2c_init(hw_i2c_t *i2c, uint8_t address);

/**
 * @brief Finalize and release a BME280 sensor driver.
 * @ingroup BME280
 * @param bme280 Pointer to the BME280 driver structure to finalize.
 *
 * This function releases resources associated with the BME280 sensor driver
 * and puts the sensor into a low-power sleep mode. After calling this function,
 * the BME280 driver should not be used for further operations.
 *
 * The function performs cleanup operations including:
 * - Putting the sensor into sleep mode to reduce power consumption
 * - Releasing any allocated calibration data
 * - Clearing the driver structure
 *
 * @note After finalization, the sensor driver should not be used.
 * @note This function does not finalize the underlying I2C interface.
 */
void driver_bme280_finalize(driver_bme280_t *bme280);

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Get true if the BME280 interface is valid.
 * @ingroup BME280
 * @return True if the BME280 interface is valid, false otherwise.
 *
 * The result of driver_bme280_init can return an empty structure if the
 * initialization fails. This function checks if the BME280 interface is valid.
 */
static inline bool driver_bme280_valid(driver_bme280_t *bme280) {
  return bme280 && bme280->i2c && hw_i2c_valid(bme280->i2c) && bme280->address;
}

/**
 * @brief Set the operating mode of the BME280 sensor.
 * @ingroup BME280
 * @param bme280 Pointer to the BME280 driver structure.
 * @param mode The operating mode to set for the sensor.
 * @return true if the mode was successfully set, false on error.
 *
 * This function configures the BME280 sensor's operating mode, which determines
 * how the sensor performs measurements and manages power consumption:
 *
 * - **SLEEP mode**: The sensor is in standby, consuming minimal power.
 *   No measurements are performed.
 * - **FORCED mode**: The sensor performs one measurement cycle of temperature,
 *   pressure, and humidity, then returns to sleep mode automatically.
 * - **NORMAL mode**: The sensor continuously performs measurements at regular
 *   intervals according to the configured standby time.
 *
 * The function writes to the sensor's control register to change the operating
 * mode. In forced mode, the sensor will automatically return to sleep mode
 * after completing the measurement cycle.
 *
 * @note Mode changes may take some time to take effect depending on the sensor
 * state.
 * @note In forced mode, measurements should be read before the sensor returns
 * to sleep.
 * @see driver_bme280_get_mode() for reading the current operating mode.
 * @see driver_bme280_mode_t for available operating modes.
 */
bool driver_bme280_set_mode(driver_bme280_t *bme280, driver_bme280_mode_t mode);

/**
 * @brief Get the current operating mode of the BME280 sensor.
 * @ingroup BME280
 * @param bme280 Pointer to the BME280 driver structure.
 * @return The current operating mode of the sensor, or 0 on error.
 *
 * This function reads the sensor's control register to determine the current
 * operating mode. The returned mode indicates how the sensor is currently
 * configured for measurements and power management.
 *
 * Note that in forced mode, the sensor automatically transitions back to sleep
 * mode after completing a measurement cycle, so the returned mode may be
 * DRIVER_BME280_MODE_SLEEP even if forced mode was recently set.
 *
 * @note The bme280 pointer should not be NULL and must be properly initialized.
 * @note The function returns 0 if an error occurs during communication.
 * @note In forced mode, the sensor may have already returned to sleep mode when
 * this is called.
 * @see driver_bme280_set_mode() for setting the operating mode.
 * @see driver_bme280_mode_t for available operating modes.
 */
driver_bme280_mode_t driver_bme280_get_mode(driver_bme280_t *bme280);

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Reset the BME280 sensor to its default state.
 * @ingroup BME280
 * @param bme280 Pointer to the BME280 driver structure.
 * @return true if the reset was successful, false on error.
 *
 * This function performs a software reset of the BME280 sensor by writing to
 * the reset register. The reset operation restores the sensor to its power-on
 * default state, which includes:
 *
 * - All configuration registers are reset to their default values
 * - The sensor enters sleep mode (lowest power consumption)
 * - All previous measurement settings are cleared
 * - Internal state machines are reinitialized
 *
 * After a successful reset, the sensor will be in the same state as immediately
 * after power-on. Any previous configuration (sampling rates, filter settings,
 * standby times) will need to be reconfigured.
 */
bool driver_bme280_reset(driver_bme280_t *bme280);

/**
 * @brief Read temperature, pressure, and humidity data from the BME280 sensor.
 * @ingroup BME280
 * @param bme280 Pointer to the BME280 driver structure.
 * @param temperature Pointer to store temperature in degrees Celsius (can be
 * NULL if not needed).
 * @param pressure Pointer to store pressure in Pascals (can be NULL if not
 * needed).
 * @param humidity Pointer to store relative humidity in % (can be NULL if not
 * needed).
 * @return true if data was successfully read, false on error.
 *
 * This function reads the current sensor values. If the sensor is in SLEEP
 * mode, it will automatically switch to FORCED mode, take a measurement, then
 * return to SLEEP. For NORMAL mode, it reads the most recent measurement.
 */
bool driver_bme280_read_data(driver_bme280_t *bme280, float *temperature,
                             float *pressure, float *humidity);

/**
 * @brief Read only temperature from the BME280 sensor.
 * @ingroup BME280
 * @param bme280 Pointer to the BME280 driver structure.
 * @return Temperature in degrees Celsius, or NaN on error.
 */
float driver_bme280_read_temperature(driver_bme280_t *bme280);

/**
 * @brief Read only pressure from the BME280 sensor.
 * @ingroup BME280
 * @param bme280 Pointer to the BME280 driver structure.
 * @return Pressure in Pascals, or NaN on error.
 */
float driver_bme280_read_pressure(driver_bme280_t *bme280);

/**
 * @brief Read only humidity from the BME280 sensor.
 * @ingroup BME280
 * @param bme280 Pointer to the BME280 driver structure.
 * @return Relative humidity in %, or NaN on error.
 */
float driver_bme280_read_humidity(driver_bme280_t *bme280);

/**
 * @brief Calculate altitude from pressure measurement.
 * @ingroup BME280
 * @param pressure_pa Current pressure in Pascals.
 * @param sea_level_pa Sea level pressure in Pascals (default: 101325).
 * @return Altitude in meters.
 *
 * Uses the barometric formula to estimate altitude. Accuracy depends on
 * knowing the current sea level pressure for your location.
 */
float driver_bme280_calculate_altitude(float pressure_pa, float sea_level_pa);

/**
 * @brief Calculate sea level pressure from current pressure and altitude.
 * @ingroup BME280
 * @param pressure_pa Current pressure in Pascals.
 * @param altitude_m Known altitude in meters.
 * @return Estimated sea level pressure in Pascals.
 */
float driver_bme280_calculate_sea_level_pressure(float pressure_pa,
                                                 float altitude_m);

// Default sea level pressure constant
#define DRIVER_BME280_SEA_LEVEL_PRESSURE_PA                                    \
  101325.0f ///< Standard sea level pressure in Pascals
