/**
 * @file main.c
 * @brief Simple ST7789 test - fill entire screen with red
 */

#include <drivers/drivers_st7789.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdio.h>

// Pin definitions
#define ST7789_PIN_DC 16
#define ST7789_PIN_CS 17
#define ST7789_PIN_SCK 18
#define ST7789_PIN_SDA 19
#define ST7789_PIN_BACKLIGHT 20

// Colors
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE 0x001F
#define COLOR_WHITE 0xFFFF
#define COLOR_BLACK 0x0000

static hw_spi_t spi;
static driver_st7789_t display;

int main() {
  sys_init();
  sys_printf("ST7789 Simple Test\n");

  // Initialize SPI (adapter, sck, tx, rx, cs, cs_active_low, baudrate)
  // CS pin is handled automatically by SPI driver
  spi = hw_spi_init(0, ST7789_PIN_SCK, ST7789_PIN_SDA, 0, ST7789_PIN_CS, true,
                    10000000);
  if (!hw_spi_valid(&spi)) {
    sys_printf("Failed to initialize SPI\n");
    return 1;
  }

  // Initialize display
  display = driver_st7789_init(&spi, ST7789_PIN_DC,
                               0, // RESET pin not connected
                               ST7789_PIN_BACKLIGHT, 240, 135, false);
  if (!driver_st7789_valid(&display)) {
    sys_printf("Failed to initialize display\n");
    return 1;
  }

  sys_printf("Display initialized\n");

  // Set rotation
  if (!driver_st7789_set_rotation(&display, DRIVER_ST7789_ROTATION_0)) {
    sys_printf("Failed to set rotation\n");
    return 1;
  }

  // Fill entire screen with red
  sys_printf("Filling screen with red...\n");
  if (!driver_st7789_fill(&display, COLOR_RED)) {
    sys_printf("Failed to fill screen\n");
    return 1;
  }

  sys_printf("Screen filled with red. Now cycling through colors...\n");

  // Cycle through different colors to demonstrate performance
  uint16_t colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE,
                       COLOR_BLACK};
  const char *color_names[] = {"RED", "GREEN", "BLUE", "WHITE", "BLACK"};
  int num_colors = sizeof(colors) / sizeof(colors[0]);

  // Keep running and cycle colors
  int color_index = 0;
  while (true) {
    sys_sleep(2000); // Wait 2 seconds between color changes

    color_index = (color_index + 1) % num_colors;
    sys_printf("Filling screen with %s...\n", color_names[color_index]);

    if (!driver_st7789_fill(&display, colors[color_index])) {
      sys_printf("Failed to fill screen with color\n");
    } else {
      sys_printf("Successfully filled screen with %s\n",
                 color_names[color_index]);
    }
  }

  return 0;
}
