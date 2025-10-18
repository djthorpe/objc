/**
 * @file ssd1677.h
 * @brief SSD1677 E-Paper Display Controller Driver
 * @defgroup SSD1677 SSD1677 E-Paper Display
 * @ingroup Drivers
 *
 * Driver for the SSD1677 e-paper display controller.
 * Supports monochrome displays with SPI interface, such as these:
 *
 * 7.5inch HD e-Paper HAT
 * (https://www.waveshare.com/wiki/7.5inch_HD_e-Paper_HAT) - 880x528, monochrome
 */
#pragma once
#include <runtime-hw/hw.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief SSD1677 display configuration (opaque)
 * @ingroup SSD1677
 */
typedef struct {
  uint8_t reserved[256]; ///< Reserved space for internal implementation
} driver_ssd1677_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize SSD1677 display controller
 * @ingroup SSD1677
 * @param display Pointer to display structure
 * @param spi Pointer to initialized SPI interface
 * @param width Display width in pixels
 * @param height Display height in pixels
 * @param dc_pin Data/Command GPIO pin
 * @param reset_pin Reset GPIO pin
 * @param busy_pin Busy GPIO pin
 * @return true if initialization successful, false otherwise
 */
bool driver_ssd1677_init(driver_ssd1677_t *display, hw_spi_t *spi,
                         uint16_t width, uint16_t height, uint8_t dc_pin,
                         uint8_t reset_pin, uint8_t busy_pin);

/**
 * @brief Finalize and release SSD1677 display
 * @ingroup SSD1677
 * @param display Pointer to display structure
 */
void driver_ssd1677_finalize(driver_ssd1677_t *display);

///////////////////////////////////////////////////////////////////////////////
// CONTROL

/**
 * @brief Put display into sleep mode
 * @ingroup SSD1677
 * @param display Pointer to display structure
 */
void driver_ssd1677_sleep(driver_ssd1677_t *display);

///////////////////////////////////////////////////////////////////////////////
// DISPLAY OPERATIONS

/**
 * @brief Clear the display (always does full refresh)
 * @ingroup SSD1677
 * @param display Pointer to display structure
 */
void driver_ssd1677_clear(driver_ssd1677_t *display);

/**
 * @brief Display a full frame buffer
 * @ingroup SSD1677
 * @param display Pointer to display structure
 * @param image_data Pointer to image data buffer
 * @param is_base_image true if this should be stored as base image for partial
 * updates
 */
void driver_ssd1677_update(driver_ssd1677_t *display, const uint8_t *image_data,
                           bool is_base_image);

/**
 * @brief Display a partial frame at specified coordinates
 * @ingroup SSD1677
 * @param display Pointer to display structure
 * @param image_data Pointer to image data buffer
 * @param x_start Starting X coordinate (must be multiple of 8)
 * @param y_start Starting Y coordinate
 * @param x_end Ending X coordinate
 * @param y_end Ending Y coordinate
 * @param use_gc_mode true to use grayscale conversion mode
 */
void driver_ssd1677_update_partial(driver_ssd1677_t *display,
                                   const uint8_t *image_data, uint16_t x_start,
                                   uint16_t y_start, uint16_t x_end,
                                   uint16_t y_end, bool use_gc_mode);

///////////////////////////////////////////////////////////////////////////////
// UTILITY

/**
 * @brief Check if display is valid and initialized
 * @ingroup SSD1677
 * @param display Pointer to display structure
 * @return true if display is valid, false otherwise
 */
bool driver_ssd1677_valid(driver_ssd1677_t *display);

#ifdef __cplusplus
}
#endif