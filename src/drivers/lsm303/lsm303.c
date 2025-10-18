/**
 * @file lsm303.c
 * @brief LSM303D Accelerometer/Magnetometer Driver implementation
 * @ingroup LSM303
 */
#include <drivers/drivers_lsm303.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

#include "lsm303_registers.h"

///////////////////////////////////////////////////////////////////////////////
// CONSTANTS

// Default settings
#define LSM303_DEFAULT_ACCEL_SCALE DRIVER_LSM303_ACCEL_SCALE_2G
#define LSM303_DEFAULT_MAG_SCALE DRIVER_LSM303_MAG_SCALE_2GAUSS

// Scale factors for converting raw values to physical units
static const float ACCEL_SCALE_FACTORS[] = {
    2.0f / 32768.0f, // ±2g
    4.0f / 32768.0f, // ±4g
    6.0f / 32768.0f, // ±6g
    8.0f / 32768.0f, // ±8g
    16.0f / 32768.0f // ±16g
};

static const float MAG_SCALE_FACTORS[] = {
    2.0f / 32768.0f, // ±2 gauss
    4.0f / 32768.0f, // ±4 gauss
    8.0f / 32768.0f, // ±8 gauss
    12.0f / 32768.0f // ±12 gauss
};

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * @brief Write a single byte to an LSM303 register
 */
static bool lsm303_write_register(driver_lsm303_t *driver, uint8_t reg,
                                  uint8_t value) {
  uint8_t buffer[2];
  buffer[0] = reg;
  buffer[1] = value;
  return hw_i2c_xfr(&driver->i2c, LSM303_I2C_ADDR, buffer, 2, 0, 0) == 2;
}

/**
 * @brief Read a single byte from an LSM303 register
 */
static bool lsm303_read_register(driver_lsm303_t *driver, uint8_t reg,
                                 uint8_t *value) {
  uint8_t buffer[2];
  buffer[0] = reg;
  if (hw_i2c_xfr(&driver->i2c, LSM303_I2C_ADDR, buffer, 1, 1, 0) == 2) {
    *value = buffer[1];
    return true;
  }
  return false;
}

/**
 * @brief Read multiple bytes from LSM303 registers with auto-increment
 */
static bool lsm303_read_registers(driver_lsm303_t *driver, uint8_t reg,
                                  uint8_t *buffer, uint8_t len) {
  // Use auto-increment bit for multi-byte reads
  uint8_t temp_buffer[7]; // Max 6 data bytes + 1 register byte
  temp_buffer[0] = reg | LSM303_AUTO_INCREMENT;
  if (hw_i2c_xfr(&driver->i2c, LSM303_I2C_ADDR, temp_buffer, 1, len, 0) ==
      (size_t)(1 + len)) {
    sys_memcpy(buffer, &temp_buffer[1], len);
    return true;
  }
  return false;
}

/**
 * @brief Verify device ID
 */
static bool lsm303_verify_id(driver_lsm303_t *driver) {
  uint8_t id;
  if (!lsm303_read_register(driver, LSM303_WHO_AM_I, &id)) {
    return false;
  }

  return (id == LSM303_WHOAMI_VALUE);
}

/**
 * @brief Convert raw 16-bit value to signed integer (little-endian)
 */
static int16_t lsm303_bytes_to_int16(uint8_t low, uint8_t high) {
  return (int16_t)((high << 8) | low);
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

bool driver_lsm303_init(driver_lsm303_t *driver, hw_i2c_t *i2c) {
  return driver_lsm303_init_with_config(driver, i2c, LSM303_DEFAULT_ACCEL_SCALE,
                                        LSM303_DEFAULT_MAG_SCALE);
}

bool driver_lsm303_init_with_config(driver_lsm303_t *driver, hw_i2c_t *i2c,
                                    driver_lsm303_accel_scale_t accel_scale,
                                    driver_lsm303_mag_scale_t mag_scale) {
  sys_assert(driver);
  sys_assert(i2c);

  // Zero out the structure
  sys_memset(driver, 0, sizeof(driver_lsm303_t));

  // Copy I2C handle
  sys_memcpy(&driver->i2c, i2c, sizeof(hw_i2c_t));

  // Store scale settings
  driver->accel_scale = accel_scale;
  driver->mag_scale = mag_scale;

  // Verify device ID
  if (!lsm303_verify_id(driver)) {
    return false;
  }

  // Configure CTRL1: Enable all accelerometer axes, 50Hz data rate
  uint8_t ctrl1 = LSM303_ACCEL_RATE_50HZ | LSM303_CTRL1_AXEN |
                  LSM303_CTRL1_AYEN | LSM303_CTRL1_AZEN;
  if (!lsm303_write_register(driver, LSM303_CTRL1, ctrl1)) {
    return false;
  }

  // Configure CTRL2: Set accelerometer full scale
  uint8_t ctrl2_scale;
  switch (accel_scale) {
  case DRIVER_LSM303_ACCEL_SCALE_2G:
    ctrl2_scale = LSM303_ACCEL_SCALE_2G;
    break;
  case DRIVER_LSM303_ACCEL_SCALE_4G:
    ctrl2_scale = LSM303_ACCEL_SCALE_4G;
    break;
  case DRIVER_LSM303_ACCEL_SCALE_6G:
    ctrl2_scale = LSM303_ACCEL_SCALE_6G;
    break;
  case DRIVER_LSM303_ACCEL_SCALE_8G:
    ctrl2_scale = LSM303_ACCEL_SCALE_8G;
    break;
  case DRIVER_LSM303_ACCEL_SCALE_16G:
    ctrl2_scale = LSM303_ACCEL_SCALE_16G;
    break;
  default:
    ctrl2_scale = LSM303_ACCEL_SCALE_2G;
    break;
  }
  if (!lsm303_write_register(driver, LSM303_CTRL2, ctrl2_scale)) {
    return false;
  }

  // Configure CTRL5: Enable temperature sensor, 50Hz mag data rate
  uint8_t ctrl5 = LSM303_CTRL5_TEMP_EN | LSM303_MAG_RATE_50HZ;
  if (!lsm303_write_register(driver, LSM303_CTRL5, ctrl5)) {
    return false;
  }

  // Configure CTRL6: Set magnetometer full scale
  uint8_t ctrl6_scale;
  switch (mag_scale) {
  case DRIVER_LSM303_MAG_SCALE_2GAUSS:
    ctrl6_scale = LSM303_MAG_SCALE_2GAUSS;
    break;
  case DRIVER_LSM303_MAG_SCALE_4GAUSS:
    ctrl6_scale = LSM303_MAG_SCALE_4GAUSS;
    break;
  case DRIVER_LSM303_MAG_SCALE_8GAUSS:
    ctrl6_scale = LSM303_MAG_SCALE_8GAUSS;
    break;
  case DRIVER_LSM303_MAG_SCALE_12GAUSS:
    ctrl6_scale = LSM303_MAG_SCALE_12GAUSS;
    break;
  default:
    ctrl6_scale = LSM303_MAG_SCALE_2GAUSS;
    break;
  }
  if (!lsm303_write_register(driver, LSM303_CTRL6, ctrl6_scale)) {
    return false;
  }

  // Configure CTRL7: Magnetometer continuous conversion mode
  if (!lsm303_write_register(driver, LSM303_CTRL7,
                             LSM303_CTRL7_MAG_CONTINUOUS)) {
    return false;
  }

  // Small delay to let sensor stabilize
  sys_sleep(10);

  return true;
}

void driver_lsm303_finalize(driver_lsm303_t *driver) {
  sys_assert(driver);

  // Power down accelerometer and magnetometer
  lsm303_write_register(driver, LSM303_CTRL1, 0x00);
  lsm303_write_register(driver, LSM303_CTRL7, LSM303_CTRL7_MAG_OFF);

  // Zero out the structure
  sys_memset(driver, 0, sizeof(driver_lsm303_t));
}

///////////////////////////////////////////////////////////////////////////////
// DATA ACQUISITION

bool driver_lsm303_read_accel(driver_lsm303_t *driver,
                              driver_lsm303_vec3_t *data) {
  sys_assert(driver);
  sys_assert(data);

  // Read 6 bytes (X, Y, Z - 2 bytes each) starting from OUT_X_L_A
  uint8_t buffer[6];
  if (!lsm303_read_registers(driver, LSM303_OUT_X_L_A, buffer, 6)) {
    return false;
  }

  // Convert to signed 16-bit values (little-endian)
  int16_t x_raw = lsm303_bytes_to_int16(buffer[0], buffer[1]);
  int16_t y_raw = lsm303_bytes_to_int16(buffer[2], buffer[3]);
  int16_t z_raw = lsm303_bytes_to_int16(buffer[4], buffer[5]);

  // Convert to g using the appropriate scale factor
  float scale = ACCEL_SCALE_FACTORS[driver->accel_scale];
  data->x = x_raw * scale;
  data->y = y_raw * scale;
  data->z = z_raw * scale;

  return true;
}

bool driver_lsm303_read_mag(driver_lsm303_t *driver,
                            driver_lsm303_vec3_t *data) {
  sys_assert(driver);
  sys_assert(data);

  // Read 6 bytes (X, Y, Z - 2 bytes each) starting from OUT_X_L_M
  uint8_t buffer[6];
  if (!lsm303_read_registers(driver, LSM303_OUT_X_L_M, buffer, 6)) {
    return false;
  }

  // Convert to signed 16-bit values (little-endian)
  int16_t x_raw = lsm303_bytes_to_int16(buffer[0], buffer[1]);
  int16_t y_raw = lsm303_bytes_to_int16(buffer[2], buffer[3]);
  int16_t z_raw = lsm303_bytes_to_int16(buffer[4], buffer[5]);

  // Convert to gauss using the appropriate scale factor
  float scale = MAG_SCALE_FACTORS[driver->mag_scale];
  data->x = x_raw * scale;
  data->y = y_raw * scale;
  data->z = z_raw * scale;

  return true;
}

bool driver_lsm303_read_temp(driver_lsm303_t *driver, float *temp) {
  sys_assert(driver);
  sys_assert(temp);

  // Read 2 bytes (temperature - 12-bit value in 16-bit register)
  uint8_t buffer[2];
  if (!lsm303_read_registers(driver, LSM303_TEMP_OUT_L, buffer, 2)) {
    return false;
  }

  // Combine bytes into 16-bit value (little-endian: low byte, high byte)
  int16_t temp_raw = (int16_t)((buffer[1] << 8) | buffer[0]);

  // Temperature is 12-bit left-justified in 16-bit register
  // Shift right by 4 to get the 12-bit signed value
  temp_raw = temp_raw >> 4;

  // LSM303D formula: add offset of 20°C (20 * 8 LSB/°C = 160 LSB)
  // Then divide by 8 to convert from LSB to °C
  *temp = (temp_raw + 160) / 8.0f;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS

/**
 * @brief Fast approximation of atan2 without using math library
 *
 * This implementation uses a polynomial approximation and is suitable
 * for embedded systems without floating-point math library support.
 * Accuracy is sufficient for compass heading calculations.
 */
static float fast_atan2(float y, float x) {
  // Handle special cases
  if (x == 0.0f && y == 0.0f) {
    return 0.0f;
  }

  const float pi = 3.14159265f;
  const float pi_2 = 1.57079633f;

  float abs_y = (y < 0) ? -y : y;
  float angle;

  if (x >= 0.0f) {
    float r = (x - abs_y) / (x + abs_y);
    angle = pi_2 - pi_2 * r;
  } else {
    float r = (x + abs_y) / (abs_y - x);
    angle = pi - pi_2 * r;
  }

  return (y < 0.0f) ? -angle : angle;
}

float driver_lsm303_get_heading(const driver_lsm303_vec3_t *mag) {
  if (!mag) {
    return -1.0f;
  }

  // Calculate heading from X and Y magnetometer components
  // Returns angle in radians from -π to π
  float heading = fast_atan2(mag->y, mag->x);

  // Convert to degrees
  heading = heading * 57.29578f; // 180/π

  // Normalize to 0-360 range
  if (heading < 0.0f) {
    heading += 360.0f;
  }

  return heading;
}
