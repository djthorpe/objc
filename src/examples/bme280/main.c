/**
 * @file main.c
 * @brief An example of reading temperature, pressure, and humidity
 *
 * This example demonstrates how to read data from the BME280 sensor
 * and print it to the console.
 */
#include <drivers/drivers.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

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

  sys_printf("BME280 initialized successfully\n");

  // Finalize
  driver_bme280_finalize(&bme280);
  hw_i2c_finalize(&i2c);

  // Cleanup and exit
  hw_exit();
  sys_exit();
  return 0;
}
