/**
 * @file bme280.c
 * @brief BME280 Temperature, Pressure, and Humidity Sensor Driver
 * @ingroup BME280
 */
#include <drivers/drivers_bme280.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// CONSTANTS

// I2C addresses
#define BME280_I2C_ADDR_PRIMARY 0x76
#define BME280_I2C_ADDR_SECONDARY 0x77

// Register addresses
#define BME280_REG_CHIP_ID 0xD0
#define BME280_REG_RESET 0xE0
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_STATUS 0xF3
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG 0xF5
#define BME280_REG_PRESS_MSB 0xF7
#define BME280_REG_TEMP_MSB 0xFA
#define BME280_REG_HUM_MSB 0xFD

// Calibration data registers
#define BME280_REG_CALIB_00 0x88
#define BME280_REG_CALIB_26 0xE1

// Chip IDs
#define BME280_CHIP_ID 0x60 // BME280: temp + pressure + humidity
#define BMP280_CHIP_ID 0x58 // BMP280: temp + pressure only

// Soft reset command
#define BME280_SOFT_RESET_CMD 0xB6

// Default sea level pressure (Pa)
#define BME280_SEA_LEVEL_PRESSURE 101325.0f

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief BME280 calibration data (internal)
 */
typedef struct {
  uint16_t dig_t1;
  int16_t dig_t2;
  int16_t dig_t3;
  uint16_t dig_p1;
  int16_t dig_p2;
  int16_t dig_p3;
  int16_t dig_p4;
  int16_t dig_p5;
  int16_t dig_p6;
  int16_t dig_p7;
  int16_t dig_p8;
  int16_t dig_p9;
  uint8_t dig_h1;
  int16_t dig_h2;
  uint8_t dig_h3;
  int16_t dig_h4;
  int16_t dig_h5;
  int8_t dig_h6;
  int32_t t_fine; ///< Temperature value used in compensation
} bme280_calib_t;

/**
 * @brief BME280 driver internal structure with calibration data
 */
typedef struct {
  hw_i2c_t i2c;         ///< I2C interface
  uint8_t addr;         ///< I2C address (0x76 or 0x77)
  uint8_t chip_id;      ///< Chip ID (0x60=BME280, 0x58=BMP280)
  float temp_offset;    ///< Temperature offset in °C (for calibration)
  bme280_calib_t calib; ///< Calibration data
} driver_bme280_internal_t;

// Helper macro to cast public struct to internal struct
#define BME_INTERNAL(drv) ((driver_bme280_internal_t *)(drv))

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * @brief Write a single byte to a BME280 register
 */
static bool bme280_write_register(driver_bme280_t *driver, uint8_t reg,
                                  uint8_t value) {
  uint8_t buffer[2];
  buffer[0] = reg;
  buffer[1] = value;
  return hw_i2c_xfr(&driver->i2c, driver->addr, buffer, 2, 0, 0) == 2;
}

/**
 * @brief Read a single byte from a BME280 register
 */
static bool bme280_read_register(driver_bme280_t *driver, uint8_t reg,
                                 uint8_t *value) {
  uint8_t buffer[2];
  buffer[0] = reg;
  if (hw_i2c_xfr(&driver->i2c, driver->addr, buffer, 1, 1, 0) == 2) {
    *value = buffer[1];
    return true;
  }
  return false;
}

/**
 * @brief Read multiple bytes from BME280 registers
 */
static bool bme280_read_registers(driver_bme280_t *driver, uint8_t reg,
                                  uint8_t *buffer, uint8_t len) {
  uint8_t temp_buffer[32]; // Max read size
  if (len > 31)
    return false;

  temp_buffer[0] = reg;
  if (hw_i2c_xfr(&driver->i2c, driver->addr, temp_buffer, 1, len, 0) ==
      (size_t)(1 + len)) {
    sys_memcpy(buffer, &temp_buffer[1], len);
    return true;
  }
  return false;
}

/**
 * @brief Read and store calibration data
 */
static bool bme280_read_calibration(driver_bme280_t *driver) {
  uint8_t calib[26];
  uint8_t calib_h[7];

  // Read temperature and pressure calibration data (0x88-0xA1)
  if (!bme280_read_registers(driver, BME280_REG_CALIB_00, calib, 26)) {
    return false;
  }

  // Read humidity calibration data (0xE1-0xE7) - only for BME280
  if (BME_INTERNAL(driver)->chip_id == BME280_CHIP_ID) {
    if (!bme280_read_registers(driver, BME280_REG_CALIB_26, calib_h, 7)) {
      return false;
    }
  }

  // Parse temperature calibration
  BME_INTERNAL(driver)->calib.dig_t1 = (calib[1] << 8) | calib[0];
  BME_INTERNAL(driver)->calib.dig_t2 = (int16_t)((calib[3] << 8) | calib[2]);
  BME_INTERNAL(driver)->calib.dig_t3 = (int16_t)((calib[5] << 8) | calib[4]);

  // Parse pressure calibration
  BME_INTERNAL(driver)->calib.dig_p1 = (calib[7] << 8) | calib[6];
  BME_INTERNAL(driver)->calib.dig_p2 = (int16_t)((calib[9] << 8) | calib[8]);
  BME_INTERNAL(driver)->calib.dig_p3 = (int16_t)((calib[11] << 8) | calib[10]);
  BME_INTERNAL(driver)->calib.dig_p4 = (int16_t)((calib[13] << 8) | calib[12]);
  BME_INTERNAL(driver)->calib.dig_p5 = (int16_t)((calib[15] << 8) | calib[14]);
  BME_INTERNAL(driver)->calib.dig_p6 = (int16_t)((calib[17] << 8) | calib[16]);
  BME_INTERNAL(driver)->calib.dig_p7 = (int16_t)((calib[19] << 8) | calib[18]);
  BME_INTERNAL(driver)->calib.dig_p8 = (int16_t)((calib[21] << 8) | calib[20]);
  BME_INTERNAL(driver)->calib.dig_p9 = (int16_t)((calib[23] << 8) | calib[22]);

  // Parse humidity calibration (BME280 only, BMP280 doesn't have humidity)
  if (BME_INTERNAL(driver)->chip_id == BME280_CHIP_ID) {
    BME_INTERNAL(driver)->calib.dig_h1 = calib[25];
    BME_INTERNAL(driver)->calib.dig_h2 =
        (int16_t)((calib_h[1] << 8) | calib_h[0]);
    BME_INTERNAL(driver)->calib.dig_h3 = calib_h[2];
    BME_INTERNAL(driver)->calib.dig_h4 =
        (int16_t)((calib_h[3] << 4) | (calib_h[4] & 0x0F));
    BME_INTERNAL(driver)->calib.dig_h5 =
        (int16_t)((calib_h[5] << 4) | (calib_h[4] >> 4));
    BME_INTERNAL(driver)->calib.dig_h6 = (int8_t)calib_h[6];
  } else {
    // BMP280: Zero out humidity calibration
    BME_INTERNAL(driver)->calib.dig_h1 = 0;
    BME_INTERNAL(driver)->calib.dig_h2 = 0;
    BME_INTERNAL(driver)->calib.dig_h3 = 0;
    BME_INTERNAL(driver)->calib.dig_h4 = 0;
    BME_INTERNAL(driver)->calib.dig_h5 = 0;
    BME_INTERNAL(driver)->calib.dig_h6 = 0;
  }

  return true;
}

/**
 * @brief Compensate temperature (returns in °C, updates t_fine)
 * @note BMP280 datasheet integer compensation algorithm
 *       Returns temperature in DegC, resolution is 0.01 DegC
 */
static float bme280_compensate_temperature(driver_bme280_t *driver,
                                           int32_t adc_t) {
  int32_t var1, var2, T;

  var1 =
      ((((adc_t >> 3) - ((int32_t)BME_INTERNAL(driver)->calib.dig_t1 << 1))) *
       ((int32_t)BME_INTERNAL(driver)->calib.dig_t2)) >>
      11;
  var2 = (((((adc_t >> 4) - ((int32_t)BME_INTERNAL(driver)->calib.dig_t1)) *
            ((adc_t >> 4) - ((int32_t)BME_INTERNAL(driver)->calib.dig_t1))) >>
           12) *
          ((int32_t)BME_INTERNAL(driver)->calib.dig_t3)) >>
         14;

  BME_INTERNAL(driver)->calib.t_fine = var1 + var2;
  T = (BME_INTERNAL(driver)->calib.t_fine * 5 + 128) >> 8;

  return (float)T / 100.0f;
}

/**
 * @brief Compensate pressure (returns in Pa)
 * @note BMP280 datasheet 64-bit integer compensation algorithm (page 22)
 *       Returns pressure in Q24.8 format (24 integer bits, 8 fractional bits)
 *       Output of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
 */
static float bme280_compensate_pressure(driver_bme280_t *driver,
                                        int32_t adc_p) {
  int64_t var1, var2, p;

  var1 = ((int64_t)BME_INTERNAL(driver)->calib.t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)BME_INTERNAL(driver)->calib.dig_p6;
  var2 = var2 + ((var1 * (int64_t)BME_INTERNAL(driver)->calib.dig_p5) << 17);
  var2 = var2 + (((int64_t)BME_INTERNAL(driver)->calib.dig_p4) << 35);
  var1 = ((var1 * var1 * (int64_t)BME_INTERNAL(driver)->calib.dig_p3) >> 8) +
         ((var1 * (int64_t)BME_INTERNAL(driver)->calib.dig_p2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) *
             ((int64_t)BME_INTERNAL(driver)->calib.dig_p1) >>
         33;

  if (var1 == 0) {
    return 0.0f; // Avoid division by zero
  }

  p = 1048576 - adc_p;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 =
      (((int64_t)BME_INTERNAL(driver)->calib.dig_p9) * (p >> 13) * (p >> 13)) >>
      25;
  var2 = (((int64_t)BME_INTERNAL(driver)->calib.dig_p8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) +
      (((int64_t)BME_INTERNAL(driver)->calib.dig_p7) << 4);

  // Convert from Q24.8 format to Pa (divide by 256)
  return (float)p / 256.0f;
}

/**
 * @brief Compensate humidity (returns in %)
 */
static float bme280_compensate_humidity(driver_bme280_t *driver,
                                        int32_t adc_h) {
  int32_t v_x1_u32r;

  v_x1_u32r = (BME_INTERNAL(driver)->calib.t_fine - ((int32_t)76800));
  v_x1_u32r =
      (((((adc_h << 14) -
          (((int32_t)BME_INTERNAL(driver)->calib.dig_h4) << 20) -
          (((int32_t)BME_INTERNAL(driver)->calib.dig_h5) * v_x1_u32r)) +
         ((int32_t)16384)) >>
        15) *
       (((((((v_x1_u32r * ((int32_t)BME_INTERNAL(driver)->calib.dig_h6)) >>
             10) *
            (((v_x1_u32r * ((int32_t)BME_INTERNAL(driver)->calib.dig_h3)) >>
              11) +
             ((int32_t)32768))) >>
           10) +
          ((int32_t)2097152)) *
             ((int32_t)BME_INTERNAL(driver)->calib.dig_h2) +
         8192) >>
        14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)BME_INTERNAL(driver)->calib.dig_h1)) >>
                            4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

  return (float)(v_x1_u32r >> 12) / 1024.0f;
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

bool driver_bme280_init_i2c(driver_bme280_t *driver, hw_i2c_t *i2c,
                            uint8_t addr_offset) {
  // Initialize with default settings: 1x oversampling, filter off
  return driver_bme280_init_i2c_with_config(
      driver, i2c, addr_offset, DRIVER_BME280_OVERSAMPLING_1X,
      DRIVER_BME280_OVERSAMPLING_1X, DRIVER_BME280_OVERSAMPLING_1X,
      DRIVER_BME280_FILTER_OFF);
}

bool driver_bme280_init_i2c_with_config(driver_bme280_t *driver, hw_i2c_t *i2c,
                                        uint8_t addr_offset,
                                        driver_bme280_oversampling_t temp_os,
                                        driver_bme280_oversampling_t press_os,
                                        driver_bme280_oversampling_t hum_os,
                                        driver_bme280_filter_t filter) {
  sys_assert(driver);
  sys_assert(i2c);

  // Zero out the structure
  sys_memset(driver, 0, sizeof(driver_bme280_internal_t));

  if (addr_offset > 1) {
    return false;
  }

  // Copy I2C handle and set address
  sys_memcpy(&driver->i2c, i2c, sizeof(hw_i2c_t));
  driver->addr =
      (addr_offset == 0) ? BME280_I2C_ADDR_PRIMARY : BME280_I2C_ADDR_SECONDARY;

  // Verify chip ID
  uint8_t chip_id;
  if (!bme280_read_register(driver, BME280_REG_CHIP_ID, &chip_id)) {
#ifdef DEBUG
    sys_printf("driver_bme280_init: failed to read chip ID from 0x%02X\n",
               driver->addr);
#endif
    sys_memset(driver, 0, sizeof(driver_bme280_internal_t));
    return false;
  }

#ifdef DEBUG
  sys_printf("driver_bme280_init: read chip ID 0x%02X from 0x%02X (expected "
             "0x%02X or 0x%02X)\n",
             chip_id, driver->addr, BME280_CHIP_ID, BMP280_CHIP_ID);
#endif

  if (chip_id != BME280_CHIP_ID && chip_id != BMP280_CHIP_ID) {
#ifdef DEBUG
    sys_printf("driver_bme280_init: unsupported chip ID\n");
#endif
    sys_memset(driver, 0, sizeof(driver_bme280_internal_t));
    return false;
  }

  // Store chip ID for later use
  BME_INTERNAL(driver)->chip_id = chip_id;
  BME_INTERNAL(driver)->temp_offset = 0.0f;

  // Soft reset
  bme280_write_register(driver, BME280_REG_RESET, BME280_SOFT_RESET_CMD);
  sys_sleep(10); // Wait for reset

  // Read calibration data
  if (!bme280_read_calibration(driver)) {
    sys_memset(driver, 0, sizeof(driver_bme280_internal_t));
    return false;
  }

  // Configure humidity oversampling (BME280 only, must be done before
  // ctrl_meas)
  if (BME_INTERNAL(driver)->chip_id == BME280_CHIP_ID) {
    uint8_t ctrl_hum = hum_os & 0x07;
    bme280_write_register(driver, BME280_REG_CTRL_HUM, ctrl_hum);
  }

  // Configure filter
  uint8_t config = (filter & 0x07) << 2;
  bme280_write_register(driver, BME280_REG_CONFIG, config);

  // Configure temperature and pressure oversampling, sleep mode
  uint8_t ctrl_meas = ((temp_os & 0x07) << 5) | ((press_os & 0x07) << 2) | 0x00;
  bme280_write_register(driver, BME280_REG_CTRL_MEAS, ctrl_meas);

  return true;
}

bool driver_bme280_valid(driver_bme280_t *driver) {
  return driver && driver->addr != 0;
}

void driver_bme280_finalize(driver_bme280_t *driver) {
  if (!driver_bme280_valid(driver)) {
    return;
  }

  // Set to sleep mode (already in sleep, but ensure)
  bme280_write_register(driver, BME280_REG_CTRL_MEAS, 0x00);
  sys_memset(driver, 0, sizeof(driver_bme280_internal_t));
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

void driver_bme280_set_temp_offset(driver_bme280_t *driver, float offset) {
  if (!driver_bme280_valid(driver)) {
    return;
  }
  BME_INTERNAL(driver)->temp_offset = offset;
}

///////////////////////////////////////////////////////////////////////////////
// DATA ACQUISITION

bool driver_bme280_read_data(driver_bme280_t *driver,
                             driver_bme280_data_t *data) {
  if (!driver_bme280_valid(driver) || !data) {
    return false;
  }

  // Trigger forced mode measurement (single shot)
  // Read current ctrl_meas to preserve oversampling settings
  uint8_t ctrl_meas;
  if (!bme280_read_register(driver, BME280_REG_CTRL_MEAS, &ctrl_meas)) {
    return false;
  }

  // Set mode to forced (bits 1:0 = 01)
  ctrl_meas = (ctrl_meas & 0xFC) | 0x01;
  if (!bme280_write_register(driver, BME280_REG_CTRL_MEAS, ctrl_meas)) {
    return false;
  }

  // Wait for measurement to complete (check status register)
  // Status bit 3 (measuring) should be 0 when done
  uint8_t status;
  int timeout = 100; // ~100ms timeout
  do {
    sys_sleep(1);
    if (!bme280_read_register(driver, BME280_REG_STATUS, &status)) {
      return false;
    }
  } while ((status & 0x08) && --timeout > 0);

  if (timeout == 0) {
    return false; // Timeout waiting for measurement
  }

  // Read all sensor data (8 bytes: pressure[3], temperature[3], humidity[2])
  uint8_t raw[8];
  if (!bme280_read_registers(driver, BME280_REG_PRESS_MSB, raw, 8)) {
    return false;
  }

  // Parse raw ADC values (20-bit for pressure/temperature, 16-bit for humidity)
  int32_t adc_p =
      ((int32_t)raw[0] << 12) | ((int32_t)raw[1] << 4) | ((int32_t)raw[2] >> 4);
  int32_t adc_t =
      ((int32_t)raw[3] << 12) | ((int32_t)raw[4] << 4) | ((int32_t)raw[5] >> 4);
  int32_t adc_h = ((int32_t)raw[6] << 8) | (int32_t)raw[7];

  // Compensate (temperature first, as it updates t_fine needed for others)
  data->temperature = bme280_compensate_temperature(driver, adc_t) +
                      BME_INTERNAL(driver)->temp_offset;
  data->pressure = bme280_compensate_pressure(driver, adc_p);

  // Humidity only available on BME280, not BMP280
  if (BME_INTERNAL(driver)->chip_id == BME280_CHIP_ID) {
    data->humidity = bme280_compensate_humidity(driver, adc_h);
  } else {
    data->humidity = 0.0f; // BMP280 doesn't have humidity sensor
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS

/**
 * @brief Fast approximation of pow(x, 1/5.255) using log approximation
 */
static float bme280_pow_1_5255(float x) {
  // For x close to 1.0, use Taylor series: x^a ≈ 1 + a*ln(x)
  // ln(x) ≈ (x-1) - (x-1)^2/2 + (x-1)^3/3 for x near 1
  // This is valid for pressure ratios (typically 0.8 to 1.2)

  float ln_x = x - 1.0f;
  float ln_x2 = ln_x * ln_x;
  ln_x = ln_x - ln_x2 * 0.5f + ln_x2 * ln_x * 0.333333f;

  // x^(1/5.255) = exp(ln(x)/5.255)
  float exp_arg = ln_x * 0.1903f; // 1/5.255 ≈ 0.1903

  // exp(y) ≈ 1 + y + y^2/2 + y^3/6 for small y
  float exp_arg2 = exp_arg * exp_arg;
  return 1.0f + exp_arg + exp_arg2 * 0.5f + exp_arg2 * exp_arg * 0.166667f;
}

/**
 * @brief Fast approximation of pow(x, 5.255) using log approximation
 */
static float bme280_pow_5255(float x) {
  // Similar approach but with exponent 5.255
  float ln_x = x - 1.0f;
  float ln_x2 = ln_x * ln_x;
  ln_x = ln_x - ln_x2 * 0.5f + ln_x2 * ln_x * 0.333333f;

  // x^5.255 = exp(ln(x)*5.255)
  float exp_arg = ln_x * 5.255f;

  // exp(y) ≈ 1 + y + y^2/2 + y^3/6 for small y
  float exp_arg2 = exp_arg * exp_arg;
  return 1.0f + exp_arg + exp_arg2 * 0.5f + exp_arg2 * exp_arg * 0.166667f;
}

float driver_bme280_calculate_altitude(const driver_bme280_data_t *data,
                                       float sea_level_pressure) {
  sys_assert(data);

  if (sea_level_pressure == 0.0f) {
    sea_level_pressure = BME280_SEA_LEVEL_PRESSURE;
  }

  // International barometric formula
  // h = 44330 * (1 - (P/P0)^(1/5.255))
  float ratio = data->pressure / sea_level_pressure;
  return 44330.0f * (1.0f - bme280_pow_1_5255(ratio));
}

float driver_bme280_calculate_sea_level_pressure(
    const driver_bme280_data_t *data, float altitude) {
  sys_assert(data);

  // P0 = P / (1 - h/44330)^5.255
  float ratio = 1.0f - (altitude / 44330.0f);
  return data->pressure / bme280_pow_5255(ratio);
}
