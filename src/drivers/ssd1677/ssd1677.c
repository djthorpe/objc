/**
 * @file ssd1677.c
 * @brief SSD1677 E-Paper Display Controller Driver Implementation
 */
#include <drivers/drivers_ssd1677.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// INTERNAL STRUCTURE

typedef struct {
  hw_spi_t spi;     ///< SPI interface
  hw_gpio_t dc;     ///< Data/Command pin
  hw_gpio_t reset;  ///< Reset pin
  hw_gpio_t busy;   ///< Busy pin
  uint16_t width;   ///< Display width in pixels
  uint16_t height;  ///< Display height in pixels
  bool is_sleeping; ///< Sleep state flag
} ssd1677_internal_t;

// Static assert to ensure internal structure fits in reserved space
_Static_assert(sizeof(ssd1677_internal_t) <= sizeof(driver_ssd1677_t),
               "ssd1677_internal_t too large for driver_ssd1677_t");

// Macro to access internal structure
#define SSD1677_INTERNAL(display) ((ssd1677_internal_t *)(display))

///////////////////////////////////////////////////////////////////////////////
// CONSTANTS

// SSD1677 Commands
#define SSD1677_CMD_SW_RESET 0x12
#define SSD1677_CMD_TEMP_SENSOR_CONTROL 0x46
#define SSD1677_CMD_TEMP_SENSOR_LOAD 0x47
#define SSD1677_CMD_GATE_SETTING 0x01
#define SSD1677_CMD_GATE_VOLTAGE 0x03
#define SSD1677_CMD_SOURCE_VOLTAGE 0x04
#define SSD1677_CMD_DATA_ENTRY_MODE 0x11
#define SSD1677_CMD_BORDER_WAVEFORM 0x3C
#define SSD1677_CMD_BOOSTER_SETTING 0x0C
#define SSD1677_CMD_TEMP_SENSOR_INTERNAL 0x18
#define SSD1677_CMD_VCOM_VALUE 0x2C
#define SSD1677_CMD_DISPLAY_OPTION 0x37
#define SSD1677_CMD_SET_RAM_X_RANGE 0x44
#define SSD1677_CMD_SET_RAM_Y_RANGE 0x45
#define SSD1677_CMD_SET_RAM_X_COUNTER 0x4E
#define SSD1677_CMD_SET_RAM_Y_COUNTER 0x4F
#define SSD1677_CMD_WRITE_RAM_BLACK 0x24
#define SSD1677_CMD_WRITE_RAM_RED 0x26
#define SSD1677_CMD_DISPLAY_UPDATE_CTRL2 0x22
#define SSD1677_CMD_DISPLAY_UPDATE 0x20
#define SSD1677_CMD_DEEP_SLEEP 0x10
#define SSD1677_CMD_LOAD_LUT 0x32

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

// Internal control functions
static void ssd1677_reset_internal(driver_ssd1677_t *display);
static void ssd1677_wake_internal(driver_ssd1677_t *display);
static void ssd1677_ensure_awake(driver_ssd1677_t *display);
static void ssd1677_turn_on_display(driver_ssd1677_t *display);
static void ssd1677_wait_until_idle(driver_ssd1677_t *display);

// Helper functions to reduce code duplication
static size_t ssd1677_get_buffer_size(ssd1677_internal_t *internal);
static uint16_t ssd1677_get_y_start_position(ssd1677_internal_t *internal);
static void ssd1677_reset_ram_counters(driver_ssd1677_t *display, uint16_t x,
                                       uint16_t y);
static void ssd1677_send_display_command(driver_ssd1677_t *display,
                                         uint8_t mode);

/**
 * @brief Send a command to SSD1677
 * @param display Pointer to display structure
 * @param command Command byte to send
 */
static void ssd1677_send_command(driver_ssd1677_t *display, uint8_t command) {
  sys_assert(display);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Set DC pin low for command mode
  hw_gpio_set(&internal->dc, false);

  // Send command via SPI
  hw_spi_xfr(&internal->spi, &command, 1, 0);
}

/**
 * @brief Send data to SSD1677
 * @param display Pointer to display structure
 * @param data Data byte to send
 */
static void ssd1677_send_data(driver_ssd1677_t *display, uint8_t data) {
  sys_assert(display);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Set DC pin high for data mode
  hw_gpio_set(&internal->dc, true);

  // Send data via SPI
  hw_spi_xfr(&internal->spi, &data, 1, 0);
}

/**
 * @brief Send multiple data bytes to SSD1677
 * @param display Pointer to display structure
 * @param data Pointer to data buffer
 * @param len Number of bytes to send
 */
static void ssd1677_send_data_buffer(driver_ssd1677_t *display,
                                     const uint8_t *data, size_t len) {
  sys_assert(display);
  sys_assert(data || len == 0);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  if (len == 0)
    return;

  // Set DC pin high for data mode
  hw_gpio_set(&internal->dc, true);

  // Send data in 4K chunks for efficiency
  const size_t chunk_size = 4096;
  size_t bytes_sent = 0;

  while (bytes_sent < len) {
    size_t remaining = len - bytes_sent;
    size_t current_chunk = (remaining > chunk_size) ? chunk_size : remaining;

    size_t transferred = hw_spi_xfr(&internal->spi, (void *)(data + bytes_sent),
                                    current_chunk, 0);

    if (transferred != current_chunk) {
      break; // Transfer failed
    }

    bytes_sent += transferred;
  }
}

/**
 * @brief Set RAM address window
 * @param display Pointer to display structure
 * @param x_start Starting X coordinate
 * @param y_start Starting Y coordinate
 * @param x_end Ending X coordinate
 * @param y_end Ending Y coordinate
 */
static void ssd1677_set_memory_area(driver_ssd1677_t *display, uint16_t x_start,
                                    uint16_t y_start, uint16_t x_end,
                                    uint16_t y_end) {
  sys_assert(display);

  // Convert pixel coordinates to controller address units (bytes)
  uint16_t x_start_addr = x_start / 8;
  uint16_t x_end_addr = x_end / 8;

  // Set X range
  ssd1677_send_command(display, SSD1677_CMD_SET_RAM_X_RANGE);
  ssd1677_send_data(display, x_start_addr & 0xFF);
  ssd1677_send_data(display, (x_start_addr >> 8) & 0x03);
  ssd1677_send_data(display, x_end_addr & 0xFF);
  ssd1677_send_data(display, (x_end_addr >> 8) & 0x03);

  // Set Y range
  ssd1677_send_command(display, SSD1677_CMD_SET_RAM_Y_RANGE);
  ssd1677_send_data(display, y_start & 0xFF);
  ssd1677_send_data(display, (y_start >> 8) & 0x03);
  ssd1677_send_data(display, y_end & 0xFF);
  ssd1677_send_data(display, (y_end >> 8) & 0x03);
}

/**
 * @brief Set RAM address pointer
 * @param display Pointer to display structure
 * @param x X coordinate
 * @param y Y coordinate
 */
static void ssd1677_set_memory_pointer(driver_ssd1677_t *display, uint16_t x,
                                       uint16_t y) {
  sys_assert(display);

  uint16_t x_addr = x / 8;

  // Set X counter (2 bytes per spec Table 8-7)
  ssd1677_send_command(display, SSD1677_CMD_SET_RAM_X_COUNTER);
  ssd1677_send_data(display, x_addr & 0xFF);        // XAD[7:0]
  ssd1677_send_data(display, (x_addr >> 8) & 0x03); // XAD[9:8]

  // Set Y counter (2 bytes per spec Table 8-7)
  ssd1677_send_command(display, SSD1677_CMD_SET_RAM_Y_COUNTER);
  ssd1677_send_data(display, y & 0xFF);        // YAD[7:0]
  ssd1677_send_data(display, (y >> 8) & 0x03); // YAD[9:8]
}

///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

/**
 * @brief Calculate buffer size for display image data
 * @param internal Pointer to internal structure
 * @return Buffer size in bytes
 */
static size_t ssd1677_get_buffer_size(ssd1677_internal_t *internal) {
  uint16_t width_bytes = (internal->width + 7) / 8;
  return width_bytes * internal->height;
}

/**
 * @brief Get Y start position (top of display)
 * @param internal Pointer to internal structure
 * @return Y start position
 */
static uint16_t ssd1677_get_y_start_position(ssd1677_internal_t *internal) {
  (void)internal;
  return 0; // Controller expects Y counter reset to first row before writes
}

/**
 * @brief Reset RAM counters to specified position
 * @param display Pointer to display structure
 * @param x X coordinate
 * @param y Y coordinate
 */
static void ssd1677_reset_ram_counters(driver_ssd1677_t *display, uint16_t x,
                                       uint16_t y) {
  uint16_t x_addr = x / 8;

  // Reset X counter
  ssd1677_send_command(display, SSD1677_CMD_SET_RAM_X_COUNTER);
  ssd1677_send_data(display, x_addr & 0xFF);        // X counter low byte
  ssd1677_send_data(display, (x_addr >> 8) & 0x03); // X counter high byte

  // Reset Y counter
  ssd1677_send_command(display, SSD1677_CMD_SET_RAM_Y_COUNTER);
  ssd1677_send_data(display, y & 0xFF);        // Y counter low byte
  ssd1677_send_data(display, (y >> 8) & 0x03); // Y counter high byte
}

/**
 * @brief Send display update command sequence
 * @param display Pointer to display structure
 * @param mode Display mode command (0x91, 0xC7, 0xF7, etc.)
 */
static void ssd1677_send_display_command(driver_ssd1677_t *display,
                                         uint8_t mode) {
  ssd1677_send_command(display, SSD1677_CMD_DISPLAY_UPDATE_CTRL2);
  ssd1677_send_data(display, mode);
  ssd1677_send_command(display, SSD1677_CMD_DISPLAY_UPDATE);
  ssd1677_wait_until_idle(display);
}

/**
 * @brief Turn on display with standard update
 * @param display Pointer to display structure
 */
static void ssd1677_turn_on_display(driver_ssd1677_t *display) {
  sys_assert(display);

  // Ensure display is awake
  ssd1677_ensure_awake(display);

  // Step 1: Load temperature and waveform settings from OTP (0xB1)
  ssd1677_send_display_command(display, 0xB1);

  // Step 2: Trigger a full display refresh (0xF7)
  ssd1677_send_display_command(display, 0xF7);
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

bool driver_ssd1677_init(driver_ssd1677_t *display, hw_spi_t *spi,
                         uint16_t width, uint16_t height, uint8_t dc_pin,
                         uint8_t reset_pin, uint8_t busy_pin) {
  if (!display || !spi)
    return false;

  // Clear display structure
  sys_memset(display, 0, sizeof(driver_ssd1677_t));
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Copy SPI interface
  internal->spi = *spi;

  // Initialize GPIO pins
  internal->dc = hw_gpio_init(0, dc_pin, HW_GPIO_OUTPUT);
  internal->reset = hw_gpio_init(0, reset_pin, HW_GPIO_OUTPUT);
  internal->busy = hw_gpio_init(0, busy_pin, HW_GPIO_INPUT);

  if (!hw_gpio_valid(&internal->dc) || !hw_gpio_valid(&internal->reset) ||
      !hw_gpio_valid(&internal->busy)) {
    driver_ssd1677_finalize(display);
    return false;
  }

  // Set display dimensions
  internal->width = width;
  internal->height = height;

  // Initialize sleep state
  internal->is_sleeping = false;

  // Perform hardware reset and initialization
  ssd1677_reset_internal(display);
  ssd1677_wake_internal(display);

  return true;
}

void driver_ssd1677_finalize(driver_ssd1677_t *display) {
  if (!display)
    return;
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  if (hw_spi_valid(&internal->spi)) {
    hw_spi_finalize(&internal->spi);
  }

  if (hw_gpio_valid(&internal->dc)) {
    hw_gpio_finalize(&internal->dc);
  }

  if (hw_gpio_valid(&internal->reset)) {
    hw_gpio_finalize(&internal->reset);
  }

  if (hw_gpio_valid(&internal->busy)) {
    hw_gpio_finalize(&internal->busy);
  }

  sys_memset(display, 0, sizeof(driver_ssd1677_t));
}

static void ssd1677_reset_internal(driver_ssd1677_t *display) {
  sys_assert(display);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Hardware reset sequence
  hw_gpio_set(&internal->reset, true);
  sys_sleep(20);
  hw_gpio_set(&internal->reset, false);
  sys_sleep(2);
  hw_gpio_set(&internal->reset, true);
  sys_sleep(20);
}

static void ssd1677_wait_until_idle(driver_ssd1677_t *display) {
  sys_assert(display);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Check initial BUSY pin state
  bool initial_busy = hw_gpio_get(&internal->busy);

  if (!initial_busy) {
    sys_sleep(200); // Small delay after BUSY low
    return;
  }

  // Wait while busy pin is high, with timeout
  uint32_t timeout_ms = 30000; // 30 second timeout for e-paper refresh
  uint32_t elapsed_ms = 0;
  uint32_t last_report = 0;

  while (hw_gpio_get(&internal->busy) && elapsed_ms < timeout_ms) {
    sys_sleep(10);
    elapsed_ms += 10;

    // Report progress every 5 seconds (reduced frequency)
    if (elapsed_ms - last_report >= 5000) {
      sys_printf("SSD1677: Waiting for BUSY... %d ms\n", elapsed_ms);
      last_report = elapsed_ms;
    }
  }

  if (elapsed_ms >= timeout_ms) {
    sys_printf("WARNING: SSD1677 BUSY timeout (%d ms)\n", timeout_ms);
  } else {
    sys_sleep(200); // Small delay after BUSY low
  }
}

void driver_ssd1677_sleep(driver_ssd1677_t *display) {
  sys_assert(display);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Don't sleep if already sleeping
  if (internal->is_sleeping) {
    return;
  }
  ssd1677_send_command(display, SSD1677_CMD_DEEP_SLEEP);
  ssd1677_send_data(display,
                    0x01); // Deep sleep mode 1 (spec says 0x01, was 0x03)

  // Mark as sleeping
  internal->is_sleeping = true;
}

static void ssd1677_wake_internal(driver_ssd1677_t *display) {
  sys_assert(display);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Software reset first
  ssd1677_send_command(display, 0x12);
  ssd1677_wait_until_idle(display);

  // Start with commands 0x74, 0x7E (from spec reference)
  ssd1677_send_command(display, 0x74);
  ssd1677_send_data(display, 0x54);
  ssd1677_send_command(display, 0x7E);
  ssd1677_send_data(display, 0x3B);

  // Set MUX (Driver Output Control) based on display height
  uint16_t mux_value = internal->height - 1;

  // Validate height is within SSD1677 limits
  if (internal->height < 300 || internal->height > 680) {
    sys_printf("WARNING: Display height %d outside range (300-680)\n",
               internal->height);
  }

  ssd1677_send_command(display, 0x01);
  ssd1677_send_data(display, mux_value & 0xFF);
  ssd1677_send_data(display, (mux_value >> 8) & 0x03);
  ssd1677_send_data(display, 0x00);

  // Data entry mode: Y+ then X- (controller default for this panel).
  // The display expects X to decrement across a row and Y to increment
  // between rows when streaming data from RAM.
  ssd1677_send_command(display, 0x11);
  ssd1677_send_data(display, 0x01); // Y+,X- mode

  // Border waveform control - critical for proper display operation
  ssd1677_send_command(display, 0x3C);
  ssd1677_send_data(display, 0x05); // Border follows LUT1 with different timing

  // Calculate X range in controller address units (bytes of 8 pixels)
  uint16_t width_bytes = (internal->width + 7) / 8;
  uint16_t x_max = width_bytes - 1; // e.g., 110 bytes -> max address 109

  // Set RAM X address range
  ssd1677_send_command(display, 0x44);
  ssd1677_send_data(display, 0x00);         // X start low = 0 bytes
  ssd1677_send_data(display, 0x00);         // X start high = 0
  ssd1677_send_data(display, x_max & 0xFF); // X end low
  ssd1677_send_data(display, (x_max >> 8) & 0x03);

  // Set RAM Y address range based on actual display height (528 pixels)
  // Start from last row and wrap through 0 as per datasheet examples.
  uint16_t y_max = internal->height - 1; // 527 for 528-pixel display

  ssd1677_send_command(display, 0x45);
  ssd1677_send_data(display, y_max & 0xFF);
  ssd1677_send_data(display, (y_max >> 8) & 0x03);
  ssd1677_send_data(display, 0x00);
  ssd1677_send_data(display, 0x00);

  // Temperature sensor initialization
  ssd1677_send_command(display, 0x1A);
  ssd1677_send_data(display, 0x64);
  ssd1677_send_data(display, 0x00);

  // Set RAM counters to start position (X=0, Y=0)
  ssd1677_reset_ram_counters(display, 0, 0);

  // Mark as awake
  internal->is_sleeping = false;
}

/**
 * @brief Ensure display is awake before operations
 * @param display Pointer to display structure
 */
static void ssd1677_ensure_awake(driver_ssd1677_t *display) {
  sys_assert(display);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  if (internal->is_sleeping) {
    ssd1677_wake_internal(display);
  }
}

void driver_ssd1677_clear(driver_ssd1677_t *display) {
  sys_assert(display);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Ensure display is awake
  ssd1677_ensure_awake(display);

  // Reset address window to full screen before streaming data
  uint16_t x_end = internal->width - 1;
  uint16_t y_end = internal->height - 1;
  ssd1677_set_memory_area(display, 0, 0, x_end, y_end);

  // Calculate buffer size and get start position
  size_t buffer_size = ssd1677_get_buffer_size(internal);
  uint16_t y_start = ssd1677_get_y_start_position(internal);

  // Reset RAM counters to start position
  ssd1677_reset_ram_counters(display, 0, y_start);

  // Create buffer for actual display size
  uint8_t *clear_buffer = sys_malloc(buffer_size);
  if (!clear_buffer) {
    sys_printf("ERROR: Failed to allocate clear buffer\n");
    return;
  }
  sys_memset(clear_buffer, 0xFF, buffer_size);

  // Write to black RAM
  ssd1677_send_command(display, SSD1677_CMD_WRITE_RAM_BLACK);
  ssd1677_send_data_buffer(display, clear_buffer, buffer_size);

  // Reset counters again for red RAM
  ssd1677_reset_ram_counters(display, 0, y_start);

  // Write to red RAM
  ssd1677_send_command(display, SSD1677_CMD_WRITE_RAM_RED);
  ssd1677_send_data_buffer(display, clear_buffer, buffer_size);

  sys_free(clear_buffer);

  ssd1677_turn_on_display(display);
}

void driver_ssd1677_update(driver_ssd1677_t *display, const uint8_t *image_data,
                           bool is_base_image) {
  sys_assert(display);
  sys_assert(image_data);
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  // Ensure display is awake
  ssd1677_ensure_awake(display);

  // Reset address window to full screen before streaming data
  uint16_t x_end = internal->width - 1;
  uint16_t y_end = internal->height - 1;
  ssd1677_set_memory_area(display, 0, 0, x_end, y_end);

  // Align the RAM pointer with the refreshed address window
  ssd1677_set_memory_pointer(display, 0, 0);

  // Calculate buffer size and get start position
  size_t expected_size = ssd1677_get_buffer_size(internal);
  uint16_t y_start = ssd1677_get_y_start_position(internal);

  // Reset RAM counters to start position
  ssd1677_reset_ram_counters(display, 0, y_start);

  // Write to black RAM (use the provided image data directly)
  ssd1677_send_command(display, SSD1677_CMD_WRITE_RAM_BLACK);
  ssd1677_send_data_buffer(display, image_data, expected_size);

  // If base image, also write to red RAM for differential updates
  if (is_base_image) {
    // Reset counters for red RAM
    ssd1677_reset_ram_counters(display, 0, y_start);

    ssd1677_send_command(display, SSD1677_CMD_WRITE_RAM_RED);
    ssd1677_send_data_buffer(display, image_data, expected_size);
  }

  // Turn on display (will load OTP LUT and update)
  ssd1677_turn_on_display(display);
}

void driver_ssd1677_update_partial(driver_ssd1677_t *display,
                                   const uint8_t *image_data, uint16_t x_start,
                                   uint16_t y_start, uint16_t x_end,
                                   uint16_t y_end, bool use_gc_mode) {
  sys_assert(display);
  sys_assert(image_data);
  (void)use_gc_mode; // Suppress unused parameter warning

  // Ensure display is awake
  ssd1677_ensure_awake(display);

  // Calculate dimensions
  uint16_t width = ((x_end - x_start) % 8 == 0) ? (x_end - x_start) / 8
                                                : (x_end - x_start) / 8 + 1;
  uint16_t image_size = width * (y_end - y_start);

  // Adjust end coordinates (inclusive to exclusive)
  x_end -= 1;
  y_end -= 1;

  // Set memory area for partial update
  ssd1677_set_memory_area(display, x_start, y_start, x_end, y_end);
  ssd1677_set_memory_pointer(display, x_start, y_start);

  // Write image data
  ssd1677_send_command(display, SSD1677_CMD_WRITE_RAM_BLACK);
  ssd1677_send_data_buffer(display, image_data, image_size);

  // Turn on display (will load OTP LUT and update)
  ssd1677_turn_on_display(display);
}

bool driver_ssd1677_valid(driver_ssd1677_t *display) {
  if (!display)
    return false;
  ssd1677_internal_t *internal = SSD1677_INTERNAL(display);

  return hw_spi_valid(&internal->spi) && hw_gpio_valid(&internal->dc) &&
         hw_gpio_valid(&internal->reset) && hw_gpio_valid(&internal->busy);
}