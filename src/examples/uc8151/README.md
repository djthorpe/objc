# UC8151 E-Paper Display Example

This example demonstrates how to use the UC8151 e-paper display driver with a Raspberry Pi Pico. The UC8151 is a popular controller for monochrome e-ink displays, commonly found in 128x296 pixel displays.

## Hardware Setup

### Pin Connections

Connect your UC8151 e-paper display to the Raspberry Pi Pico using the following pin assignments:

| Display Pin | Pico Pin | Function |
|-------------|----------|----------|
| CS          | 17       | SPI Chip Select |
| CLK         | 18       | SPI Clock |
| MOSI        | 19       | SPI Data Out |
| DC          | 20       | Data/Command Select |
| RESET       | 21       | Reset Pin |
| BUSY        | 26       | Busy Status Pin |
| VCC         | 3.3V     | Power Supply |
| GND         | GND      | Ground |

### Display Specifications

- **Resolution**: 128 x 296 pixels
- **Color**: Monochrome (black/white)
- **Interface**: SPI
- **Controller**: UC8151
- **Framebuffer Size**: 4,736 bytes (128 × 296 ÷ 8)

## Building

### For Raspberry Pi Pico

```bash
# Configure for Pico
CC=clang TARGET=armv6m-none-eabi TOOLCHAIN_PATH=/opt/LLVM-ET-Arm-19.1.5-Darwin-universal make

# Build the example
cmake --build build --target uc8151_example

# Flash to Pico
picotool load -x build/src/examples/uc8151/uc8151_example.uf2
```

### For Development/Testing

```bash
# Configure for local development
make

# Build the example
cmake --build build --target uc8151_example

# Run the example (stub mode)
./build/src/examples/uc8151/uc8151_example
```

## Features Demonstrated

### 1. Display Initialization

- SPI interface setup
- UC8151 controller initialization
- GPIO pin configuration

### 2. Framebuffer Operations

- Pixel-level drawing functions
- Line and rectangle drawing
- Pattern generation

### 3. Update Speed Modes

The example tests all four update speed modes:

- **Default**: ~4.5 seconds (highest quality)
- **Medium**: ~2.0 seconds (good quality)
- **Fast**: ~0.8 seconds (reduced quality)
- **Turbo**: ~0.25 seconds (lowest quality)

### 4. Partial Updates

- Regional display updates for faster refresh
- Animation using partial updates
- Efficient screen region management

### 5. Display Modes

- **Rotation**: 0° and 180° display rotation
- **Inversion**: Normal and inverted color modes
- **Blocking/Non-blocking**: Synchronous and asynchronous updates

### 6. Animation Demo

- Moving square animation using partial updates
- Frame-based animation techniques
- Performance optimization for e-paper displays

## Code Structure

### Drawing Functions

```c
void set_pixel(uint16_t x, uint16_t y, uint8_t color);
void draw_hline(uint16_t x, uint16_t y, uint16_t width, uint8_t color);
void draw_vline(uint16_t x, uint16_t y, uint16_t height, uint8_t color);
void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
void draw_filled_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
```

### Display Operations

```c
// Initialize display
display = driver_uc8151_spi_init(&spi, DC_PIN, RESET_PIN, BUSY_PIN, WIDTH, HEIGHT);

// Configure update speed
driver_uc8151_set_update_speed(&display, DRIVER_UC8151_UPDATE_SPEED_FAST);

// Full display update
driver_uc8151_update(&display, framebuffer, FRAMEBUFFER_SIZE);

// Partial display update
driver_uc8151_partial_update(&display, framebuffer, FRAMEBUFFER_SIZE, region);
```

## Performance Notes

### Update Times

- **Full Updates**: Complete display refresh, slower but highest quality
- **Partial Updates**: Regional refresh, faster but may have ghosting
- **Speed Modes**: Trade-off between update time and display quality

### Best Practices

1. Use partial updates for animations and frequent changes
2. Use faster speed modes for dynamic content
3. Use default speed mode for final display states
4. Minimize full display updates to preserve e-paper lifetime

## Troubleshooting

### Common Issues

1. **Display not responding**
   - Check wiring connections
   - Verify power supply (3.3V)
   - Ensure SPI frequency is within limits (≤10MHz)

2. **Ghosting or incomplete updates**
   - Try using default speed mode
   - Perform a full update before partial updates
   - Check busy pin connection

3. **Build errors**
   - Ensure all dependencies are installed
   - Check target platform configuration
   - Verify include paths

### Debug Output

The example includes comprehensive debug output showing:

- Initialization status
- Update timing measurements
- Test progress and results
- Error conditions and failures

## Extending the Example

### Adding Custom Patterns

Modify the drawing functions to create custom graphics:

```c
void draw_custom_pattern(void) {
    // Your custom drawing code here
    clear_framebuffer();
    // Add your graphics operations
    draw_filled_rect(10, 10, 50, 50, 1);
}
```

### Integrating with Sensors

Combine with other drivers to create dynamic displays:

```c
#include <drivers/drivers_bme280.h>

void display_sensor_data(void) {
    // Read sensor data
    // Format for display
    // Update e-paper display
}
```

### Power Management

For battery-powered applications, add power management:

```c
void sleep_display(void) {
    driver_uc8151_power_off(&display);
    // Enter deep sleep mode
}
```

## Related Documentation

- [UC8151 Driver API](../../../include/drivers/drivers_uc8151.h)
- [Hardware Abstraction Layer](../../../include/runtime-hw/hw.h)
- [SPI Interface Documentation](../../../include/runtime-hw/hw_spi.h)
- [Building Instructions](../../.github/instructions/testing.instructions.md)
