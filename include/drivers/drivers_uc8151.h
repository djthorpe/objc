/**
 * @file drivers_uc8151.h
 * @brief UC8151 e-paper display driver interface
 * @defgroup UC8151 UC8151
 * @ingroup Drivers
 *
 * UC8151 e-paper display driver interface for monochrome displays.
 * The UC8151 is a popular e-paper display controller used in various
 * e-ink displays, supporting partial and full updates with different
 * refresh speeds.
 */
#pragma once
#include <runtime-hw/hw.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief UC8151 update speed modes.
 * @ingroup UC8151
 */
typedef enum {
  DRIVER_UC8151_UPDATE_SPEED_DEFAULT = 0, ///< Default speed (~4.5s)
  DRIVER_UC8151_UPDATE_SPEED_MEDIUM = 1,  ///< Medium speed (~2s)
  DRIVER_UC8151_UPDATE_SPEED_FAST = 2,    ///< Fast speed (~0.8s)
  DRIVER_UC8151_UPDATE_SPEED_TURBO = 3    ///< Turbo speed (~0.25s)
} driver_uc8151_update_speed_t;

/**
 * @brief UC8151 rotation modes.
 * @ingroup UC8151
 */
typedef enum {
  DRIVER_UC8151_ROTATION_0 = 0,  ///< No rotation
  DRIVER_UC8151_ROTATION_180 = 1 ///< 180-degree rotation
} driver_uc8151_rotation_t;

/**
 * @brief UC8151 display rectangle for partial updates.
 * @ingroup UC8151
 */
typedef struct {
  uint16_t x;      ///< X coordinate (pixels)
  uint16_t y;      ///< Y coordinate (pixels)
  uint16_t width;  ///< Width (pixels)
  uint16_t height; ///< Height (pixels)
} driver_uc8151_rect_t;

/**
 * @brief UC8151 display driver structure.
 * @ingroup UC8151
 * @headerfile drivers_uc8151.h drivers/drivers.h
 *
 * This structure contains the configuration and state information for a UC8151
 * e-paper display controller instance. The UC8151 supports various update
 * speeds and partial refresh capabilities.
 */
typedef struct driver_uc8151_t {
  hw_spi_t *spi;                             ///< Pointer to the SPI interface
  hw_gpio_t dc;                              ///< Data/Command pin
  hw_gpio_t reset;                           ///< Reset pin
  hw_gpio_t busy;                            ///< Busy pin
  uint16_t width;                            ///< Display width in pixels
  uint16_t height;                           ///< Display height in pixels
  driver_uc8151_update_speed_t update_speed; ///< Current update speed
  driver_uc8151_rotation_t rotation;         ///< Display rotation
  bool inverted;                             ///< Color inversion flag
  bool blocking;                             ///< Blocking mode flag
} driver_uc8151_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a UC8151 display driver using the SPI interface.
 * @ingroup UC8151
 * @param spi Pointer to an initialized SPI interface for display communication.
 * @param dc_pin GPIO pin number for Data/Command control.
 * @param reset_pin GPIO pin number for display reset.
 * @param busy_pin GPIO pin number for busy status.
 * @param width Display width in pixels.
 * @param height Display height in pixels.
 * @return A UC8151 driver structure representing the initialized display.
 *
 * This function initializes a UC8151 e-paper display driver for communication
 * over SPI. The display requires several control pins and supports various
 * resolutions.
 *
 * @note The spi pointer must be valid and the SPI interface must be properly
 * initialized with appropriate speed and mode.
 * @see hw_spi_init() for SPI interface initialization.
 * @see driver_uc8151_finalize() for cleanup and resource release.
 */
driver_uc8151_t driver_uc8151_init(hw_spi_t *spi, uint8_t dc_pin,
                                   uint8_t reset_pin, uint8_t busy_pin,
                                   uint16_t width, uint16_t height);

/**
 * @brief Finalize and release a UC8151 display driver.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure to finalize.
 *
 * This function releases resources associated with the UC8151 display driver
 * and puts the display into a low-power state.
 */
void driver_uc8151_finalize(driver_uc8151_t *uc8151);

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Check if a UC8151 driver structure is valid.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure to validate.
 * @return True if the driver structure is valid, false otherwise.
 */
static inline bool driver_uc8151_valid(driver_uc8151_t *uc8151) {
  return uc8151 && uc8151->spi && hw_spi_valid(uc8151->spi) &&
         uc8151->width > 0 && uc8151->height > 0;
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

/**
 * @brief Set the update speed of the UC8151 display.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 * @param speed Update speed mode.
 * @return True if the speed was set successfully, false otherwise.
 */
bool driver_uc8151_set_update_speed(driver_uc8151_t *uc8151,
                                    driver_uc8151_update_speed_t speed);

/**
 * @brief Set the rotation of the UC8151 display.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 * @param rotation Rotation mode.
 * @return True if the rotation was set successfully, false otherwise.
 */
bool driver_uc8151_set_rotation(driver_uc8151_t *uc8151,
                                driver_uc8151_rotation_t rotation);

/**
 * @brief Set the color inversion of the UC8151 display.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 * @param inverted True to invert colors, false for normal colors.
 * @return True if the inversion was set successfully, false otherwise.
 */
bool driver_uc8151_set_inverted(driver_uc8151_t *uc8151, bool inverted);

/**
 * @brief Set the blocking mode of the UC8151 display.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 * @param blocking True to enable blocking mode, false for non-blocking.
 * @return True if the blocking mode was set successfully, false otherwise.
 */
bool driver_uc8151_set_blocking(driver_uc8151_t *uc8151, bool blocking);

/**
 * @brief Get the estimated update time for the current speed setting.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 * @return Estimated update time in milliseconds.
 */
uint32_t driver_uc8151_get_update_time(driver_uc8151_t *uc8151);

///////////////////////////////////////////////////////////////////////////////
// DISPLAY OPERATIONS

/**
 * @brief Check if the UC8151 display is busy.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 * @return True if the display is busy, false if ready.
 */
bool driver_uc8151_is_busy(driver_uc8151_t *uc8151);

/**
 * @brief Wait for the UC8151 display to become ready.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 */
void driver_uc8151_busy_wait(driver_uc8151_t *uc8151);

/**
 * @brief Reset the UC8151 display.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 */
void driver_uc8151_reset(driver_uc8151_t *uc8151);

/**
 * @brief Turn off the UC8151 display.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 */
void driver_uc8151_power_off(driver_uc8151_t *uc8151);

/**
 * @brief Update the entire UC8151 display with new framebuffer data.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 * @param data Pointer to the framebuffer data.
 * @param size Size of the framebuffer in bytes.
 * @return True if the update was initiated successfully, false otherwise.
 */
bool driver_uc8151_update(driver_uc8151_t *uc8151, const void *data,
                          size_t size);

/**
 * @brief Update a partial region of the UC8151 display.
 * @ingroup UC8151
 * @param uc8151 Pointer to the UC8151 driver structure.
 * @param data Pointer to the framebuffer data.
 * @param size Size of the framebuffer in bytes.
 * @param rect Region to update.
 * @return True if the partial update was initiated successfully, false
 * otherwise.
 */
bool driver_uc8151_partial_update(driver_uc8151_t *uc8151, const uint8_t *data,
                                  size_t size, driver_uc8151_rect_t rect);
