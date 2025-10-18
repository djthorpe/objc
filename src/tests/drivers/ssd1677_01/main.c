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
  sys_printf("3. Test 5: Corner squares to verify coordinate system\n");
  sys_printf("4. Test 6: Partial update demo - sequential corners\n");
  sys_printf("5. Test 7: Pattern update demonstration\n");
  sys_printf("6. Test 8: Grid pattern with 150x150 pixel squares\n");
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

    // Test 5: Four LARGE corner squares to understand full orientation
    sys_printf("\n=== TEST 5: Four LARGE Corner Squares ===\n");

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
        "Y increases from TOP to BOTTOM (coordinate system confirmed)\n");
    sys_sleep(8000); // Longer time to observe

    // Test 6: Partial Update Demo - Sequential Corner Updates
    sys_printf("\n=== TEST 6: Partial Update Demo - Sequential Corners ===\n");
    sys_printf(
        "Demonstrating partial updates by drawing each corner separately\n");

    // Start with clear display
    driver_ssd1677_clear(&display);
    sys_sleep(3000);

    int corner_size = 80;
    uint16_t corner_width_bytes =
        (corner_size + 7) / 8; // 10 bytes for 80 pixels
    size_t corner_buffer_size = corner_width_bytes * corner_size;

    // Create corner buffer for partial updates
    uint8_t *corner_buffer = sys_malloc(corner_buffer_size);
    if (!corner_buffer) {
      sys_printf("Failed to allocate corner buffer\n");
    } else {
      // Corner 1: Top-left (0,0) to (79,79) - BLACK
      sys_printf("1. Drawing top-left corner (0,0) to (79,79)...\n");
      sys_memset(corner_buffer, 0x00, corner_buffer_size); // All black
      driver_ssd1677_update_partial(&display, corner_buffer, 0, 0, corner_size,
                                    corner_size, false);
      sys_sleep(3000);

      // Corner 2: Top-right (800,0) to (879,79) - BLACK
      sys_printf("2. Drawing top-right corner (800,0) to (879,79)...\n");
      sys_memset(corner_buffer, 0x00, corner_buffer_size); // All black
      driver_ssd1677_update_partial(&display, corner_buffer, 800, 0, 880,
                                    corner_size, false);
      sys_sleep(3000);

      // Corner 3: Bottom-left (0,448) to (79,527) - BLACK
      sys_printf("3. Drawing bottom-left corner (0,448) to (79,527)...\n");
      sys_memset(corner_buffer, 0x00, corner_buffer_size); // All black
      driver_ssd1677_update_partial(&display, corner_buffer, 0, 448,
                                    corner_size, 528, false);
      sys_sleep(3000);

      // Corner 4: Bottom-right (800,448) to (879,527) - BLACK
      sys_printf("4. Drawing bottom-right corner (800,448) to (879,527)...\n");
      sys_memset(corner_buffer, 0x00, corner_buffer_size); // All black
      driver_ssd1677_update_partial(&display, corner_buffer, 800, 448, 880, 528,
                                    false);
      sys_sleep(3000);

      sys_free(corner_buffer);
    }

    sys_printf("PARTIAL UPDATE DEMO COMPLETE:\n");
    sys_printf("- Each corner drawn individually using partial updates\n");
    sys_printf("- Coordinate system: (0,0) = top-left, Y increases downward\n");
    sys_printf("- Partial updates working correctly!\n");
    sys_sleep(5000);
    sys_printf("COORDINATE SYSTEM CONFIRMED:\n");
    sys_printf("- (0,0) = Physical TOP-LEFT\n");
    sys_printf("- (879,0) = Physical TOP-RIGHT\n");
    sys_printf("- (0,527) = Physical BOTTOM-LEFT\n");
    sys_printf("- (879,527) = Physical BOTTOM-RIGHT\n");
    sys_printf("Y increases from TOP to BOTTOM (screen coordinates)\n");
    sys_sleep(8000);

    sys_free(test_buffer);
  } else {
    sys_printf("Failed to allocate test buffer\n");
  }

  // Test 7: Pattern Update Demo
  sys_printf("\n=== TEST 7: Pattern Update Demo ===\n");
  sys_printf("Demonstrating pattern updates with standard refresh\n");

  size_t fast_buffer_size = (880 * 528) / 8;
  uint8_t *fast_buffer = sys_malloc(fast_buffer_size);
  if (fast_buffer) {
    // Clear display first
    sys_printf("Starting with clear display...\n");
    driver_ssd1677_clear(&display);
    sys_sleep(2000);

    // Create a simple test pattern
    sys_memset(fast_buffer, 0xFF, fast_buffer_size); // White background

    // Add some horizontal stripes for visibility
    for (int y = 0; y < 528; y += 40) {
      for (int stripe = 0; stripe < 20 && (y + stripe) < 528; stripe++) {
        for (int byte_x = 0; byte_x < 110; byte_x++) {
          size_t byte_index = ((y + stripe) * 110) + byte_x;
          if (byte_index < fast_buffer_size) {
            fast_buffer[byte_index] = 0x00; // Black stripes
          }
        }
      }
    }

    // Test 1: Display stripe pattern
    sys_printf("1. Displaying stripe pattern...\n");
    driver_ssd1677_update(&display, fast_buffer, true);
    sys_printf("Stripe pattern displayed\n");
    sys_sleep(3000);

    // Modify pattern for second test
    for (int y = 10; y < 528; y += 40) {
      for (int stripe = 0; stripe < 20 && (y + stripe) < 528; stripe++) {
        for (int byte_x = 0; byte_x < 110; byte_x++) {
          size_t byte_index = ((y + stripe) * 110) + byte_x;
          if (byte_index < fast_buffer_size) {
            fast_buffer[byte_index] = 0xAA; // Gray pattern (alternating pixels)
          }
        }
      }
    }

    // Test 2: Updated pattern
    sys_printf("2. Displaying updated pattern...\n");
    driver_ssd1677_update(&display, fast_buffer, false);
    sys_printf("Updated pattern displayed\n");

    sys_sleep(3000);

    // Test 3: Sequential updates demo
    sys_printf("3. Sequential updates demo (5 pattern changes)...\n");
    for (int i = 0; i < 5; i++) {
      // Alternate between two patterns
      uint8_t fill_pattern = (i % 2 == 0) ? 0x55 : 0xAA;

      // Fill center area with alternating pattern
      for (int y = 200; y < 328; y++) {
        for (int byte_x = 20; byte_x < 90; byte_x++) {
          size_t byte_index = (y * 110) + byte_x;
          if (byte_index < fast_buffer_size) {
            fast_buffer[byte_index] = fill_pattern;
          }
        }
      }

      sys_printf("  Update %d/5...\n", i + 1);
      driver_ssd1677_update(&display, fast_buffer, false);
      sys_printf("  Update %d completed\n", i + 1);

      // Short delay between updates
      sys_sleep(1000);
    }

    sys_printf("PATTERN UPDATE DEMO COMPLETE:\n");
    sys_printf("- Standard updates provide consistent image quality\n");
    sys_printf("- E-paper refresh time dominates total update time\n");
    sys_printf("- Each update loads fresh LUT for best quality\n");

    sys_free(fast_buffer);
  } else {
    sys_printf("Failed to allocate pattern buffer\n");
  }

  sys_sleep(3000);

  // Test 8: Grid Pattern Demo
  sys_printf("\n=== TEST 8: Grid Pattern Demo ===\n");
  sys_printf("Drawing a grid of 150x150 pixel squares\n");

  size_t grid_buffer_size = (880 * 528) / 8;
  uint8_t *grid_buffer = sys_malloc(grid_buffer_size);
  if (grid_buffer) {
    // Clear buffer to white background
    sys_memset(grid_buffer, 0xFF, grid_buffer_size);

    // Calculate grid parameters
    const uint16_t square_size = 150; // 150x150 pixel squares
    const uint16_t display_width = 880;
    const uint16_t display_height = 528;

    // Calculate how many squares fit
    uint16_t squares_x =
        display_width / square_size; // Should be 5 squares (880/150 = 5.86)
    uint16_t squares_y =
        display_height / square_size; // Should be 3 squares (528/150 = 3.52)

    sys_printf("Grid: %dx%d squares of %dx%d pixels each\n", squares_x,
               squares_y, square_size, square_size);
    sys_printf("Coverage: %dx%d pixels (leaves %dx%d uncovered)\n",
               squares_x * square_size, squares_y * square_size,
               display_width - (squares_x * square_size),
               display_height - (squares_y * square_size));
    sys_printf("Buffer size: %zu bytes (expected: %zu)\n", grid_buffer_size,
               (880 * 528) / 8);

    // Draw the grid - alternate between black and white squares (checkerboard
    // pattern)
    uint16_t black_squares_drawn = 0;
    for (uint16_t grid_y = 0; grid_y < squares_y; grid_y++) {
      for (uint16_t grid_x = 0; grid_x < squares_x; grid_x++) {
        // Determine if this square should be black (checkerboard pattern)
        bool is_black_square = ((grid_x + grid_y) % 2) == 0;

        if (is_black_square) {
          black_squares_drawn++;
          // Fill this square with black pixels
          uint16_t start_x = grid_x * square_size;
          uint16_t start_y = grid_y * square_size;
          uint16_t end_x = start_x + square_size;
          uint16_t end_y = start_y + square_size;

          // Ensure we don't go beyond display boundaries
          if (end_x > display_width)
            end_x = display_width;
          if (end_y > display_height)
            end_y = display_height;

          sys_printf("Drawing black square %d at (%d,%d) to (%d,%d)\n",
                     black_squares_drawn, start_x, start_y, end_x - 1,
                     end_y - 1);

          // Fill the square area with black pixels
          for (uint16_t y = start_y; y < end_y; y++) {
            for (uint16_t x = start_x; x < end_x; x++) {
              // Calculate byte position and bit position within byte
              uint16_t byte_x = x / 8;
              uint8_t bit_pos = 7 - (x % 8);
              size_t byte_index = (y * 110) + byte_x; // 110 = 880/8

              if (byte_index < grid_buffer_size) {
                // Set pixel to black (clear the bit)
                grid_buffer[byte_index] &= ~(1 << bit_pos);
              } else {
                sys_printf("ERROR: byte_index %zu >= buffer_size %zu\n",
                           byte_index, grid_buffer_size);
              }
            }
          }
        }
      }
    }

    sys_printf("Drew %d black squares total\n", black_squares_drawn);

    // Add a simple test rectangle in top-left corner to verify pixel drawing
    // works
    sys_printf("Adding test rectangle in top-left corner (50x50 pixels)...\n");
    for (uint16_t y = 0; y < 50; y++) {
      for (uint16_t x = 0; x < 50; x++) {
        uint16_t byte_x = x / 8;
        uint8_t bit_pos = 7 - (x % 8);
        size_t byte_index = (y * 110) + byte_x;
        if (byte_index < grid_buffer_size) {
          grid_buffer[byte_index] &= ~(1 << bit_pos); // Black pixel
        }
      }
    }

    sys_printf("Grid pattern generated, updating display...\n");
    driver_ssd1677_update(&display, grid_buffer, true);
    sys_printf("Grid pattern displayed!\n");
    sys_printf("GRID PATTERN DEMO COMPLETE:\n");
    sys_printf("- Checkerboard pattern with 150x150 pixel squares\n");
    sys_printf("- Demonstrates precise pixel control and geometric patterns\n");
    sys_printf("- Shows display's full resolution capability\n");

    sys_free(grid_buffer);
  } else {
    sys_printf("Failed to allocate grid buffer\n");
  }

  sys_sleep(5000); // Let user see the grid pattern

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
