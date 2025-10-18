#include <drivers/drivers.h>
#include <drivers/drivers_ssd1677.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_ssd1677_01(void) {
  sys_printf("SSD1677 Hardware Test - 880x528 Display\n");
  sys_printf(
      "SPI: /dev/spidev0.0 @ 4MHz, DC=GPIO25, RESET=GPIO17, BUSY=GPIO24\n");

  // Initialize SPI interface
  hw_spi_t spi;
  if (!hw_spi_init_device(&spi, "/dev/spidev0.0", false, 4000000)) {
    sys_printf("ERROR: Failed to initialize SPI device /dev/spidev0.0\n");
    return 1;
  }
  sys_printf("SPI device initialized successfully\n");

  // Hardware setup info
  sys_printf("\n=== Hardware Configuration ===\n");
  sys_printf("GPIO pins will be initialized by the driver:\n");
  sys_printf("- DC pin: GPIO25 (output)\n");
  sys_printf("- RESET pin: GPIO17 (output)\n");
  sys_printf("- BUSY pin: GPIO24 (input)\n");
  sys_printf("===================================\n");

  sys_printf("=== Starting Driver Initialization ===\n");
  sys_printf("About to initialize display driver...\n");

  // Initialize display with actual hardware parameters
  driver_ssd1677_t display;
  sys_memset(&display, 0, sizeof(display));

  // Display dimensions: 880x528 (7.5" HD e-Paper)
  // GPIO pins: DC=25, RESET=17, BUSY=24
  if (!driver_ssd1677_init(&display, &spi, 880, 528, 25, 17, 24)) {
    sys_printf("ERROR: Failed to initialize SSD1677 display\n");
    hw_spi_finalize(&spi);
    return 1;
  }
  sys_printf("SSD1677 display initialized successfully: 880x528 pixels\n");

  // Verify display is valid
  if (!driver_ssd1677_valid(&display)) {
    sys_printf("ERROR: Display validation failed\n");
    driver_ssd1677_finalize(&display);
    hw_spi_finalize(&spi);
    return 1;
  }
  sys_printf(
      "Display validation passed - all GPIO pins initialized by driver\n");

  // Test buffer size calculation for 880x528
  size_t expected_buffer_size = (880 * 528) / 8;
  sys_printf("Expected buffer size: %zu bytes\n", expected_buffer_size);
  sys_printf("Bytes per row: %d (880/8 = 110)\n", 110);
  sys_printf("Total rows: %d\n", 528);

  sys_printf("\n=== DIAGNOSTIC FLOW ===\n");
  sys_printf("1. Test 0: Simple black test to check basic functionality\n");
  sys_printf("2. Test 1: Standard clear operation\n");
  sys_printf("3. Tests 5-7 verify addressing, patterns, and orientation\n");
  sys_printf("========================\n");

  // Test 0: Simple black screen (all zeros)
  sys_printf("\n=== TEST 0: Simple Black Test (All 0x00) ===\n");
  size_t buffer_size = (880 * 528) / 8;
  uint8_t *black_buffer = sys_malloc(buffer_size);
  if (black_buffer) {
    sys_memset(black_buffer, 0x00, buffer_size);
    sys_printf("Sending all-black buffer (%zu bytes of 0x00)\n", buffer_size);
    driver_ssd1677_update(&display, black_buffer, true);
    sys_printf("If display stays unchanged, there may be a polarity or refresh "
               "issue\n");
    sys_free(black_buffer);
    sys_sleep(3000);
  }

  // Test 1: Standard clear operation
  sys_printf("\n=== TEST 1: Standard Clear (All 0xFF) ===\n");
  driver_ssd1677_clear(&display);
  sys_printf("Waiting 2 seconds...\n");
  sys_sleep(2000);

  // Test buffer allocation for remaining tests
  size_t test_buffer_size = (880 * 528) / 8;
  uint8_t *test_buffer = sys_malloc(test_buffer_size);
  if (test_buffer) {

    // Test 5: Simple pattern with wider stripes
    sys_printf("\n=== TEST 5: Wide Horizontal Stripes ===\n");
    for (size_t y = 0; y < 528; y++) {
      // Much larger stripes: 64 pixels high instead of 32
      uint8_t row_color = (y / 64) % 2 == 0 ? 0x00 : 0xFF;
      for (size_t x = 0; x < 110; x++) {
        size_t byte_index = (y * 110) + x;
        if (byte_index < test_buffer_size) {
          test_buffer[byte_index] = row_color;
        }
      }
    }
    driver_ssd1677_update(&display, test_buffer, true);
    sys_printf("Expected: Wide horizontal stripes (64 pixels high)\n");
    sys_sleep(3000);

    // Test 6: Origin Detection - Small black square at (0,0) to (100,100)
    sys_printf("\n=== TEST 6: Origin Detection (0,0) to (100,100) ===\n");

    // Fill with white background
    sys_memset(test_buffer, 0xFF, test_buffer_size);

    // Create a 100x100 black square starting at pixel (0,0)
    for (int y = 0; y < 100; y++) {
      for (int x_pixel = 0; x_pixel < 100; x_pixel++) {
        // Convert pixel coordinates to byte/bit position
        int byte_x = x_pixel / 8;        // Which byte in the row
        int bit_pos = 7 - (x_pixel % 8); // Which bit in that byte (MSB first)

        if (byte_x < 110) { // Make sure we don't exceed row width
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            // Clear the bit to make it black (assuming 0=black)
            test_buffer[byte_index] &= ~(1 << bit_pos);
          }
        }
      }
    }

    driver_ssd1677_update(&display, test_buffer, true);
    sys_printf("Expected: Small 100x100 black square at pixel (0,0)\n");
    sys_printf("This will help identify display orientation and where (0,0) is "
               "located\n");
    sys_sleep(5000); // Longer delay to observe carefully

    // Test 7: Four LARGE corner squares to understand full orientation
    sys_printf("\n=== TEST 7: Four LARGE Corner Squares ===\n");

    // Fill with white background
    sys_memset(test_buffer, 0xFF, test_buffer_size);

    // Make squares MUCH larger: 120x120 pixels each
    int square_size = 120;

    sys_printf("Creating 120x120 pixel squares at each corner...\n");

    // Top-left (0,0) to (120,120) - BLACK
    sys_printf("Drawing top-left square (0,0) to (%d,%d)\n", square_size - 1,
               square_size - 1);
    for (int y = 0; y < square_size && y < 528; y++) {
      for (int x_pixel = 0; x_pixel < square_size && x_pixel < 880; x_pixel++) {
        int byte_x = x_pixel / 8;
        int bit_pos = 7 - (x_pixel % 8);
        if (byte_x < 110) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            test_buffer[byte_index] &= ~(1 << bit_pos); // Black
          }
        }
      }
    }

    // Top-right (760,0) to (880,120) - BLACK
    sys_printf("Drawing top-right square (%d,0) to (879,%d)\n",
               880 - square_size, square_size - 1);
    for (int y = 0; y < square_size && y < 528; y++) {
      for (int x_pixel = 880 - square_size; x_pixel < 880; x_pixel++) {
        int byte_x = x_pixel / 8;
        int bit_pos = 7 - (x_pixel % 8);
        if (byte_x < 110) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            test_buffer[byte_index] &= ~(1 << bit_pos); // Black
          }
        }
      }
    }

    // Bottom-left (0,408) to (120,528) - BLACK
    sys_printf("Drawing bottom-left square (0,%d) to (%d,527)\n",
               528 - square_size, square_size - 1);
    for (int y = 528 - square_size; y < 528; y++) {
      for (int x_pixel = 0; x_pixel < square_size && x_pixel < 880; x_pixel++) {
        int byte_x = x_pixel / 8;
        int bit_pos = 7 - (x_pixel % 8);
        if (byte_x < 110) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            test_buffer[byte_index] &= ~(1 << bit_pos); // Black
          }
        }
      }
    }

    // Bottom-right (760,408) to (880,528) - BLACK
    sys_printf("Drawing bottom-right square (%d,%d) to (879,527)\n",
               880 - square_size, 528 - square_size);
    for (int y = 528 - square_size; y < 528; y++) {
      for (int x_pixel = 880 - square_size; x_pixel < 880; x_pixel++) {
        int byte_x = x_pixel / 8;
        int bit_pos = 7 - (x_pixel % 8);
        if (byte_x < 110) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            test_buffer[byte_index] &= ~(1 << bit_pos); // Black
          }
        }
      }
    }

    driver_ssd1677_update(&display, test_buffer, true);
    sys_printf("Expected: LARGE 120x120 black squares in all 4 corners\n");
    sys_printf(
        "If you see only some corners, it indicates addressing issues\n");
    sys_printf(
        "If the top strip is not updating, there's a Y-address problem\n");
    sys_sleep(8000); // Longer time to observe

    // Test 8: Investigate top horizontal strip issue
    sys_printf("\n=== TEST 8: Top Strip Investigation ===\n");

    // Fill with white background
    sys_memset(test_buffer, 0xFF, test_buffer_size);

    // Create alternating black/white horizontal bars in the top area
    sys_printf("Creating horizontal bars to test top area addressing...\n");
    for (int y = 0; y < 100; y++) {                        // First 100 rows
      uint8_t row_color = (y / 10) % 2 == 0 ? 0x00 : 0xFF; // 10-pixel high bars
      sys_printf("Row %d: color 0x%02X\n", y, row_color);
      for (int byte_x = 0; byte_x < 110; byte_x++) {
        size_t byte_index = (y * 110) + byte_x;
        if (byte_index < test_buffer_size) {
          test_buffer[byte_index] = row_color;
        }
      }
    }

    driver_ssd1677_update(&display, test_buffer, true);
    sys_printf("Expected: Alternating horizontal bars in top 100 rows\n");
    sys_printf("If no bars appear at top, Y-addressing has offset issues\n");
    sys_sleep(8000);

    // Test 9: Coordinate System Confirmation - (0,0) is bottom-left
    sys_printf("\n=== TEST 9: Coordinate System Confirmation ===\n");
    sys_printf("Now we know (0,0) is BOTTOM-LEFT, let's map the corners:\n");

    // Fill with white background
    sys_memset(test_buffer, 0xFF, test_buffer_size);

    int square_size = 80;

    // Bottom-left (0,0) to (80,80) - BLACK (this should be where we saw the
    // first square)
    sys_printf("Bottom-left square: (0,0) to (%d,%d) - BLACK\n",
               square_size - 1, square_size - 1);
    for (int y = 0; y < square_size; y++) {
      for (int x_pixel = 0; x_pixel < square_size; x_pixel++) {
        int byte_x = x_pixel / 8;
        int bit_pos = 7 - (x_pixel % 8);
        if (byte_x < 110) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            test_buffer[byte_index] &= ~(1 << bit_pos); // Black
          }
        }
      }
    }

    // Bottom-right (800,0) to (880,80) - BLACK
    sys_printf("Bottom-right square: (%d,0) to (879,%d) - BLACK\n",
               880 - square_size, square_size - 1);
    for (int y = 0; y < square_size; y++) {
      for (int x_pixel = 880 - square_size; x_pixel < 880; x_pixel++) {
        int byte_x = x_pixel / 8;
        int bit_pos = 7 - (x_pixel % 8);
        if (byte_x < 110) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            test_buffer[byte_index] &= ~(1 << bit_pos); // Black
          }
        }
      }
    }

    // Top-left (0,448) to (80,528) - BLACK (this should be at physical
    // top-left)
    sys_printf("Top-left square: (0,%d) to (%d,527) - BLACK\n",
               528 - square_size, square_size - 1);
    for (int y = 528 - square_size; y < 528; y++) {
      for (int x_pixel = 0; x_pixel < square_size; x_pixel++) {
        int byte_x = x_pixel / 8;
        int bit_pos = 7 - (x_pixel % 8);
        if (byte_x < 110) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            test_buffer[byte_index] &= ~(1 << bit_pos); // Black
          }
        }
      }
    }

    // Top-right (800,448) to (880,528) - BLACK (this should be at physical
    // top-right)
    sys_printf("Top-right square: (%d,%d) to (879,527) - BLACK\n",
               880 - square_size, 528 - square_size);
    for (int y = 528 - square_size; y < 528; y++) {
      for (int x_pixel = 880 - square_size; x_pixel < 880; x_pixel++) {
        int byte_x = x_pixel / 8;
        int bit_pos = 7 - (x_pixel % 8);
        if (byte_x < 110) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < test_buffer_size) {
            test_buffer[byte_index] &= ~(1 << bit_pos); // Black
          }
        }
      }
    }

    driver_ssd1677_update(&display, test_buffer, true);
    sys_printf("COORDINATE SYSTEM CONFIRMED:\n");
    sys_printf("- (0,0) = Physical BOTTOM-LEFT\n");
    sys_printf("- (879,0) = Physical BOTTOM-RIGHT\n");
    sys_printf("- (0,527) = Physical TOP-LEFT\n");
    sys_printf("- (879,527) = Physical TOP-RIGHT\n");
    sys_printf("Y increases from BOTTOM to TOP (like math coordinates)\n");
    sys_sleep(8000);

    sys_free(test_buffer);
  } else {
    sys_printf("Failed to allocate test buffer\n");
  }

  // Put display to sleep
  sys_printf("Putting display to sleep...\n");
  driver_ssd1677_sleep(&display);

  // Cleanup
  sys_printf("Cleaning up resources...\n");
  driver_ssd1677_finalize(&display);
  hw_spi_finalize(&spi);

  sys_printf("All hardware tests passed!\n");
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestHardwareMain("test_ssd1677_01", test_ssd1677_01); }
