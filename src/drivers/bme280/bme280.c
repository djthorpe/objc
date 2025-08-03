#include <BME280_SensorAPI/bme280.h>
#include <drivers/drivers.h>
#include <math.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

BME280_INTF_RET_TYPE _driver_bme280_i2c_read(uint8_t reg, uint8_t *data,
                                             uint32_t len, void *userdata);

BME280_INTF_RET_TYPE _driver_bme280_i2c_write(uint8_t reg, const uint8_t *data,
                                              uint32_t len, void *userdata);

void _driver_bme280_delay_us(uint32_t period, void *intf_ptr);

static struct bme280_dev *_driver_bme280_dev(driver_bme280_t *bme280);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a BME280 sensor driver.
 */
driver_bme280_t driver_bme280_i2c_init(hw_i2c_t *i2c, uint8_t address) {
  sys_assert(i2c);
  sys_assert(hw_i2c_valid(i2c));
  sys_assert(sizeof(struct bme280_dev) <= DRIVER_BME280_RESERVED_SIZE);

  // Create the BME280 driver structure
  driver_bme280_t bme280 = {0};
  struct bme280_dev *dev = (struct bme280_dev *)bme280.reserved;
  sys_memset(dev, 0, sizeof(struct bme280_dev));

  // Address and timeout parameters
  uint8_t timeout_ms = 100;
  if (address == 0) {
    address = BME280_I2C_ADDR_PRIM;
  }

  // Initialize the BME280 sensor data structures
  dev->intf = BME280_I2C_INTF;
  dev->intf_ptr = i2c;
  dev->read = _driver_bme280_i2c_read;
  dev->write = _driver_bme280_i2c_write;
  dev->delay_us = _driver_bme280_delay_us;

  // Set the I2C interface parameters
  bme280.address = address;
  bme280.timeout_ms = timeout_ms;

  // Set up the BME280 device
  int8_t rslt = bme280_init(_driver_bme280_dev(&bme280));
  if (rslt == BME280_OK) {
    bme280.i2c = i2c;
  } else {
#ifdef DEBUG
    sys_printf("bme280_init() error code %d\n", rslt);
#endif
    bme280.address = 0;
    bme280.timeout_ms = 0;
  }

  // Return success (or failure)
  return bme280;
}

/**
 * @brief Finalize and release a BME280 sensor driver.
 */
void driver_bme280_finalize(driver_bme280_t *bme280) {
  sys_assert(bme280 && driver_bme280_valid(bme280));

  // Put BME280 into sleep mode
  bme280_set_sensor_mode(BME280_POWERMODE_SLEEP, _driver_bme280_dev(bme280));

  // Clear the driver structure
  sys_memset(bme280, 0, sizeof(driver_bme280_t));
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Set the operating mode of the BME280 sensor.
 */
bool driver_bme280_set_mode(driver_bme280_t *bme280,
                            driver_bme280_mode_t mode) {
  sys_assert(bme280 && driver_bme280_valid(bme280));

  uint8_t sensor_mode;
  switch (mode) {
  case DRIVER_BME280_MODE_SLEEP:
    sensor_mode = BME280_POWERMODE_SLEEP;
    break;
  case DRIVER_BME280_MODE_FORCED:
    sensor_mode = BME280_POWERMODE_FORCED;
    break;
  case DRIVER_BME280_MODE_NORMAL:
    sensor_mode = BME280_POWERMODE_NORMAL;
    break;
  default:
    return false; // Invalid mode
  }

  return bme280_set_sensor_mode(sensor_mode, _driver_bme280_dev(bme280)) ==
         BME280_OK;
}

/**
 * @brief Get the current operating mode of the BME280 sensor.
 */
driver_bme280_mode_t driver_bme280_get_mode(driver_bme280_t *bme280) {
  sys_assert(bme280 && driver_bme280_valid(bme280));

  uint8_t mode;
  int8_t rslt = bme280_get_sensor_mode(&mode, _driver_bme280_dev(bme280));
  if (rslt != BME280_OK) {
#ifdef DEBUG
    sys_printf("bme280_get_sensor_mode() error code %d\n", rslt);
#endif
    return 0; // Return 0 on error
  }

  switch (mode) {
  case BME280_POWERMODE_SLEEP:
    return DRIVER_BME280_MODE_SLEEP;
  case BME280_POWERMODE_FORCED:
    return DRIVER_BME280_MODE_FORCED;
  case BME280_POWERMODE_NORMAL:
    return DRIVER_BME280_MODE_NORMAL;
  default:
    return 0; // Return 0 for unknown/invalid modes
  }
}

/**
 * @brief Reset the BME280 sensor to its default state.
 */
bool driver_bme280_reset(driver_bme280_t *bme280) {
  sys_assert(bme280 && driver_bme280_valid(bme280));

  int8_t rslt = bme280_soft_reset(_driver_bme280_dev(bme280));
  if (rslt != BME280_OK) {
#ifdef DEBUG
    sys_printf("bme280_soft_reset() error code %d\n", rslt);
#endif
    return false;
  }

  return true;
}

/**
 * @brief Read temperature, pressure, and humidity data from the BME280 sensor.
 */
bool driver_bme280_read_data(driver_bme280_t *bme280, float *temperature,
                             float *pressure, float *humidity) {
  sys_assert(bme280 && driver_bme280_valid(bme280));

  struct bme280_dev *dev = _driver_bme280_dev(bme280);
  struct bme280_data comp_data;

  // Check current mode
  uint8_t current_mode;
  int8_t rslt = bme280_get_sensor_mode(&current_mode, dev);
  if (rslt != BME280_OK) {
#ifdef DEBUG
    sys_printf("bme280_get_sensor_mode() error code %d\n", rslt);
#endif
    return false;
  }

  // If sensor is in sleep mode, trigger a forced measurement
  bool was_sleeping = (current_mode == BME280_POWERMODE_SLEEP);
  if (was_sleeping) {
    rslt = bme280_set_sensor_mode(BME280_POWERMODE_FORCED, dev);
    if (rslt != BME280_OK) {
#ifdef DEBUG
      sys_printf("bme280_set_sensor_mode(FORCED) error code %d\n", rslt);
#endif
      return false;
    }

    // Wait for measurement to complete
    // Calculate measurement delay based on current settings
    struct bme280_settings settings;
    rslt = bme280_get_sensor_settings(&settings, dev);
    if (rslt == BME280_OK) {
      uint32_t delay_ms;
      rslt = bme280_cal_meas_delay(&delay_ms, &settings);
      if (rslt == BME280_OK) {
        sys_sleep(delay_ms);
      } else {
        // Use default delay if calculation fails
        sys_sleep(10); // 10ms default
      }
    } else {
      sys_sleep(10); // 10ms default
    }
  }

  // Read all sensor data (temperature, pressure, humidity)
  uint8_t sensor_comp = BME280_PRESS | BME280_TEMP | BME280_HUM;
  rslt = bme280_get_sensor_data(sensor_comp, &comp_data, dev);
  if (rslt != BME280_OK) {
#ifdef DEBUG
    sys_printf("bme280_get_sensor_data() error code %d\n", rslt);
#endif
    return false;
  }

  // Store results in output parameters
  // Note: Using BME280_32BIT_ENABLE for integer mode with specific scaling:
  // - Temperature: DegC * 100 (divide by 100 for actual degrees)
  // - Pressure: Pa (already in correct units for 32-bit mode)
  // - Humidity: %RH * 1000 (divide by 1000 for actual percentage)
  if (temperature != NULL) {
#ifdef BME280_DOUBLE_ENABLE
    *temperature = (float)comp_data.temperature; // Already in degrees Celsius
#else
    *temperature =
        (float)comp_data.temperature / 100.0f; // Convert from DegC*100 to DegC
#endif
  }

  if (pressure != NULL) {
#ifdef BME280_DOUBLE_ENABLE
    *pressure = (float)comp_data.pressure; // Already in Pascals
#else
    *pressure = (float)comp_data.pressure; // Already in Pascals for 32-bit mode
#endif
  }

  if (humidity != NULL) {
#ifdef BME280_DOUBLE_ENABLE
    *humidity = (float)comp_data.humidity; // Already in percentage
#else
    *humidity =
        (float)comp_data.humidity / 1000.0f; // Convert from %RH*1000 to %RH
#endif
  }

  return true;
}

/**
 * @brief Read only temperature from the BME280 sensor.
 */
float driver_bme280_read_temperature(driver_bme280_t *bme280) {
  float temperature;
  if (driver_bme280_read_data(bme280, &temperature, NULL, NULL)) {
    return temperature;
  }
  return NAN; // Return NaN on error
}

/**
 * @brief Read only pressure from the BME280 sensor.
 */
float driver_bme280_read_pressure(driver_bme280_t *bme280) {
  float pressure;
  if (driver_bme280_read_data(bme280, NULL, &pressure, NULL)) {
    return pressure;
  }
  return NAN; // Return NaN on error
}

/**
 * @brief Read only humidity from the BME280 sensor.
 */
float driver_bme280_read_humidity(driver_bme280_t *bme280) {
  float humidity;
  if (driver_bme280_read_data(bme280, NULL, NULL, &humidity)) {
    return humidity;
  }
  return NAN; // Return NaN on error
}

/**
 * @brief Get the chip ID of the BME280 sensor.
 */
uint8_t driver_bme280_get_chip_id(driver_bme280_t *bme280) {
  sys_assert(bme280 && driver_bme280_valid(bme280));

  struct bme280_dev *dev = _driver_bme280_dev(bme280);
  return dev->chip_id;
}

/**
 * @brief Calculate altitude from pressure measurement.
 */
float driver_bme280_calculate_altitude(float pressure_pa, float sea_level_pa) {
  if (sea_level_pa <= 0.0f) {
    sea_level_pa = DRIVER_BME280_SEA_LEVEL_PRESSURE_PA;
  }

  // Barometric formula: h = 44330 * (1 - (P/P0)^(1/5.255))
  float ratio = pressure_pa / sea_level_pa;
  float altitude = 44330.0f * (1.0f - powf(ratio, 1.0f / 5.255f));
  return altitude;
}

/**
 * @brief Calculate sea level pressure from current pressure and altitude.
 */
float driver_bme280_calculate_sea_level_pressure(float pressure_pa,
                                                 float altitude_m) {
  // Inverse barometric formula: P0 = P / (1 - h/44330)^5.255
  float ratio = 1.0f - (altitude_m / 44330.0f);
  float sea_level_pressure = pressure_pa / powf(ratio, 5.255f);
  return sea_level_pressure;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

BME280_INTF_RET_TYPE _driver_bme280_i2c_read(uint8_t reg, uint8_t *data,
                                             uint32_t len, void *userdata) {
  sys_assert(userdata);
  sys_assert(data || len == 0);

  hw_i2c_t *i2c = (hw_i2c_t *)userdata;
  uint8_t addr = i2c->reserved[I2C_RESERVED_ADDR_IDX];
  uint8_t timeout_ms = i2c->reserved[I2C_RESERVED_TIMEOUT_IDX];
  size_t bytes_read = hw_i2c_read(i2c, addr, reg, data, len, timeout_ms);
  return (bytes_read == len) ? BME280_OK : BME280_E_COMM_FAIL;
}

BME280_INTF_RET_TYPE _driver_bme280_i2c_write(uint8_t reg, const uint8_t *data,
                                              uint32_t len, void *userdata) {
  sys_assert(userdata);
  sys_assert(data || len == 0);

  hw_i2c_t *i2c = (hw_i2c_t *)userdata;
  uint8_t addr = i2c->reserved[HW_I2C_RESERVED_ADDR_IDX];
  uint8_t timeout_ms = i2c->reserved[HW_I2C_RESERVED_TIMEOUT_IDX];
  size_t bytes_written = hw_i2c_write(i2c, addr, reg, data, len, timeout_ms);
  return (bytes_written == len) ? BME280_OK : BME280_E_COMM_FAIL;
}

/*!
 * Delay function map to COINES platform
 */
void _driver_bme280_delay_us(uint32_t period, void *userdata) {
  (void)userdata; // Unused parameter
  (void)period;   // Unused parameter
#ifdef DEBUG
  sys_printf("TODO: Setting BME280 delay: %u us\n", period);
#endif
}

/*!
 * Helper function to get the BME280 device structure and set up I2C parameters
 */
static struct bme280_dev *_driver_bme280_dev(driver_bme280_t *bme280) {
  sys_assert(bme280);
  struct bme280_dev *dev = (struct bme280_dev *)bme280->reserved;
  hw_i2c_t *i2c = dev->intf_ptr;
  i2c->reserved[HW_I2C_RESERVED_ADDR_IDX] = bme280->address;
  i2c->reserved[HW_I2C_RESERVED_TIMEOUT_IDX] = bme280->timeout_ms;
  return dev;
}
