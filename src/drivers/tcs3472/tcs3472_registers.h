/**
 * @file tcs3472_registers.h
 * @brief TCS3472 Color Light-to-Digital Converter register definitions
 * @ingroup Drivers
 *
 * Register map and constants for the TCS3472 RGB color sensor.
 */
#pragma once
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// REGISTER ADDRESSES

/**
 * @brief TCS3472 register addresses
 * @ingroup TCS3472
 * @note Must OR with TCS3472_COMMAND_BIT (0x80) when accessing via I2C
 */
#define TCS3472_ENABLE            0x00  ///< Enable states and interrupts
#define TCS3472_ATIME             0x01  ///< RGBC time
#define TCS3472_WTIME             0x03  ///< Wait time
#define TCS3472_CONFIG            0x0D  ///< Configuration
#define TCS3472_CONTROL           0x0F  ///< Control register (gain)
#define TCS3472_ID                0x12  ///< Device ID
#define TCS3472_STATUS            0x13  ///< Device status
#define TCS3472_CDATAL            0x14  ///< Clear data low byte
#define TCS3472_CDATAH            0x15  ///< Clear data high byte
#define TCS3472_RDATAL            0x16  ///< Red data low byte
#define TCS3472_RDATAH            0x17  ///< Red data high byte
#define TCS3472_GDATAL            0x18  ///< Green data low byte
#define TCS3472_GDATAH            0x19  ///< Green data high byte
#define TCS3472_BDATAL            0x1A  ///< Blue data low byte
#define TCS3472_BDATAH            0x1B  ///< Blue data high byte

///////////////////////////////////////////////////////////////////////////////
// COMMAND REGISTER

/**
 * @brief Command register bit
 * @ingroup TCS3472
 * 
 * Must be set to 1 when addressing the command register
 */
#define TCS3472_COMMAND_BIT       0x80

///////////////////////////////////////////////////////////////////////////////
// ENABLE REGISTER BITS

/**
 * @brief Enable register bits
 * @ingroup TCS3472
 */
#define TCS3472_ENABLE_WEN        0x08  ///< Wait enable
#define TCS3472_ENABLE_AEN        0x02  ///< RGBC enable
#define TCS3472_ENABLE_PON        0x01  ///< Power on

///////////////////////////////////////////////////////////////////////////////
// STATUS REGISTER BITS

/**
 * @brief Status register bits
 * @ingroup TCS3472
 */
#define TCS3472_STATUS_AVALID     0x01  ///< RGBC valid

///////////////////////////////////////////////////////////////////////////////
// CONTROL REGISTER (GAIN)

/**
 * @brief Control register gain values
 * @ingroup TCS3472
 */
#define TCS3472_GAIN_1X           0x00  ///< 1x gain
#define TCS3472_GAIN_4X           0x01  ///< 4x gain
#define TCS3472_GAIN_16X          0x02  ///< 16x gain
#define TCS3472_GAIN_60X          0x03  ///< 60x gain

///////////////////////////////////////////////////////////////////////////////
// I2C ADDRESS

/**
 * @brief TCS3472 I2C address
 * @ingroup TCS3472
 */
#define TCS3472_I2C_ADDR          0x29

///////////////////////////////////////////////////////////////////////////////
// DEVICE ID

/**
 * @brief Expected device ID values
 * @ingroup TCS3472
 */
#define TCS3472_ID_VALUE          0x44  ///< TCS34721/TCS34725
#define TCS34727_ID_VALUE         0x4D  ///< TCS34723/TCS34727

///////////////////////////////////////////////////////////////////////////////
// INTEGRATION TIME VALUES

/**
 * @brief Common ATIME (integration time) values
 * @ingroup TCS3472
 * @note Integration time = (256 - ATIME) * 2.4ms
 */
#define TCS3472_ATIME_2_4MS       0xFF  ///< 2.4ms - 1 cycle
#define TCS3472_ATIME_24MS        0xF6  ///< 24ms - 10 cycles
#define TCS3472_ATIME_50MS        0xEB  ///< 50ms - 20 cycles
#define TCS3472_ATIME_101MS       0xD5  ///< 101ms - 42 cycles
#define TCS3472_ATIME_154MS       0xC0  ///< 154ms - 64 cycles
#define TCS3472_ATIME_700MS       0x00  ///< 700ms - 256 cycles (max)

///////////////////////////////////////////////////////////////////////////////
// CONFIG REGISTER BITS

/**
 * @brief Config register bits
 * @ingroup TCS3472
 */
#define TCS3472_CONFIG_WLONG      0x02  ///< Wait long - 12x wait time
