/**
 * @file main.c
 * @brief An example of reading and writing BME280 operating modes and sensor
 * data
 *
 * This example demonstrates how to read and set the operating mode of the
 * BME280 sensor, and how to read temperature, pressure, and humidity data.
 * It shows how to switch between SLEEP, FORCED, and NORMAL modes and
 * demonstrates data reading in each mode.
 */
#include <drivers/drivers.h>
#include <math.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

// Temporary float printing function since sys_printf doesn't support %f
void putf(const char *label, float value, const char *unit) {
  int whole = (int)value;
  int decimal = (int)((value - whole) * 100);
  if (decimal < 0)
    decimal = -decimal; // Handle negative decimals
  sys_printf("%s%d.%02d%s\n", label, whole, decimal, unit ? unit : "");
}

// Helper function to print current mode
void print_mode(driver_bme280_t *bme280) {
  driver_bme280_mode_t mode = driver_bme280_get_mode(bme280);
  switch (mode) {
  case DRIVER_BME280_MODE_SLEEP:
    sys_printf("BME280 is in SLEEP mode\n");
    break;
  case DRIVER_BME280_MODE_FORCED:
    sys_printf("BME280 is in FORCED mode\n");
    break;
  case DRIVER_BME280_MODE_NORMAL:
    sys_printf("BME280 is in NORMAL mode\n");
    break;
  default:
    sys_printf("BME280 is in UNKNOWN mode (%d)\n", mode);
    break;
  }
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();
  hw_init();

  // Get default I2C interface for BME280
  hw_i2c_t i2c = hw_i2c_init_default(100000); // 100kHz I2C speed
  if (!hw_i2c_valid(&i2c)) {
    sys_printf("Failed to initialize I2C interface\n");
    return -1;
  }

  // Get the BME280 driver instance with default address
  driver_bme280_t bme280 = driver_bme280_i2c_init(&i2c, 0);
  sys_assert(driver_bme280_valid(&bme280));

  // Read the initial operating mode
  sys_printf("Reading initial BME280 operating mode:\n");
  print_mode(&bme280);

  // Demonstrate setting the sensor to FORCED mode and reading data
  sys_printf("\nSetting BME280 to FORCED mode and reading data...\n");
  if (driver_bme280_set_mode(&bme280, DRIVER_BME280_MODE_FORCED)) {
    sys_printf("Successfully set to FORCED mode\n");

    // Read sensor data
    float temperature, pressure, humidity;
    if (driver_bme280_read_data(&bme280, &temperature, &pressure, &humidity)) {
      putf("Temperature: ", temperature, "°C");
      putf("Pressure: ", pressure, " Pa");
      putf("Humidity: ", humidity, "%");
    } else {
      sys_printf("Failed to read sensor data\n");
    }

    sys_printf("Mode after forced measurement:\n");
    print_mode(&bme280);
  } else {
    sys_printf("Failed to set FORCED mode\n");
  }

  // Demonstrate setting the sensor to NORMAL mode
  sys_printf("\nSetting BME280 to NORMAL mode...\n");
  if (driver_bme280_set_mode(&bme280, DRIVER_BME280_MODE_NORMAL)) {
    sys_printf("Successfully set to NORMAL mode\n");
    print_mode(&bme280);

    // Read a few measurements in normal mode
    for (int i = 0; i < 3; i++) {
      sys_sleep(1000); // Wait 1 second between readings

      float temperature, pressure, humidity;
      if (driver_bme280_read_data(&bme280, &temperature, &pressure,
                                  &humidity)) {
        sys_printf("Reading %d:\n", i + 1);
        putf("  Temperature: ", temperature, "°C");
        putf("  Pressure: ", pressure, " Pa");
        putf("  Humidity: ", humidity, "%");
      } else {
        sys_printf("Reading %d - Failed to read sensor data\n", i + 1);
      }
    }
  } else {
    sys_printf("Failed to set NORMAL mode\n");
  }

  // Demonstrate setting the sensor to SLEEP mode
  sys_printf("\nSetting BME280 to SLEEP mode...\n");
  if (driver_bme280_set_mode(&bme280, DRIVER_BME280_MODE_SLEEP)) {
    sys_sleep(500); // Wait for mode change to take effect
    print_mode(&bme280);
  } else {
    sys_printf("Failed to set SLEEP mode\n");
  }

  // Test new API functions
  sys_printf("\n=== Testing Additional API Functions ===\n");

  // Test individual sensor reading functions
  sys_printf("\nTesting individual sensor readings...\n");

  float temp_only = driver_bme280_read_temperature(&bme280);
  if (!isnan(temp_only)) {
    putf("Temperature only: ", temp_only, "°C");
  } else {
    sys_printf("Failed to read temperature\n");
  }

  float pressure_only = driver_bme280_read_pressure(&bme280);
  if (!isnan(pressure_only)) {
    putf("Pressure only: ", pressure_only, " Pa");
  } else {
    sys_printf("Failed to read pressure\n");
  }

  float humidity_only = driver_bme280_read_humidity(&bme280);
  if (!isnan(humidity_only)) {
    putf("Humidity only: ", humidity_only, "%");
  } else {
    sys_printf("Failed to read humidity\n");
  }

  // Test altitude calculations
  sys_printf("\nTesting altitude calculations...\n");
  if (!isnan(pressure_only)) {
    float altitude = driver_bme280_calculate_altitude(
        pressure_only, 0.0f); // Use default sea level pressure
    putf("Calculated altitude (standard atmosphere): ", altitude, " meters");

    // Test with a known altitude assumption
    float test_altitude = 1000.0f; // 1000 meters
    float calculated_sea_level = driver_bme280_calculate_sea_level_pressure(
        pressure_only, test_altitude);
    putf("If current altitude is 1000m, sea level pressure would be: ",
         calculated_sea_level, " Pa");

    float recalculated_altitude =
        driver_bme280_calculate_altitude(pressure_only, calculated_sea_level);
    putf("Recalculated altitude from that sea level pressure: ",
         recalculated_altitude, " meters");
  } else {
    sys_printf(
        "Cannot test altitude calculations - no valid pressure reading\n");
  }

  // Finalize
  driver_bme280_finalize(&bme280);
  hw_i2c_finalize(&i2c);

  // Cleanup and exit
  hw_exit();
  sys_exit();
  return 0;
}
