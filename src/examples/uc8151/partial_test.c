/**
 * @file partial_test.c
 * @brief Simple UC8151 partial update test
 *
 * This is a minimal test program focused specifically on testing
 * partial update functionality of the UC8151 e-paper display.
 */

#include <drivers/drivers.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// CONSTANTS

// Pin definitions
#define UC8151_PIN_CS 17
#define UC8151_PIN_CLK 18
#define UC8151_PIN_MOSI 19
#define UC8151_PIN_DC 20
#define UC8151_PIN_RESET 21
#define UC8151_PIN_BUSY 26

// Display dimensions
#define DISPLAY_WIDTH 296
#define DISPLAY_HEIGHT 128
#define FRAMEBUFFER_SIZE ((DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8)

// SPI configuration
#define SPI_FREQUENCY 10000000 // 10 MHz
#define SPI_INSTANCE 0

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

static uint8_t framebuffer[FRAMEBUFFER_SIZE];
static driver_uc8151_t display;
static hw_spi_t spi;

///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

void clear_framebuffer(void) { memset(framebuffer, 0x00, FRAMEBUFFER_SIZE); }

void set_pixel(uint16_t x, uint16_t y, uint8_t color) {
  if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
    return;
  }

  uint32_t byte_index = x * (DISPLAY_HEIGHT / 8) + (y / 8);
  uint8_t bit_index = y % 8;

  if (color) {
    framebuffer[byte_index] |= (0x80 >> bit_index);
  } else {
    framebuffer[byte_index] &= ~(0x80 >> bit_index);
  }
}

void draw_filled_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                      uint8_t color) {
  for (uint16_t dy = 0; dy < height; dy++) {
    for (uint16_t dx = 0; dx < width; dx++) {
      set_pixel(x + dx, y + dy, color);
    }
  }
}

bool init_display(void) {
  // Initialize SPI
  spi = hw_spi_init(SPI_INSTANCE, UC8151_PIN_CLK, UC8151_PIN_MOSI, 0,
                    UC8151_PIN_CS, true, SPI_FREQUENCY);
  if (!hw_spi_valid(&spi)) {
    return false;
  }

  // Initialize UC8151 display
  display =
      driver_uc8151_spi_init(&spi, UC8151_PIN_DC, UC8151_PIN_RESET,
                             UC8151_PIN_BUSY, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  if (!driver_uc8151_valid(&display)) {
    hw_spi_finalize(&spi);
    return false;
  }

  // Set to fast update speed for partial updates
  driver_uc8151_set_update_speed(&display, DRIVER_UC8151_UPDATE_SPEED_FAST);

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION

int main(void) {
  sys_printf("UC8151 Partial Update Test\n");
  sys_printf("=========================\n");

  // Initialize system
  sys_init();
  hw_init();

  // Initialize display
  if (!init_display()) {
    sys_printf("Display initialization failed\n");
    return 1;
  }

  sys_printf("Display initialized successfully\n");
  sys_printf("Display dimensions: %dx%d pixels\n", DISPLAY_WIDTH,
             DISPLAY_HEIGHT);
  sys_printf("Framebuffer size: %d bytes\n", FRAMEBUFFER_SIZE);

  // Step 1: Clear the entire display
  sys_printf("\nStep 1: Clearing entire display\n");
  clear_framebuffer();
  if (driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE)) {
    sys_printf("Full clear completed\n");
  } else {
    sys_printf("Full clear failed\n");
    return 1;
  }
  sys_sleep(3000);

  // Step 2: Test small partial update in center
  sys_printf("\nStep 2: Small center square partial update\n");
  driver_uc8151_rect_t center = {DISPLAY_WIDTH / 2 - 20,
                                 DISPLAY_HEIGHT / 2 - 20, 40, 40};

  draw_filled_rect(center.x, center.y, center.width, center.height, 1);

  sys_printf("Region: x=%d, y=%d, w=%d, h=%d\n", center.x, center.y,
             center.width, center.height);

  if (driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                   center)) {
    sys_printf("Center partial update completed\n");
  } else {
    sys_printf("Center partial update failed\n");
  }
  sys_sleep(3000);

  // Step 3: Test corner partial update
  sys_printf("\nStep 3: Top-left corner partial update\n");
  driver_uc8151_rect_t corner = {10, 10, 30, 30};

  draw_filled_rect(corner.x, corner.y, corner.width, corner.height, 1);

  sys_printf("Region: x=%d, y=%d, w=%d, h=%d\n", corner.x, corner.y,
             corner.width, corner.height);

  if (driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                   corner)) {
    sys_printf("Corner partial update completed\n");
  } else {
    sys_printf("Corner partial update failed\n");
  }
  sys_sleep(3000);

  // Step 4: Test horizontal strip
  sys_printf("\nStep 4: Horizontal strip partial update\n");
  driver_uc8151_rect_t hstrip = {20, DISPLAY_HEIGHT / 2 - 5, DISPLAY_WIDTH - 40,
                                 10};

  draw_filled_rect(hstrip.x, hstrip.y, hstrip.width, hstrip.height, 1);

  sys_printf("Region: x=%d, y=%d, w=%d, h=%d\n", hstrip.x, hstrip.y,
             hstrip.width, hstrip.height);

  if (driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                   hstrip)) {
    sys_printf("Horizontal strip partial update completed\n");
  } else {
    sys_printf("Horizontal strip partial update failed\n");
  }
  sys_sleep(3000);

  // Step 5: Test vertical strip
  sys_printf("\nStep 5: Vertical strip partial update\n");
  driver_uc8151_rect_t vstrip = {DISPLAY_WIDTH / 2 - 5, 20, 10,
                                 DISPLAY_HEIGHT - 40};

  draw_filled_rect(vstrip.x, vstrip.y, vstrip.width, vstrip.height, 1);

  sys_printf("Region: x=%d, y=%d, w=%d, h=%d\n", vstrip.x, vstrip.y,
             vstrip.width, vstrip.height);

  if (driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                   vstrip)) {
    sys_printf("Vertical strip partial update completed\n");
  } else {
    sys_printf("Vertical strip partial update failed\n");
  }
  sys_sleep(3000);

  // Step 6: Full update to show complete result
  sys_printf("\nStep 6: Full update to show complete result\n");
  if (driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE)) {
    sys_printf("Full update completed\n");
  } else {
    sys_printf("Full update failed\n");
  }
  sys_sleep(3000);

  // Step 7: Final clear
  sys_printf("\nStep 7: Final clear\n");
  clear_framebuffer();
  if (driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE)) {
    sys_printf("Final clear completed\n");
  } else {
    sys_printf("Final clear failed\n");
  }

  sys_printf("\n=== Partial Update Test Complete ===\n");

  // Cleanup
  driver_uc8151_finalize(&display);
  hw_spi_finalize(&spi);
  hw_exit();
  sys_exit();
  return 0;
}
