#include <drivers/drivers.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_envirophat_04(void) {
  driver_envirophat_t driver;
  driver_bme280_data_t env_data;

  // Initialize the envirophat driver
  sys_printf("Initializing Enviro pHAT...\n");
  if (!driver_envirophat_init(&driver)) {
    sys_printf("Failed to initialize Enviro pHAT\n");
    return -1;
  }
  sys_printf("Enviro pHAT initialized successfully\n");

  // Scan I2C bus for devices
  sys_printf("\nScanning I2C bus for devices:\n");
  for (uint8_t addr = 0x03; addr < 0x78; addr++) {
    uint8_t dummy;
    if (hw_i2c_read(&driver.i2c, addr, 0x00, &dummy, 1, 0) > 0) {
      sys_printf("  Found device at address 0x%02X\n", addr);
    }
  }
  sys_printf("\n");

  // Check if BME280 is present
  if (!driver_bme280_valid(&driver.bme280)) {
    sys_printf("BME280 sensor not detected\n");
    sys_printf("This test requires a BME280 sensor to be present\n");
    driver_envirophat_finalize(&driver);
    return -1;
  }

  sys_printf("BME280 sensor detected at address 0x%02X\n", driver.bme280.addr);

  // Read and display BME280 environmental data
  sys_printf("\nReading BME280 environmental data (10 samples):\n");
  sys_printf("Sample | Temperature | Pressure  | Humidity | Altitude\n");
  sys_printf("-------|-------------|-----------|----------|--------------------"
             "----------\n");

  for (int i = 0; i < 10; i++) {
    bool env_ok = driver_envirophat_read_environmental(&driver, &env_data);

    if (env_ok) {
      // Calculate altitude assuming standard sea level pressure
      float altitude = driver_bme280_calculate_altitude(&env_data, 101325.0f);

      // Also calculate with typical high pressure (example)
      float altitude_corrected =
          driver_bme280_calculate_altitude(&env_data, 102800.0f);

      printf("  %2d   |   %6.2f°C  | %7.0f Pa | %5.1f %%  | %6.1f m (std) / "
             "%6.1f m (1028hPa)\n",
             i + 1, env_data.temperature, env_data.pressure, env_data.humidity,
             altitude, altitude_corrected);
      fflush(stdout);
    } else {
      sys_printf("  %2d   | Failed to read BME280 data\n", i + 1);
    }

    sys_sleep(1000);
  }

  sys_printf("\n");
  sys_printf("Note: Altitude calculation shows two values:\n");
  sys_printf("      - Standard (1013.25 hPa) - for reference only\n");
  sys_printf(
      "      - Corrected (1028 hPa) - example using high pressure system\n");
  sys_printf("      For accurate altitude, check your local sea level pressure "
             "from weather reports.\n");
  printf("      Your current pressure: %.0f Pa (%.2f hPa)\n\n",
         env_data.pressure, env_data.pressure / 100.0f);
  fflush(stdout);

  // Demonstrate sea level pressure calculation
  // Demonstrate sea level pressure calculation
  sys_printf("Sea level pressure calculation examples:\n");
  if (driver_envirophat_read_environmental(&driver, &env_data)) {
    printf("Current pressure: %.0f Pa (%.2f hPa)\n", env_data.pressure,
           env_data.pressure / 100.0f);

    // Example: If you're at 45m altitude (actual location)
    float sea_level_45m =
        driver_bme280_calculate_sea_level_pressure(&env_data, 45.0f);
    printf("If at 45m altitude, calculated sea level pressure: %.0f Pa (%.2f "
           "hPa)\n",
           sea_level_45m, sea_level_45m / 100.0f);

    // Calculate altitude using the corrected sea level pressure
    float corrected_altitude =
        driver_bme280_calculate_altitude(&env_data, sea_level_45m);
    printf("Verification: altitude with corrected pressure: %.1f m\n",
           corrected_altitude);

    printf("(Should be close to 45m if calculations are correct)\n");

    fflush(stdout);
  }

  sys_printf("\n");

  // Finalize the driver
  sys_printf("Finalizing Enviro pHAT...\n");
  driver_envirophat_finalize(&driver);
  sys_printf("Test completed successfully\n");

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  return TestHardwareMain("test_envirophat_04", test_envirophat_04);
}
