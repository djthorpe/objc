#include <drivers/drivers.h>
#include <math.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// REGISTER DEFINITIONS

// ST7789 Register addresses
#define ST7789_SWRESET 0x01  // Software Reset
#define ST7789_TEOFF 0x34    // Tearing Effect Off
#define ST7789_TEON 0x35     // Tearing Effect On
#define ST7789_MADCTL 0x36   // Memory Access Control
#define ST7789_COLMOD 0x3A   // Interface Pixel Format
#define ST7789_RAMCTRL 0xB0  // RAM Control
#define ST7789_GCTRL 0xB7    // Gate Control
#define ST7789_VCOMS 0xBB    // VCOM Setting
#define ST7789_LCMCTRL 0xC0  // LCM Control
#define ST7789_VDVVRHEN 0xC2 // VDV and VRH Command Enable
#define ST7789_VRHS 0xC3     // VRH Set
#define ST7789_VDVS 0xC4     // VDV Set
#define ST7789_FRCTRL2 0xC6  // Frame Rate Control 2
#define ST7789_PWCTRL1 0xD0  // Power Control 1
#define ST7789_PORCTRL 0xB2  // Porch Control
#define ST7789_GMCTRP1 0xE0  // Positive Voltage Gamma Control
#define ST7789_GMCTRN1 0xE1  // Negative Voltage Gamma Control
#define ST7789_INVOFF 0x20   // Display Inversion Off
#define ST7789_SLPOUT 0x11   // Sleep Out
#define ST7789_DISPON 0x29   // Display On
#define ST7789_GAMSET 0x26   // Gamma Set
#define ST7789_DISPOFF 0x28  // Display Off
#define ST7789_RAMWR 0x2C    // Memory Write
#define ST7789_INVON 0x21    // Display Inversion On
#define ST7789_CASET 0x2A    // Column Address Set
#define ST7789_RASET 0x2B    // Row Address Set
#define ST7789_PWMFRSEL 0xCC // PWM Frequency Selection

// MADCTL bits
#define ST7789_MADCTL_ROW_ORDER 0x80   // Row Address Order
#define ST7789_MADCTL_COL_ORDER 0x40   // Column Address Order
#define ST7789_MADCTL_SWAP_XY 0x20     // Row/Column Exchange
#define ST7789_MADCTL_SCAN_ORDER 0x10  // Vertical Refresh Order
#define ST7789_MADCTL_RGB_BGR 0x08     // RGB-BGR Order
#define ST7789_MADCTL_HORIZ_ORDER 0x04 // Horizontal Refresh Order

// SPI frequency for ST7789 (62.5 MHz max, using 60 MHz for safety)
#define ST7789_SPI_FREQ 60000000

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * @brief Send a command to the ST7789 with optional data.
 */
static bool _driver_st7789_send_command(driver_st7789_t *st7789, uint8_t cmd,
                                        const uint8_t *data, size_t data_len) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  // Set DC low for command
  hw_gpio_set(&st7789->dc, false);

  // Send command byte
  size_t bytes_sent = hw_spi_xfr(st7789->spi, &cmd, 1, 0);
  if (bytes_sent != 1) {
    return false;
  }

  // Send data if provided
  if (data && data_len > 0) {
    // Set DC high for data
    hw_gpio_set(&st7789->dc, true);
    bytes_sent = hw_spi_xfr(st7789->spi, (void *)data, data_len, 0);
    if (bytes_sent != data_len) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Configure display rotation and window settings.
 */
static bool _driver_st7789_configure_display(driver_st7789_t *st7789) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  uint8_t madctl = 0;
  uint16_t col_start = 0, row_start = 0;
  uint16_t width = st7789->width;
  uint16_t height = st7789->height;

  // Configure based on display size and rotation
  if (width == 240 && height == 240) {
    // 240x240 displays
    row_start = st7789->round ? 40 : 80;
    col_start = 0;

    switch (st7789->rotation) {
    case DRIVER_ST7789_ROTATION_90:
      if (!st7789->round)
        row_start = 0;
      madctl = ST7789_MADCTL_HORIZ_ORDER | ST7789_MADCTL_COL_ORDER |
               ST7789_MADCTL_SWAP_XY;
      break;
    case DRIVER_ST7789_ROTATION_180:
      madctl = ST7789_MADCTL_HORIZ_ORDER | ST7789_MADCTL_COL_ORDER |
               ST7789_MADCTL_ROW_ORDER;
      break;
    case DRIVER_ST7789_ROTATION_270:
      madctl = ST7789_MADCTL_ROW_ORDER | ST7789_MADCTL_SWAP_XY;
      break;
    default: // DRIVER_ST7789_ROTATION_0
      if (!st7789->round)
        row_start = 0;
      madctl = ST7789_MADCTL_HORIZ_ORDER;
      break;
    }
  } else if (width == 240 && height == 135) {
    // Pico Display (240x135) - use working offsets from C++ implementation
    col_start = 40; // Column offset for 240x135
    row_start = 52; // Row offset for 240x135
    // Special case for ROTATE_0 - add 1 to both start AND end
    // (this will be handled in the raset calculation below)
    madctl = (st7789->rotation == DRIVER_ST7789_ROTATION_180)
                 ? ST7789_MADCTL_ROW_ORDER
                 : ST7789_MADCTL_COL_ORDER;
    madctl |= ST7789_MADCTL_SWAP_XY | ST7789_MADCTL_SCAN_ORDER;
  } else if (width == 320 && height == 240) {
    // 320x240 displays
    col_start = 0;
    row_start = 0;
    switch (st7789->rotation) {
    case DRIVER_ST7789_ROTATION_90:
      madctl = ST7789_MADCTL_SWAP_XY | ST7789_MADCTL_HORIZ_ORDER;
      break;
    case DRIVER_ST7789_ROTATION_180:
      madctl = ST7789_MADCTL_ROW_ORDER | ST7789_MADCTL_COL_ORDER;
      break;
    case DRIVER_ST7789_ROTATION_270:
      madctl = ST7789_MADCTL_SWAP_XY | ST7789_MADCTL_ROW_ORDER |
               ST7789_MADCTL_COL_ORDER;
      break;
    default: // DRIVER_ST7789_ROTATION_0
      madctl = 0;
      break;
    }
  }

  // Set MADCTL register
  if (!_driver_st7789_send_command(st7789, ST7789_MADCTL, &madctl, 1)) {
    return false;
  }

  // Set column address - send in big-endian format (MSB first)
  uint8_t caset_data[4];
  caset_data[0] = (col_start >> 8) & 0xFF;               // MSB of start
  caset_data[1] = col_start & 0xFF;                      // LSB of start
  caset_data[2] = ((col_start + width - 1) >> 8) & 0xFF; // MSB of end
  caset_data[3] = (col_start + width - 1) & 0xFF;        // LSB of end
  if (!_driver_st7789_send_command(st7789, ST7789_CASET, caset_data, 4)) {
    return false;
  }

  // Set row address - send in big-endian format (MSB first)
  uint16_t row_start_final = row_start;
  uint16_t row_end_final = row_start + height - 1;

  // Special case for 240x135 display with ROTATE_0 - add 1 to both start and
  // end
  if (width == 240 && height == 135 &&
      st7789->rotation == DRIVER_ST7789_ROTATION_0) {
    row_start_final += 1;
    row_end_final += 1;
  }

  uint8_t raset_data[4];
  raset_data[0] = (row_start_final >> 8) & 0xFF; // MSB of start
  raset_data[1] = row_start_final & 0xFF;        // LSB of start
  raset_data[2] = (row_end_final >> 8) & 0xFF;   // MSB of end
  raset_data[3] = row_end_final & 0xFF;          // LSB of end
  if (!_driver_st7789_send_command(st7789, ST7789_RASET, raset_data, 4)) {
    return false;
  }

  return true;
}

/**
 * @brief Initialize the ST7789 display with appropriate commands.
 */
static bool _driver_st7789_init_display(driver_st7789_t *st7789) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  // Reset display
  driver_st7789_reset(st7789);

  // Software reset
  if (!_driver_st7789_send_command(st7789, ST7789_SWRESET, NULL, 0)) {
    return false;
  }
  sys_sleep(150);

  // Common initialization
  if (!_driver_st7789_send_command(st7789, ST7789_TEON, NULL, 0)) {
    return false;
  }

  const uint8_t colmod_data = 0x05; // 16 bits per pixel
  if (!_driver_st7789_send_command(st7789, ST7789_COLMOD, &colmod_data, 1)) {
    return false;
  }

  const uint8_t porctrl_data[] = {0x0c, 0x0c, 0x00, 0x33, 0x33};
  if (!_driver_st7789_send_command(st7789, ST7789_PORCTRL, porctrl_data, 5)) {
    return false;
  }

  const uint8_t lcmctrl_data = 0x2c;
  if (!_driver_st7789_send_command(st7789, ST7789_LCMCTRL, &lcmctrl_data, 1)) {
    return false;
  }

  const uint8_t vdvvrhen_data = 0x01;
  if (!_driver_st7789_send_command(st7789, ST7789_VDVVRHEN, &vdvvrhen_data,
                                   1)) {
    return false;
  }

  const uint8_t vrhs_data = 0x12;
  if (!_driver_st7789_send_command(st7789, ST7789_VRHS, &vrhs_data, 1)) {
    return false;
  }

  const uint8_t vdvs_data = 0x20;
  if (!_driver_st7789_send_command(st7789, ST7789_VDVS, &vdvs_data, 1)) {
    return false;
  }

  const uint8_t pwctrl1_data[] = {0xa4, 0xa1};
  if (!_driver_st7789_send_command(st7789, ST7789_PWCTRL1, pwctrl1_data, 2)) {
    return false;
  }

  const uint8_t frctrl2_data = 0x0f;
  if (!_driver_st7789_send_command(st7789, ST7789_FRCTRL2, &frctrl2_data, 1)) {
    return false;
  }

  // Fix for light grey banding issue
  const uint8_t ramctrl_data[] = {0x00, 0xc0};
  if (!_driver_st7789_send_command(st7789, ST7789_RAMCTRL, ramctrl_data, 2)) {
    return false;
  }

  // Display-specific gamma and voltage settings
  if (st7789->width == 240 && st7789->height == 240) {
    const uint8_t gctrl_data = 0x14;
    if (!_driver_st7789_send_command(st7789, ST7789_GCTRL, &gctrl_data, 1)) {
      return false;
    }

    const uint8_t vcoms_data = 0x37;
    if (!_driver_st7789_send_command(st7789, ST7789_VCOMS, &vcoms_data, 1)) {
      return false;
    }

    const uint8_t gmctrp1_data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F,
                                    0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
    if (!_driver_st7789_send_command(st7789, ST7789_GMCTRP1, gmctrp1_data,
                                     14)) {
      return false;
    }

    const uint8_t gmctrn1_data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F,
                                    0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
    if (!_driver_st7789_send_command(st7789, ST7789_GMCTRN1, gmctrn1_data,
                                     14)) {
      return false;
    }
  } else if (st7789->width == 240 && st7789->height == 135) {
    // Pico Display Pack specific settings
    const uint8_t vrhs_pico_data = 0x00;
    if (!_driver_st7789_send_command(st7789, ST7789_VRHS, &vrhs_pico_data, 1)) {
      return false;
    }

    const uint8_t gctrl_data = 0x75;
    if (!_driver_st7789_send_command(st7789, ST7789_GCTRL, &gctrl_data, 1)) {
      return false;
    }

    const uint8_t vcoms_data = 0x3D;
    if (!_driver_st7789_send_command(st7789, ST7789_VCOMS, &vcoms_data, 1)) {
      return false;
    }

    const uint8_t unknown_cmd_data = 0xa1;
    if (!_driver_st7789_send_command(st7789, 0xd6, &unknown_cmd_data, 1)) {
      return false;
    }

    const uint8_t gmctrp1_data[] = {0x70, 0x04, 0x08, 0x09, 0x09, 0x05, 0x2A,
                                    0x33, 0x41, 0x07, 0x13, 0x13, 0x29, 0x2f};
    if (!_driver_st7789_send_command(st7789, ST7789_GMCTRP1, gmctrp1_data,
                                     14)) {
      return false;
    }

    const uint8_t gmctrn1_data[] = {0x70, 0x03, 0x09, 0x0A, 0x09, 0x06, 0x2B,
                                    0x34, 0x41, 0x07, 0x12, 0x14, 0x28, 0x2E};
    if (!_driver_st7789_send_command(st7789, ST7789_GMCTRN1, gmctrn1_data,
                                     14)) {
      return false;
    }
  } else if (st7789->width == 320 && st7789->height == 240) {
    const uint8_t gctrl_data = 0x35;
    if (!_driver_st7789_send_command(st7789, ST7789_GCTRL, &gctrl_data, 1)) {
      return false;
    }

    const uint8_t vcoms_data = 0x1f;
    if (!_driver_st7789_send_command(st7789, ST7789_VCOMS, &vcoms_data, 1)) {
      return false;
    }

    const uint8_t gmctrp1_data[] = {0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39,
                                    0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2D};
    if (!_driver_st7789_send_command(st7789, ST7789_GMCTRP1, gmctrp1_data,
                                     14)) {
      return false;
    }

    const uint8_t gmctrn1_data[] = {0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39,
                                    0x44, 0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31};
    if (!_driver_st7789_send_command(st7789, ST7789_GMCTRN1, gmctrn1_data,
                                     14)) {
      return false;
    }
  }

  // Enable display inversion and turn on display
  if (!_driver_st7789_send_command(st7789, ST7789_INVON, NULL, 0)) {
    return false;
  }

  if (!_driver_st7789_send_command(st7789, ST7789_SLPOUT, NULL, 0)) {
    return false;
  }

  if (!_driver_st7789_send_command(st7789, ST7789_DISPON, NULL, 0)) {
    return false;
  }

  sys_sleep(100);

  // Configure display rotation and window
  if (!_driver_st7789_configure_display(st7789)) {
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a ST7789 display driver using the SPI interface.
 */
driver_st7789_t driver_st7789_init(hw_spi_t *spi, uint8_t dc_pin,
                                   uint8_t reset_pin, uint8_t backlight_pin,
                                   uint16_t width, uint16_t height,
                                   bool round) {
  sys_assert(spi);
  sys_assert(hw_spi_valid(spi));
  sys_assert(width > 0 && height > 0);

  // Create the ST7789 driver structure
  driver_st7789_t st7789 = {0};

  // Initialize GPIO pins
  st7789.dc = hw_gpio_init(dc_pin, HW_GPIO_OUTPUT);

  // Initialize optional reset pin
  if (reset_pin != 0) {
    st7789.reset = hw_gpio_init(reset_pin, HW_GPIO_OUTPUT);
  } else {
    st7789.reset = (hw_gpio_t){0}; // Invalid GPIO
  }

  // Initialize optional backlight pin
  if (backlight_pin != 0) {
    st7789.backlight = hw_gpio_init(backlight_pin, HW_GPIO_OUTPUT);
  } else {
    st7789.backlight = (hw_gpio_t){0}; // Invalid GPIO
  }

  // Check if required GPIO initialization was successful
  if (!hw_gpio_valid(&st7789.dc)) {
    return st7789; // Return invalid structure
  }

  // Set default GPIO states
  hw_gpio_set(&st7789.dc, false); // DC low initially
  if (hw_gpio_valid(&st7789.reset)) {
    hw_gpio_set(&st7789.reset, true); // Reset high (inactive)
  }

  // Initialize structure
  st7789.spi = spi;
  st7789.width = width;
  st7789.height = height;
  st7789.rotation = DRIVER_ST7789_ROTATION_0;
  st7789.interface = DRIVER_ST7789_INTERFACE_SPI;
  st7789.round = round;
  st7789.backlight_brightness = 0;

  // Initialize display
  if (!_driver_st7789_init_display(&st7789)) {
    // Clear the structure to indicate failure
    sys_memset(&st7789, 0, sizeof(driver_st7789_t));
    return st7789;
  }

  // Set backlight to off initially
  if (hw_gpio_valid(&st7789.backlight)) {
    driver_st7789_set_backlight(&st7789, 0);
    sys_sleep(50);
    driver_st7789_set_backlight(&st7789, 255); // Turn on after initialization
  }

  return st7789;
}

/**
 * @brief Finalize and release a ST7789 display driver.
 */
void driver_st7789_finalize(driver_st7789_t *st7789) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  // Turn off backlight
  if (hw_gpio_valid(&st7789->backlight)) {
    driver_st7789_set_backlight(st7789, 0);
  }

  // Turn off display
  _driver_st7789_send_command(st7789, ST7789_DISPOFF, NULL, 0);

  // Finalize GPIO pins
  if (hw_gpio_valid(&st7789->dc)) {
    hw_gpio_finalize(&st7789->dc);
  }
  if (hw_gpio_valid(&st7789->reset)) {
    hw_gpio_finalize(&st7789->reset);
  }
  if (hw_gpio_valid(&st7789->backlight)) {
    hw_gpio_finalize(&st7789->backlight);
  }

  // Clear the driver structure
  sys_memset(st7789, 0, sizeof(driver_st7789_t));
}

/**
 * @brief Check if a ST7789 driver structure is valid.
 */
bool driver_st7789_valid(const driver_st7789_t *st7789) {
  return st7789 && st7789->spi && hw_spi_valid(st7789->spi) &&
         hw_gpio_valid((hw_gpio_t *)&st7789->dc) && st7789->width > 0 &&
         st7789->height > 0;
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

/**
 * @brief Set the rotation of the ST7789 display.
 */
bool driver_st7789_set_rotation(driver_st7789_t *st7789,
                                driver_st7789_rotation_t rotation) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  st7789->rotation = rotation;
  return _driver_st7789_configure_display(st7789);
}

/**
 * @brief Set the backlight brightness of the ST7789 display.
 */
bool driver_st7789_set_backlight(driver_st7789_t *st7789, uint8_t brightness) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  if (!hw_gpio_valid(&st7789->backlight)) {
    return false; // No backlight control available
  }

  st7789->backlight_brightness = brightness;

  // Convert brightness to PWM duty cycle with gamma correction
  // float gamma = 2.8f;
  // float normalized = (float)brightness / 255.0f;
  // uint16_t pwm_value = (uint16_t)(powf(normalized, gamma) * 65535.0f + 0.5f);

  // Set PWM value (assuming PWM is configured for the backlight pin)
  // This would typically be done through a PWM hardware abstraction
  // For now, we'll use simple on/off control
  hw_gpio_set(&st7789->backlight, brightness > 127);

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// DISPLAY OPERATIONS

/**
 * @brief Reset the ST7789 display.
 */
void driver_st7789_reset(driver_st7789_t *st7789) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  if (hw_gpio_valid(&st7789->reset)) {
    hw_gpio_set(&st7789->reset, false);
    sys_sleep(10);
    hw_gpio_set(&st7789->reset, true);
    sys_sleep(10);
  }
}

/**
 * @brief Set the display window for subsequent pixel data.
 */
bool driver_st7789_set_window(driver_st7789_t *st7789,
                              driver_st7789_rect_t rect) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  // Validate rectangle
  if (rect.x >= st7789->width || rect.y >= st7789->height ||
      rect.x + rect.width > st7789->width ||
      rect.y + rect.height > st7789->height) {
    return false;
  }

  // Set column address
  uint8_t caset_data[4] = {(rect.x >> 8) & 0xFF, rect.x & 0xFF,
                           ((rect.x + rect.width - 1) >> 8) & 0xFF,
                           (rect.x + rect.width - 1) & 0xFF};
  if (!_driver_st7789_send_command(st7789, ST7789_CASET, caset_data, 4)) {
    return false;
  }

  // Set row address
  uint8_t raset_data[4] = {(rect.y >> 8) & 0xFF, rect.y & 0xFF,
                           ((rect.y + rect.height - 1) >> 8) & 0xFF,
                           (rect.y + rect.height - 1) & 0xFF};
  if (!_driver_st7789_send_command(st7789, ST7789_RASET, raset_data, 4)) {
    return false;
  }

  return true;
}

/**
 * @brief Send pixel data to the ST7789 display.
 */
bool driver_st7789_write_pixels(driver_st7789_t *st7789, const uint8_t *data,
                                size_t size) {
  sys_assert(st7789 && driver_st7789_valid(st7789));
  sys_assert(data && size > 0);

  // Send RAMWR command
  hw_gpio_set(&st7789->dc, false);
  uint8_t cmd = ST7789_RAMWR;
  size_t bytes_sent = hw_spi_xfr(st7789->spi, &cmd, 1, 0);
  if (bytes_sent != 1) {
    return false;
  }

  // Send pixel data
  hw_gpio_set(&st7789->dc, true);
  bytes_sent = hw_spi_xfr(st7789->spi, (void *)data, size, 0);

  return (bytes_sent == size);
}

/**
 * @brief Fill the entire display with a single color.
 */
bool driver_st7789_fill(driver_st7789_t *st7789, uint16_t color) {
  sys_assert(st7789 && driver_st7789_valid(st7789));

  // Don't set window here - use the window configured during initialization
  // which has the correct offsets for the specific display size

  // Convert color to big-endian bytes
  uint8_t color_bytes[2] = {(color >> 8) & 0xFF, color & 0xFF};

  // Send RAMWR command
  hw_gpio_set(&st7789->dc, false);
  uint8_t cmd = ST7789_RAMWR;
  size_t cmd_bytes_sent = hw_spi_xfr(st7789->spi, &cmd, 1, 0);
  if (cmd_bytes_sent != 1) {
    return false;
  }

  // Send color data in one massive chunk for ultimate performance
  hw_gpio_set(&st7789->dc, true);
  size_t total_pixels = (size_t)st7789->width * st7789->height;
  size_t total_bytes = total_pixels * 2; // 2 bytes per pixel

  // Allocate entire framebuffer dynamically - 64KB for 240x135 display
  uint8_t *framebuffer = sys_malloc(total_bytes);
  if (!framebuffer) {
    return false; // Out of memory
  }

  // Fill entire framebuffer with the color pattern
  for (size_t i = 0; i < total_pixels; i++) {
    framebuffer[i * 2] = color_bytes[0];
    framebuffer[i * 2 + 1] = color_bytes[1];
  }

  // Send entire framebuffer in ONE transaction
  size_t bytes_sent = hw_spi_xfr(st7789->spi, framebuffer, total_bytes, 0);
  bool success = (bytes_sent == total_bytes);

  // Free the framebuffer
  sys_free(framebuffer);

  return success;
}

/**
 * @brief Update a rectangular region of the ST7789 display.
 */
bool driver_st7789_update_rect(driver_st7789_t *st7789,
                               driver_st7789_rect_t rect, const uint8_t *data,
                               size_t size) {
  sys_assert(st7789 && driver_st7789_valid(st7789));
  sys_assert(data && size > 0);

  // Validate expected size
  size_t expected_size =
      (size_t)rect.width * rect.height * 2; // 2 bytes per pixel
  if (size != expected_size) {
    return false;
  }

  // Set window
  if (!driver_st7789_set_window(st7789, rect)) {
    return false;
  }

  // Write pixels
  return driver_st7789_write_pixels(st7789, data, size);
}
