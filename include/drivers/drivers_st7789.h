/**
 * @file drivers_st7789.h
 * @brief ST7789 TFT LCD display driver interface
 * @defgroup ST7789 ST7789
 * @ingroup Drivers
 *
 * ST7789 TFT LCD display driver interface for color displays.
 * The ST7789 is a popular TFT LCD controller supporting 16-bit color
 * with SPI and parallel interfaces.
 */
#pragma once
#include <runtime-hw/hw.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief ST7789 rotation modes.
 * @ingroup ST7789
 */
typedef enum {
  DRIVER_ST7789_ROTATION_0 = 0,   ///< No rotation
  DRIVER_ST7789_ROTATION_90 = 1,  ///< 90-degree rotation
  DRIVER_ST7789_ROTATION_180 = 2, ///< 180-degree rotation
  DRIVER_ST7789_ROTATION_270 = 3  ///< 270-degree rotation
} driver_st7789_rotation_t;

/**
 * @brief ST7789 interface type.
 * @ingroup ST7789
 */
typedef enum {
  DRIVER_ST7789_INTERFACE_SPI = 0,     ///< SPI interface
  DRIVER_ST7789_INTERFACE_PARALLEL = 1 ///< Parallel interface
} driver_st7789_interface_t;

/**
 * @brief ST7789 display rectangle for window operations.
 * @ingroup ST7789
 */
typedef struct {
  uint16_t x;      ///< X coordinate (pixels)
  uint16_t y;      ///< Y coordinate (pixels)
  uint16_t width;  ///< Width (pixels)
  uint16_t height; ///< Height (pixels)
} driver_st7789_rect_t;

/**
 * @brief ST7789 display driver structure.
 * @ingroup ST7789
 * @headerfile drivers_st7789.h drivers/drivers.h
 *
 * This structure contains the configuration and state information for a ST7789
 * TFT LCD display controller instance. The ST7789 supports both SPI and
 * parallel interfaces with 16-bit color depth.
 */
typedef struct driver_st7789_t {
  hw_spi_t *spi;       ///< Pointer to the SPI interface (NULL for parallel)
  hw_gpio_t dc;        ///< Data/Command pin
  hw_gpio_t reset;     ///< Reset pin (optional, can be invalid)
  hw_gpio_t backlight; ///< Backlight pin (optional, can be invalid)
  uint16_t width;      ///< Display width in pixels
  uint16_t height;     ///< Display height in pixels
  driver_st7789_rotation_t rotation;   ///< Display rotation
  driver_st7789_interface_t interface; ///< Interface type (SPI/Parallel)
  bool round;                          ///< Round display flag
  uint8_t backlight_brightness;        ///< Current backlight brightness (0-255)
} driver_st7789_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a ST7789 display driver using the SPI interface.
 * @ingroup ST7789
 * @param spi Pointer to an initialized SPI interface for display communication.
 * @param dc_pin GPIO pin number for Data/Command control.
 * @param reset_pin GPIO pin number for display reset (optional, use
 * HW_GPIO_INVALID if not used).
 * @param backlight_pin GPIO pin number for backlight control (optional, use
 * HW_GPIO_INVALID if not used).
 * @param width Display width in pixels.
 * @param height Display height in pixels.
 * @param round Whether the display is round (affects drawing operations).
 * @return A ST7789 driver structure representing the initialized display.
 *
 * This function initializes a ST7789 TFT LCD display driver for communication
 * over SPI. The display requires DC and CS control pins, with optional reset
 * and backlight control.
 *
 * @note The spi pointer must be valid and the SPI interface must be properly
 * initialized with appropriate speed and mode for ST7789, including CS pin.
 * @see hw_spi_init() for SPI interface initialization.
 * @see driver_st7789_finalize() for cleanup and resource release.
 */
driver_st7789_t driver_st7789_init(hw_spi_t *spi, uint8_t dc_pin,
                                   uint8_t reset_pin, uint8_t backlight_pin,
                                   uint16_t width, uint16_t height, bool round);

/**
 * @brief Finalize and release a ST7789 display driver.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure to finalize.
 *
 * This function properly shuts down the ST7789 display driver, turning off
 * the backlight, releasing GPIO resources, and clearing the driver structure.
 * After calling this function, the driver structure should not be used.
 *
 * @note This function is safe to call multiple times on the same driver.
 * @see driver_st7789_init() for initialization.
 */
void driver_st7789_finalize(driver_st7789_t *st7789);

/**
 * @brief Check if a ST7789 driver structure is valid.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure to validate.
 * @return True if the driver structure is valid, false otherwise.
 *
 * This function performs basic validation of the ST7789 driver structure
 * to ensure it has been properly initialized and contains valid configuration.
 */
bool driver_st7789_valid(const driver_st7789_t *st7789);

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

/**
 * @brief Set the rotation of the ST7789 display.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure.
 * @param rotation The rotation mode to set.
 * @return True if successful, false otherwise.
 *
 * This function configures the display rotation by setting the appropriate
 * MADCTL register values. The rotation affects how pixel data is interpreted
 * and displayed.
 */
bool driver_st7789_set_rotation(driver_st7789_t *st7789,
                                driver_st7789_rotation_t rotation);

/**
 * @brief Set the backlight brightness of the ST7789 display.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure.
 * @param brightness Brightness level (0-255, where 0 is off and 255 is full
 * brightness).
 * @return True if successful, false if backlight control is not available.
 *
 * This function controls the display backlight using PWM if a backlight pin
 * was configured during initialization.
 */
bool driver_st7789_set_backlight(driver_st7789_t *st7789, uint8_t brightness);

///////////////////////////////////////////////////////////////////////////////
// DISPLAY OPERATIONS

/**
 * @brief Reset the ST7789 display.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure.
 *
 * This function performs a hardware reset of the display if a reset pin
 * is configured, or issues a software reset command.
 */
void driver_st7789_reset(driver_st7789_t *st7789);

/**
 * @brief Set the display window for subsequent pixel data.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure.
 * @param rect Rectangle defining the window area.
 * @return True if successful, false otherwise.
 *
 * This function configures the display controller's column and row address
 * windows for subsequent pixel data writes. All pixel data sent after this
 * command will be written to the specified rectangular area.
 */
bool driver_st7789_set_window(driver_st7789_t *st7789,
                              driver_st7789_rect_t rect);

/**
 * @brief Send pixel data to the ST7789 display.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure.
 * @param data Pointer to the pixel data buffer.
 * @param size Size of the pixel data in bytes.
 * @return True if successful, false otherwise.
 *
 * This function sends raw pixel data to the display. The data should be
 * 16-bit RGB565 format pixels. The display window should be set before
 * calling this function using driver_st7789_set_window().
 */
bool driver_st7789_write_pixels(driver_st7789_t *st7789, const uint8_t *data,
                                size_t size);

/**
 * @brief Fill the entire display with a single color.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure.
 * @param color RGB565 color value to fill the display with.
 * @return True if successful, false otherwise.
 *
 * This function efficiently fills the entire display with a single color
 * by setting the full display window and writing the color data.
 */
bool driver_st7789_fill(driver_st7789_t *st7789, uint16_t color);

/**
 * @brief Update a rectangular region of the ST7789 display.
 * @ingroup ST7789
 * @param st7789 Pointer to the ST7789 driver structure.
 * @param rect Rectangle defining the update area.
 * @param data Pointer to the pixel data buffer.
 * @param size Size of the pixel data in bytes.
 * @return True if successful, false otherwise.
 *
 * This function updates a specific rectangular region of the display with
 * new pixel data. It combines setting the window and writing pixels in
 * a single operation.
 */
bool driver_st7789_update_rect(driver_st7789_t *st7789,
                               driver_st7789_rect_t rect, const uint8_t *data,
                               size_t size);
