#include <drivers/drivers.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_envirophat_03(void) {
  driver_envirophat_t driver;
  driver_lsm303_vec3_t accel, mag;
  float temp;

  // Initialize the envirophat driver
  sys_printf("Initializing Enviro pHAT...\n");
  if (!driver_envirophat_init(&driver)) {
    sys_printf("Failed to initialize Enviro pHAT\n");
    return -1;
  }
  sys_printf("Enviro pHAT initialized successfully\n");

  // Debug: Read WHO_AM_I and status registers
  uint8_t who_am_i, status_a, status_m, ctrl1, ctrl2, ctrl5, ctrl6, ctrl7;
  uint8_t accel_raw[6], mag_raw[6];
  hw_i2c_read(&driver.i2c, 0x1D, 0x0F, &who_am_i, 1, 0);
  hw_i2c_read(&driver.i2c, 0x1D, 0x20, &ctrl1, 1, 0);
  hw_i2c_read(&driver.i2c, 0x1D, 0x21, &ctrl2, 1, 0);
  hw_i2c_read(&driver.i2c, 0x1D, 0x24, &ctrl5, 1, 0);
  hw_i2c_read(&driver.i2c, 0x1D, 0x25, &ctrl6, 1, 0);
  hw_i2c_read(&driver.i2c, 0x1D, 0x26, &ctrl7, 1, 0);
  hw_i2c_read(&driver.i2c, 0x1D, 0x27, &status_a, 1, 0);
  hw_i2c_read(&driver.i2c, 0x1D, 0x07, &status_m, 1, 0);
  sys_printf("DEBUG: WHO_AM_I=0x%02X\n", who_am_i);
  sys_printf("DEBUG: CTRL1=0x%02X CTRL2=0x%02X CTRL5=0x%02X CTRL6=0x%02X "
             "CTRL7=0x%02X\n",
             ctrl1, ctrl2, ctrl5, ctrl6, ctrl7);
  sys_printf("DEBUG: STATUS_A=0x%02X STATUS_M=0x%02X\n", status_a, status_m);

  // Read raw accelerometer data
  hw_i2c_read(&driver.i2c, 0x1D, 0xA8, accel_raw, 6, 0); // 0x28 | 0x80
  sys_printf("DEBUG: Accel raw: %02X %02X %02X %02X %02X %02X\n", accel_raw[0],
             accel_raw[1], accel_raw[2], accel_raw[3], accel_raw[4],
             accel_raw[5]);

  // Read raw magnetometer data
  hw_i2c_read(&driver.i2c, 0x1D, 0x88, mag_raw, 6, 0); // 0x08 | 0x80
  sys_printf("DEBUG: Mag raw: %02X %02X %02X %02X %02X %02X\n", mag_raw[0],
             mag_raw[1], mag_raw[2], mag_raw[3], mag_raw[4], mag_raw[5]);

  // Read raw temperature data
  uint8_t temp_raw[2];
  hw_i2c_read(&driver.i2c, 0x1D, 0x85, temp_raw, 2, 0); // 0x05 | 0x80
  sys_printf("DEBUG: Temp raw: %02X %02X\n", temp_raw[0], temp_raw[1]);

  // Read and display accelerometer, magnetometer, and temperature data
  sys_printf("\nReading LSM303 sensor data (10 samples):\n");
  sys_printf("Sample | Accel X | Accel Y | Accel Z |  Mag X  |  Mag Y  |  Mag "
             "Z  | Temp | Heading\n");
  sys_printf(
      "-------|---------|---------|---------|---------|---------|--------"
      "---|------|--------\n");

  for (int i = 0; i < 10; i++) {
    bool accel_ok = driver_envirophat_read_accel(&driver, &accel);
    bool mag_ok = driver_envirophat_read_mag(&driver, &mag);
    bool temp_ok = driver_envirophat_read_temp(&driver, &temp);

    if (accel_ok && mag_ok && temp_ok) {
      float heading = driver_lsm303_get_heading(&mag);
      printf("  %2d   | %7.3f | %7.3f | %7.3f | %7.3f | %7.3f | %7.3f | "
             "%5.1f | %6.1f\n",
             i + 1, accel.x, accel.y, accel.z, mag.x, mag.y, mag.z, temp,
             heading);
      fflush(stdout);
    } else {
      sys_printf("  %2d   | Failed to read sensor data (accel=%d mag=%d "
                 "temp=%d)\n",
                 i + 1, accel_ok, mag_ok, temp_ok);
    }

    sys_sleep(500);
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
  return TestHardwareMain("test_envirophat_03", test_envirophat_03);
}
