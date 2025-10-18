/**
 * @file ssd1677_example.c
 * @brief SSD1677 E-Paper Display Driver Example
 */
#include <Application/Application.h>
#include <drivers/drivers_ssd1677.h>
#include <runtime-sys/sys.h>

static driver_ssd1677_t display;

// Simple 1-bit bitmap pattern (checkerboard)
static void create_test_pattern(uint8_t *buffer, uint16_t width,
                                uint16_t height) {
  uint16_t buffer_size = (width * height) / 8;

  for (uint16_t i = 0; i < buffer_size; i++) {
    // Create a checkerboard pattern
    buffer[i] = (((i / (width / 8)) + (i % (width / 8))) % 2) ? 0x55 : 0xAA;
  }
}

int main(void) {
  sys_printf("SSD1677 E-Paper Display Example\n");

  // Initialize SPI first
  hw_spi_t spi;
  if (!hw_spi_init(&spi, 0, 0, 0, 0, 8, true, 2000000)) {
    sys_printf("Failed to initialize SPI interface\n");
    return -1;
  }

  // Initialize the display (adjust pins for your hardware)
  // SPI already initialized, width=296, height=128, DC=24, RESET=23, BUSY=25
  if (!driver_ssd1677_init(&display, &spi, 296, 128, 24, 23, 25)) {
    sys_printf("Failed to initialize SSD1677 display\n");
    hw_spi_finalize(&spi);
    return -1;
  }
  sys_printf("Display initialized: 296x128 pixels\n");

  // Clear display with full refresh
  sys_printf("Clearing display...\n");
  driver_ssd1677_clear(&display);
  sys_sleep(2000);

  // Create test pattern
  uint16_t width = 296;
  uint16_t height = 128;
  size_t buffer_size = (width * height) / 8;

  uint8_t *image_buffer = sys_malloc(buffer_size);
  if (!image_buffer) {
    sys_printf("Failed to allocate image buffer\n");
    ssd1677_finalize(&display);
    return -1;
  }

  // Display test pattern
  sys_printf("Creating test pattern...\n");
  create_test_pattern(image_buffer, width, height);
  driver_ssd1677_update(&display, image_buffer, true);
  sys_sleep(2000);

  // Clear again
  sys_printf("Clearing display again...\n");
  driver_ssd1677_clear(&display);
  sys_sleep(2000);

  // Test partial update
  sys_printf("Testing partial update...\n");

  // Create smaller pattern for partial update
  uint16_t partial_width = 64;
  uint16_t partial_height = 64;
  uint16_t partial_size = (partial_width * partial_height) / 8;

  uint8_t *partial_buffer = sys_malloc(partial_size);
  if (partial_buffer) {
    // Fill with solid pattern
    sys_memset(partial_buffer, 0x00, partial_size); // Black pixels

    // Display in center of screen using frame_part function
    uint16_t x_offset = (width - partial_width) / 2;
    uint16_t y_offset = (height - partial_height) / 2;
    uint16_t x_end = x_offset + partial_width;
    uint16_t y_end = y_offset + partial_height;

    driver_ssd1677_update_partial(&display, partial_buffer, x_offset, y_offset,
                                  x_end, y_end, false);
    sys_sleep(2000);

    sys_free(partial_buffer);
  }

  // Put display to sleep
  sys_printf("Putting display to sleep...\n");
  driver_ssd1677_sleep(&display);

  // Cleanup
  sys_free(image_buffer);
  driver_ssd1677_finalize(&display);
  hw_spi_finalize(&spi);

  sys_printf("Example completed\n");
  return 0;
}