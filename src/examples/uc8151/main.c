/**
 * @file main.c
 * @brief UC8151 e-paper display example
 *
 * This example demonstrates how to use the UC8151 e-paper display driver
 * with a Raspberry Pi Pico. It shows initialization, basic drawing,
 * full updates, partial updates, and different update speeds.
 *
 * Pin assignments:
 *   CS    = 17 (SPI chip select)
 *   CLK   = 18 (SPI clock)
 *   MOSI  = 19 (SPI data out)
 *   DC    = 20 (Data/Command select)
 *   RESET = 21 (Reset pin)
 *   BUSY  = 26 (Busy status pin)
 *
 * Display specs: 296x128 pixels, 1-bit per pixel (monochrome)
 * Framebuffer size: (296 * 128) / 8 = 4736 bytes
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

// Display dimensions - match the actual UC8151 hardware
#define DISPLAY_WIDTH 296  // Hardware width (296 pixels wide)
#define DISPLAY_HEIGHT 128 // Hardware height (128 pixels high)
#define FRAMEBUFFER_SIZE ((DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8)

// SPI configuration
#define SPI_FREQUENCY 10000000 // 10 MHz
#define SPI_INSTANCE 0         // SPI0

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

static uint8_t framebuffer[FRAMEBUFFER_SIZE];
static driver_uc8151_t display;
static hw_spi_t spi;

///////////////////////////////////////////////////////////////////////////////
// FRAMEBUFFER OPERATIONS

/**
 * @brief Clear the entire framebuffer (set to white).
 */
void clear_framebuffer(void) { memset(framebuffer, 0x00, FRAMEBUFFER_SIZE); }

/**
 * @brief Fill the entire framebuffer (set to black).
 */
void fill_framebuffer(void) { memset(framebuffer, 0xFF, FRAMEBUFFER_SIZE); }

/**
 * @brief Set a pixel in the framebuffer.
 * @param x X coordinate (0 to DISPLAY_WIDTH-1)
 * @param y Y coordinate (0 to DISPLAY_HEIGHT-1)
 * @param color 1 for black, 0 for white
 */
void set_pixel(uint16_t x, uint16_t y, uint8_t color) {
  if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
    return;
  }

  // UC8151 framebuffer is organized in columns
  // Each byte contains 8 vertical pixels (MSB = top pixel)
  uint32_t byte_index = x * (DISPLAY_HEIGHT / 8) + (y / 8);
  uint8_t bit_index = y % 8;

  if (color) {
    framebuffer[byte_index] |= (0x80 >> bit_index);
  } else {
    framebuffer[byte_index] &= ~(0x80 >> bit_index);
  }
}

/**
 * @brief Draw a horizontal line in the framebuffer.
 */
void draw_hline(uint16_t x, uint16_t y, uint16_t width, uint8_t color) {
  for (uint16_t i = 0; i < width; i++) {
    set_pixel(x + i, y, color);
  }
}

/**
 * @brief Draw a vertical line in the framebuffer.
 */
void draw_vline(uint16_t x, uint16_t y, uint16_t height, uint8_t color) {
  for (uint16_t i = 0; i < height; i++) {
    set_pixel(x, y + i, color);
  }
}

/**
 * @brief Draw a rectangle outline in the framebuffer.
 */
void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
               uint8_t color) {
  draw_hline(x, y, width, color);              // Top
  draw_hline(x, y + height - 1, width, color); // Bottom
  draw_vline(x, y, height, color);             // Left
  draw_vline(x + width - 1, y, height, color); // Right
}

/**
 * @brief Draw a filled rectangle in the framebuffer.
 */
void draw_filled_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                      uint8_t color) {
  for (uint16_t dy = 0; dy < height; dy++) {
    draw_hline(x, y + dy, width, color);
  }
}

/**
 * @brief Draw a checkerboard pattern.
 */
void draw_checkerboard(uint8_t square_size) {
  clear_framebuffer();

  for (uint16_t y = 0; y < DISPLAY_HEIGHT; y += square_size) {
    for (uint16_t x = 0; x < DISPLAY_WIDTH; x += square_size) {
      // Determine if this square should be filled
      uint8_t fill = ((x / square_size) + (y / square_size)) % 2;
      if (fill) {
        draw_filled_rect(x, y, square_size, square_size, 1);
      }
    }
  }
}

/**
 * @brief Draw concentric rectangles.
 */
void draw_concentric_rectangles(void) {
  clear_framebuffer();

  uint16_t min_dim =
      (DISPLAY_WIDTH < DISPLAY_HEIGHT) ? DISPLAY_WIDTH : DISPLAY_HEIGHT;
  uint16_t max_rects = min_dim / 8;

  for (uint16_t i = 0; i < max_rects; i++) {
    uint16_t x = i * 4;
    uint16_t y = i * 2;
    uint16_t w = DISPLAY_WIDTH - (2 * x);
    uint16_t h = DISPLAY_HEIGHT - (2 * y);

    if (w > 0 && h > 0) {
      draw_rect(x, y, w, h, 1);
    }
  }
}

/**
 * @brief Draw gradient bars (simulated with dithering).
 */
void draw_gradient_bars(void) {
  clear_framebuffer();

  uint16_t bar_height = DISPLAY_HEIGHT / 4;

  for (uint16_t bar = 0; bar < 4; bar++) {
    uint16_t y_start = bar * bar_height;

    for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
      // Create gradient effect using dithering
      uint8_t intensity = (x * 8) / DISPLAY_WIDTH; // 0-7 intensity levels

      for (uint16_t y = y_start; y < y_start + bar_height; y++) {
        // Dithering pattern based on position and intensity
        uint8_t pattern = (x % 4) + (y % 4) * 4; // 0-15 pattern value
        if (pattern < intensity * 2) {
          set_pixel(x, y, 1);
        }
      }
    }
  }
}

/**
 * @brief Draw diagonal stripes.
 */
void draw_diagonal_stripes(uint8_t stripe_width) {
  clear_framebuffer();

  for (uint16_t y = 0; y < DISPLAY_HEIGHT; y++) {
    for (uint16_t x = 0; x < DISPLAY_WIDTH; x++) {
      // Create diagonal pattern
      if (((x + y) / stripe_width) % 2 == 0) {
        set_pixel(x, y, 1);
      }
    }
  }
}

/**
 * @brief Draw a border with crosshair pattern.
 */
void draw_border_crosshair(void) {
  clear_framebuffer();

  // Border
  draw_rect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 1);
  draw_rect(2, 2, DISPLAY_WIDTH - 4, DISPLAY_HEIGHT - 4, 1);

  // Crosshairs
  uint16_t center_x = DISPLAY_WIDTH / 2;
  uint16_t center_y = DISPLAY_HEIGHT / 2;

  // Horizontal crosshair
  draw_hline(10, center_y, DISPLAY_WIDTH - 20, 1);

  // Vertical crosshair
  draw_vline(center_x, 10, DISPLAY_HEIGHT - 20, 1);

  // Corner markers
  draw_filled_rect(5, 5, 10, 10, 1);
  draw_filled_rect(DISPLAY_WIDTH - 15, 5, 10, 10, 1);
  draw_filled_rect(5, DISPLAY_HEIGHT - 15, 10, 10, 1);
  draw_filled_rect(DISPLAY_WIDTH - 15, DISPLAY_HEIGHT - 15, 10, 10, 1);
}

/**
 * @brief Draw text-like patterns using simple shapes.
 */
void draw_text_simulation(void) {
  clear_framebuffer();

  // Simulate text lines with rectangles
  uint16_t line_height = 12;
  uint16_t line_spacing = 16;
  uint16_t margin = 10;

  for (uint16_t line = 0; line < 6; line++) {
    uint16_t y = margin + (line * line_spacing);

    // Simulate words with rectangles of varying lengths
    uint16_t x = margin;
    uint16_t word_lengths[] = {40, 25, 60, 35,
                               50, 30, 45}; // Different word lengths
    uint16_t num_words = sizeof(word_lengths) / sizeof(word_lengths[0]);

    for (uint16_t word = 0; word < num_words && x < DISPLAY_WIDTH - margin;
         word++) {
      uint16_t word_len = word_lengths[word];
      if (x + word_len > DISPLAY_WIDTH - margin) {
        word_len = DISPLAY_WIDTH - margin - x;
      }

      draw_filled_rect(x, y, word_len, line_height, 1);
      x += word_len + 8; // Word spacing
    }
  }
}

/**
 * @brief Draw a simple pattern for testing coordinate system.
 */
void draw_test_pattern(void) {
  sys_printf("Creating comprehensive test patterns...\n");

  // Test 1: All black
  sys_printf("Test 1: All black (0xFF)\n");
  memset(framebuffer, 0xFF, FRAMEBUFFER_SIZE);
}

/**
 * @brief Draw speed comparison bars for each update mode.
 */
void draw_speed_bars(void) {
  clear_framebuffer();

  // Title
  draw_filled_rect(5, 5, DISPLAY_WIDTH - 10, 25, 1);

  // Speed bars (relative to update times)
  uint16_t y_pos = 40;
  uint16_t bar_height = 15;
  uint16_t spacing = 25;

  // Default speed (4.5s) - full width
  draw_filled_rect(10, y_pos, DISPLAY_WIDTH - 20, bar_height, 1);
  y_pos += spacing;

  // Medium speed (2.0s) - ~44% width
  draw_filled_rect(10, y_pos, (DISPLAY_WIDTH - 20) * 44 / 100, bar_height, 1);
  y_pos += spacing;

  // Fast speed (0.8s) - ~18% width
  draw_filled_rect(10, y_pos, (DISPLAY_WIDTH - 20) * 18 / 100, bar_height, 1);
  y_pos += spacing;

  // Turbo speed (0.25s) - ~6% width
  draw_filled_rect(10, y_pos, (DISPLAY_WIDTH - 20) * 6 / 100, bar_height, 1);

  // Labels area (simplified representation)
  y_pos += spacing + 10;
  for (int i = 0; i < 4; i++) {
    draw_rect(10, y_pos + i * 20, 30, 15, 1);
  }
}

///////////////////////////////////////////////////////////////////////////////
// DISPLAY FUNCTIONS

/**
 * @brief Initialize the UC8151 display.
 */
bool init_display(void) {
  // Initialize SPI
  sys_printf("  CLK pin: %d, MOSI pin: %d, CS pin: %d\n", UC8151_PIN_CLK,
             UC8151_PIN_MOSI, UC8151_PIN_CS);
  sys_printf("  Frequency: %d Hz\n", SPI_FREQUENCY);

  spi = hw_spi_init(SPI_INSTANCE, UC8151_PIN_CLK, UC8151_PIN_MOSI, 0,
                    UC8151_PIN_CS, true, SPI_FREQUENCY); // CS active low
  if (!hw_spi_valid(&spi)) {
    return false;
  }

  // Initialize UC8151 display
  sys_printf("  DC pin: %d, RESET pin: %d, BUSY pin: %d\n", UC8151_PIN_DC,
             UC8151_PIN_RESET, UC8151_PIN_BUSY);
  sys_printf("  Display size: %dx%d pixels\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);

  display =
      driver_uc8151_spi_init(&spi, UC8151_PIN_DC, UC8151_PIN_RESET,
                             UC8151_PIN_BUSY, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  if (!driver_uc8151_valid(&display)) {
    hw_spi_finalize(&spi);
    return false;
  }
  return true;
}

/**
 * @brief Test different update speeds.
 */
void test_update_speeds(void) {
  sys_printf("\n=== Testing Update Speeds ===\n");

  driver_uc8151_update_speed_t speeds[] = {
      DRIVER_UC8151_UPDATE_SPEED_DEFAULT, DRIVER_UC8151_UPDATE_SPEED_MEDIUM,
      DRIVER_UC8151_UPDATE_SPEED_FAST, DRIVER_UC8151_UPDATE_SPEED_TURBO};

  const char *speed_names[] = {"Default", "Medium", "Fast", "Turbo"};

  for (int i = 0; i < 4; i++) {
    sys_printf("Testing %s speed mode...\n", speed_names[i]);

    // Set the update speed
    if (!driver_uc8151_set_update_speed(&display, speeds[i])) {
      sys_printf("Failed to set %s speed\n", speed_names[i]);
      continue;
    }

    // Get and display estimated update time
    uint32_t update_time = driver_uc8151_get_update_time(&display);
    sys_printf("Estimated update time: %lu ms\n", update_time);

    // Draw speed comparison bars
    draw_speed_bars();

    // Update display
    sys_printf("Updating display...\n");
    if (driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE)) {
      sys_printf("Display update completed\n");
    } else {
      sys_printf("Display update failed\n");
    }

    // Wait between tests
    sys_sleep(2000); // 2 seconds
  }
}

/**
 * @brief Test partial updates.
 */
void test_partial_updates(void) {
  sys_printf("\n=== Testing Partial Updates ===\n");

  // Set to fast mode for partial updates
  driver_uc8151_set_update_speed(&display, DRIVER_UC8151_UPDATE_SPEED_FAST);

  // Clear display first
  sys_printf("Clearing display for partial update test...\n");
  clear_framebuffer();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(2000); // 2 seconds

  // Test 1: Simple partial update in center
  sys_printf("Test 1: Center square partial update\n");
  driver_uc8151_rect_t center_region = {DISPLAY_WIDTH / 2 - 25,
                                        DISPLAY_HEIGHT / 2 - 25, 50, 50};

  // Draw a filled rectangle in the center
  draw_filled_rect(center_region.x, center_region.y, center_region.width,
                   center_region.height, 1);

  sys_printf("Partial update: x=%d, y=%d, w=%d, h=%d\n", center_region.x,
             center_region.y, center_region.width, center_region.height);

  if (driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                   center_region)) {
    sys_printf("Center partial update completed\n");
  } else {
    sys_printf("Center partial update failed\n");
  }
  sys_sleep(3000);

  // Test 2: Multiple corner partial updates
  driver_uc8151_rect_t corner_regions[] = {
      {10, 10, 40, 40},                                  // Top-left
      {DISPLAY_WIDTH - 50, 10, 40, 40},                  // Top-right
      {10, DISPLAY_HEIGHT - 50, 40, 40},                 // Bottom-left
      {DISPLAY_WIDTH - 50, DISPLAY_HEIGHT - 50, 40, 40}, // Bottom-right
  };

  for (int i = 0; i < 4; i++) {
    sys_printf("Test %d: Corner partial update %d\n", i + 2, i + 1);

    // Draw a filled rectangle in the corner
    draw_filled_rect(corner_regions[i].x, corner_regions[i].y,
                     corner_regions[i].width, corner_regions[i].height, 1);

    sys_printf("Partial update: x=%d, y=%d, w=%d, h=%d\n", corner_regions[i].x,
               corner_regions[i].y, corner_regions[i].width,
               corner_regions[i].height);

    if (driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                     corner_regions[i])) {
      sys_printf("Corner %d partial update completed\n", i + 1);
    } else {
      sys_printf("Corner %d partial update failed\n", i + 1);
    }
    sys_sleep(2000);
  }

  // Test 3: Horizontal strip partial update
  sys_printf("Test 6: Horizontal strip partial update\n");
  driver_uc8151_rect_t h_strip = {20, DISPLAY_HEIGHT / 2 - 10,
                                  DISPLAY_WIDTH - 40, 20};

  draw_filled_rect(h_strip.x, h_strip.y, h_strip.width, h_strip.height, 1);

  sys_printf("Partial update: x=%d, y=%d, w=%d, h=%d\n", h_strip.x, h_strip.y,
             h_strip.width, h_strip.height);

  if (driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                   h_strip)) {
    sys_printf("Horizontal strip partial update completed\n");
  } else {
    sys_printf("Horizontal strip partial update failed\n");
  }
  sys_sleep(3000);

  // Test 4: Vertical strip partial update
  sys_printf("Test 7: Vertical strip partial update\n");
  driver_uc8151_rect_t v_strip = {DISPLAY_WIDTH / 2 - 10, 20, 20,
                                  DISPLAY_HEIGHT - 40};

  draw_filled_rect(v_strip.x, v_strip.y, v_strip.width, v_strip.height, 1);

  sys_printf("Partial update: x=%d, y=%d, w=%d, h=%d\n", v_strip.x, v_strip.y,
             v_strip.width, v_strip.height);

  if (driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                   v_strip)) {
    sys_printf("Vertical strip partial update completed\n");
  } else {
    sys_printf("Vertical strip partial update failed\n");
  }
  sys_sleep(3000);

  sys_printf("=== Partial Update Tests Complete ===\n");
}

/**
 * @brief Test rotation modes.
 */
void test_rotation(void) {
  sys_printf("\n=== Testing Rotation ===\n");

  driver_uc8151_rotation_t rotations[] = {DRIVER_UC8151_ROTATION_0,
                                          DRIVER_UC8151_ROTATION_180};
  const char *rotation_names[] = {"0 degrees", "180 degrees"};

  for (int i = 0; i < 2; i++) {
    sys_printf("Testing %s rotation...\n", rotation_names[i]);

    // Set rotation
    if (!driver_uc8151_set_rotation(&display, rotations[i])) {
      sys_printf("Failed to set rotation\n");
      continue;
    }

    // Draw test pattern
    draw_test_pattern();

    // Update display
    if (driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE)) {
      sys_printf("Rotation test completed\n");
    } else {
      sys_printf("Display update failed\n");
    }

    sys_sleep(3000); // 3 seconds
  }
}

/**
 * @brief Test color inversion.
 */
void test_inversion(void) {
  sys_printf("\n=== Testing Color Inversion ===\n");

  bool inversion_modes[] = {false, true};
  const char *inversion_names[] = {"Normal", "Inverted"};

  for (int i = 0; i < 2; i++) {
    sys_printf("Testing %s colors...\n", inversion_names[i]);

    // Set inversion
    if (!driver_uc8151_set_inverted(&display, inversion_modes[i])) {
      sys_printf("Failed to set color inversion\n");
      continue;
    }

    // Draw test pattern
    draw_test_pattern();

    // Update display
    if (driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE)) {
      sys_printf("Inversion test completed\n");
    } else {
      sys_printf("Display update failed\n");
    }

    sys_sleep(3000); // 3 seconds
  }
}

/**
 * @brief Demonstrate animation with partial updates.
 */
void demo_animation(void) {
  sys_printf("\n=== Animation Demo ===\n");

  // Set to turbo mode for faster updates
  driver_uc8151_set_update_speed(&display, DRIVER_UC8151_UPDATE_SPEED_TURBO);

  // Clear display
  clear_framebuffer();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);

  // Animate a moving square
  uint16_t square_size = 20;
  uint16_t path_width = DISPLAY_WIDTH - square_size - 20;
  uint16_t y_pos = DISPLAY_HEIGHT / 2 - square_size / 2;

  for (int frame = 0; frame < 20; frame++) {
    uint16_t x_pos = 10 + (frame * path_width) / 19;

    sys_printf("Animation frame %d: square at x=%d\n", frame + 1, x_pos);

    // Clear previous frame (simple approach - clear all)
    clear_framebuffer();

    // Draw the square at new position
    draw_filled_rect(x_pos, y_pos, square_size, square_size, 1);

    // Draw path
    draw_hline(10, y_pos + square_size + 5, path_width, 1);

    // Update the region containing the animation
    driver_uc8151_rect_t update_region = {0, y_pos - 5, DISPLAY_WIDTH,
                                          square_size + 15};

    if (!driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE,
                                      update_region)) {
      sys_printf("Animation frame %d failed\n", frame + 1);
    }

    sys_sleep(500); // 0.5 seconds between frames
  }
}

///////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION

int main(void) {
  sys_printf("UC8151 E-Paper Display Test Pattern Gallery\n");
  sys_printf("==========================================\n");

  // Initialize system
  sys_init();
  hw_init();

  // Initialize display
  if (!init_display()) {
    sys_printf("Display initialization failed\n");
    return 1;
  }

  sys_printf("\n=== Display Configuration ===\n");
  sys_printf("Display dimensions: %dx%d pixels\n", DISPLAY_WIDTH,
             DISPLAY_HEIGHT);
  sys_printf("Framebuffer size: %d bytes\n", FRAMEBUFFER_SIZE);
  sys_printf("Expected size: %d bytes\n", (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8);
  sys_printf("Bytes per column: %d\n", DISPLAY_HEIGHT / 8);

  // Set to TURBO speed for faster testing
  sys_printf("\nSetting update speed to TURBO for faster testing...\n");
  if (!driver_uc8151_set_update_speed(&display,
                                      DRIVER_UC8151_UPDATE_SPEED_TURBO)) {
    sys_printf("Failed to set update speed\n");
    return 1;
  }

  sys_printf("\n=== Pattern Gallery ===\n");

  // Pattern 1: All black
  sys_printf("Pattern 1: All black\n");
  memset(framebuffer, 0xFF, FRAMEBUFFER_SIZE);
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(2000);

  // Pattern 2: All white
  sys_printf("Pattern 2: All white\n");
  memset(framebuffer, 0x00, FRAMEBUFFER_SIZE);
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(2000);

  // Pattern 3: Checkerboard (large squares)
  sys_printf("Pattern 3: Large checkerboard\n");
  draw_checkerboard(16);
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 4: Checkerboard (small squares)
  sys_printf("Pattern 4: Small checkerboard\n");
  draw_checkerboard(8);
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 5: Concentric rectangles
  sys_printf("Pattern 5: Concentric rectangles\n");
  draw_concentric_rectangles();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 6: Diagonal stripes (wide)
  sys_printf("Pattern 6: Wide diagonal stripes\n");
  draw_diagonal_stripes(12);
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 7: Diagonal stripes (narrow)
  sys_printf("Pattern 7: Narrow diagonal stripes\n");
  draw_diagonal_stripes(6);
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 8: Gradient bars with dithering
  sys_printf("Pattern 8: Gradient bars (dithered)\n");
  draw_gradient_bars();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 9: Border with crosshair
  sys_printf("Pattern 9: Border with crosshair\n");
  draw_border_crosshair();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 10: Text simulation
  sys_printf("Pattern 10: Text simulation\n");
  draw_text_simulation();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 11: Speed bars demo
  sys_printf("Pattern 11: Speed comparison bars\n");
  draw_speed_bars();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 12: Alternating pattern
  sys_printf("Pattern 12: Alternating 0xAA pattern\n");
  memset(framebuffer, 0xAA, FRAMEBUFFER_SIZE); // 10101010 pattern
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Pattern 13: Inverted alternating pattern
  sys_printf("Pattern 13: Inverted alternating 0x55 pattern\n");
  memset(framebuffer, 0x55, FRAMEBUFFER_SIZE); // 01010101 pattern
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);
  sys_sleep(3000);

  // Final clear
  sys_printf("Final: Clearing display\n");
  clear_framebuffer();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);

  sys_printf("\n=== Pattern Gallery Complete! ===\n");
  sys_printf("Total patterns displayed: 13\n");

  // Now test partial updates
  sys_printf("\n=== Testing Partial Updates ===\n");
  test_partial_updates();

  // Test animation with partial updates
  sys_printf("\n=== Testing Animation ===\n");
  demo_animation();

  // Test update speeds
  sys_printf("\n=== Testing Update Speeds ===\n");
  test_update_speeds();

  // Final clear
  sys_printf("\nFinal cleanup: Clearing display\n");
  clear_framebuffer();
  driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);

  // Cleanup
  sys_printf("\nCleaning up...\n");
  driver_uc8151_finalize(&display);
  hw_spi_finalize(&spi);
  hw_exit();
  sys_exit();
  return 0;
}
