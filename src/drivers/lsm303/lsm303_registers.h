/**
 * @file lsm303_registers.h
 * @brief LSM303D Accelerometer/Magnetometer register definitions
 * @ingroup Drivers
 *
 * Register map and constants for the LSM303D 3D accelerometer and magnetometer.
 */
#pragma once
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// I2C ADDRESS

#define LSM303_I2C_ADDR 0x1D ///< Default I2C address

///////////////////////////////////////////////////////////////////////////////
// REGISTER ADDRESSES

#define LSM303_TEMP_OUT_L 0x05 ///< Temperature output low byte
#define LSM303_TEMP_OUT_H 0x06 ///< Temperature output high byte
#define LSM303_STATUS_M 0x07   ///< Magnetometer status
#define LSM303_OUT_X_L_M 0x08  ///< Magnetometer X-axis low byte
#define LSM303_OUT_X_H_M 0x09  ///< Magnetometer X-axis high byte
#define LSM303_OUT_Y_L_M 0x0A  ///< Magnetometer Y-axis low byte
#define LSM303_OUT_Y_H_M 0x0B  ///< Magnetometer Y-axis high byte
#define LSM303_OUT_Z_L_M 0x0C  ///< Magnetometer Z-axis low byte
#define LSM303_OUT_Z_H_M 0x0D  ///< Magnetometer Z-axis high byte
#define LSM303_WHO_AM_I 0x0F   ///< Device identification (0x49)
#define LSM303_CTRL0 0x1F      ///< Control register 0
#define LSM303_CTRL1 0x20      ///< Control register 1
#define LSM303_CTRL2 0x21      ///< Control register 2
#define LSM303_CTRL3 0x22      ///< Control register 3 (INT1)
#define LSM303_CTRL4 0x23      ///< Control register 4 (INT2)
#define LSM303_CTRL5 0x24      ///< Control register 5
#define LSM303_CTRL6 0x25      ///< Control register 6
#define LSM303_CTRL7 0x26      ///< Control register 7
#define LSM303_STATUS_A 0x27   ///< Accelerometer status
#define LSM303_OUT_X_L_A 0x28  ///< Accelerometer X-axis low byte
#define LSM303_OUT_X_H_A 0x29  ///< Accelerometer X-axis high byte
#define LSM303_OUT_Y_L_A 0x2A  ///< Accelerometer Y-axis low byte
#define LSM303_OUT_Y_H_A 0x2B  ///< Accelerometer Y-axis high byte
#define LSM303_OUT_Z_L_A 0x2C  ///< Accelerometer Z-axis low byte
#define LSM303_OUT_Z_H_A 0x2D  ///< Accelerometer Z-axis high byte

///////////////////////////////////////////////////////////////////////////////
// AUTO-INCREMENT BIT

/**
 * @brief Auto-increment bit for multi-byte reads
 *
 * OR this with the register address to enable auto-increment for reading
 * multiple consecutive registers.
 */
#define LSM303_AUTO_INCREMENT 0x80

///////////////////////////////////////////////////////////////////////////////
// DEVICE ID

#define LSM303_WHOAMI_VALUE 0x49 ///< Expected WHO_AM_I register value

///////////////////////////////////////////////////////////////////////////////
// CONTROL REGISTER 1 (CTRL1) BITS

#define LSM303_CTRL1_AXEN 0x01 ///< Accelerometer X-axis enable
#define LSM303_CTRL1_AYEN 0x02 ///< Accelerometer Y-axis enable
#define LSM303_CTRL1_AZEN 0x04 ///< Accelerometer Z-axis enable
#define LSM303_CTRL1_BDU 0x08  ///< Block data update

///////////////////////////////////////////////////////////////////////////////
// CONTROL REGISTER 5 (CTRL5) BITS

#define LSM303_CTRL5_TEMP_EN 0x80 ///< Temperature sensor enable

///////////////////////////////////////////////////////////////////////////////
// CONTROL REGISTER 7 (CTRL7) BITS

#define LSM303_CTRL7_MAG_CONTINUOUS 0x00 ///< Magnetometer continuous mode
#define LSM303_CTRL7_MAG_SINGLE 0x01     ///< Magnetometer single conversion
#define LSM303_CTRL7_MAG_OFF 0x02        ///< Magnetometer power-down

///////////////////////////////////////////////////////////////////////////////
// DATA RATES

/**
 * @brief Accelerometer data rate values for CTRL1[7:4]
 */
typedef enum {
  LSM303_ACCEL_RATE_OFF = 0x00,   ///< Power down
  LSM303_ACCEL_RATE_3HZ = 0x10,   ///< 3.125 Hz
  LSM303_ACCEL_RATE_6HZ = 0x20,   ///< 6.25 Hz
  LSM303_ACCEL_RATE_12HZ = 0x30,  ///< 12.5 Hz
  LSM303_ACCEL_RATE_25HZ = 0x40,  ///< 25 Hz
  LSM303_ACCEL_RATE_50HZ = 0x50,  ///< 50 Hz (default)
  LSM303_ACCEL_RATE_100HZ = 0x60, ///< 100 Hz
  LSM303_ACCEL_RATE_200HZ = 0x70, ///< 200 Hz
  LSM303_ACCEL_RATE_400HZ = 0x80, ///< 400 Hz
  LSM303_ACCEL_RATE_800HZ = 0x90, ///< 800 Hz
  LSM303_ACCEL_RATE_1600HZ = 0xA0 ///< 1600 Hz
} lsm303_accel_rate_t;

/**
 * @brief Magnetometer data rate values for CTRL5[4:2]
 */
typedef enum {
  LSM303_MAG_RATE_3HZ = 0x00,  ///< 3.125 Hz
  LSM303_MAG_RATE_6HZ = 0x04,  ///< 6.25 Hz
  LSM303_MAG_RATE_12HZ = 0x08, ///< 12.5 Hz
  LSM303_MAG_RATE_25HZ = 0x0C, ///< 25 Hz
  LSM303_MAG_RATE_50HZ = 0x10, ///< 50 Hz (default)
  LSM303_MAG_RATE_100HZ = 0x14 ///< 100 Hz
} lsm303_mag_rate_t;

///////////////////////////////////////////////////////////////////////////////
// FULL SCALE RANGES

/**
 * @brief Accelerometer full scale values for CTRL2[5:3]
 */
typedef enum {
  LSM303_ACCEL_SCALE_2G = 0x00, ///< ±2g (default)
  LSM303_ACCEL_SCALE_4G = 0x08, ///< ±4g
  LSM303_ACCEL_SCALE_6G = 0x10, ///< ±6g
  LSM303_ACCEL_SCALE_8G = 0x18, ///< ±8g
  LSM303_ACCEL_SCALE_16G = 0x20 ///< ±16g
} lsm303_accel_scale_t;

/**
 * @brief Magnetometer full scale values for CTRL6[6:5]
 */
typedef enum {
  LSM303_MAG_SCALE_2GAUSS = 0x00, ///< ±2 gauss (default)
  LSM303_MAG_SCALE_4GAUSS = 0x20, ///< ±4 gauss
  LSM303_MAG_SCALE_8GAUSS = 0x40, ///< ±8 gauss
  LSM303_MAG_SCALE_12GAUSS = 0x60 ///< ±12 gauss
} lsm303_mag_scale_t;
