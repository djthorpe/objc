#include <drivers/drivers.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_envirophat_02(void) {
  driver_envirophat_t driver;
  driver_tcs3472_rgbc_t color;

  // Initialize the envirophat driver
  sys_printf("Initializing Enviro pHAT...\n");
  if (!driver_envirophat_init(&driver)) {
    sys_printf("Failed to initialize Enviro pHAT\n");
    return -1;
  }
  sys_printf("Enviro pHAT initialized successfully\n");

  // Read and display color data with LED on and off
  sys_printf("\nReading color data:\n");
  sys_printf("LED    | Sample | Clear | Red   | Green | Blue\n");
  sys_printf("-------|--------|-------|-------|-------|-------\n");

  // First 5 samples with LED OFF
  driver_envirophat_set_led(&driver, false);
  for (int i = 0; i < 5; i++) {
    if (driver_envirophat_read_color(&driver, &color)) {
      sys_printf(" OFF   |   %2d   | %5u | %5u | %5u | %5u\n", i + 1,
                 color.clear, color.red, color.green, color.blue);
    } else {
      sys_printf(" OFF   |   %2d   | Failed to read color data\n", i + 1);
    }
    sys_sleep(500);
  }

  // Turn on LED
  sys_printf("-------|--------|-------|-------|-------|-------\n");
  sys_printf("Turning on LED...\n");
  driver_envirophat_set_led(&driver, true);
  sys_sleep(200); // Let LED stabilize
  sys_printf("-------|--------|-------|-------|-------|-------\n");

  // Next 5 samples with LED ON
  for (int i = 0; i < 5; i++) {
    if (driver_envirophat_read_color(&driver, &color)) {
      sys_printf(" ON    |   %2d   | %5u | %5u | %5u | %5u\n", i + 6,
                 color.clear, color.red, color.green, color.blue);
    } else {
      sys_printf(" ON    |   %2d   | Failed to read color data\n", i + 6);
    }
    sys_sleep(500);
  }

  // Turn off LED
  sys_printf("-------|--------|-------|-------|-------|-------\n");
  sys_printf("Turning off LED...\n");
  driver_envirophat_set_led(&driver, false);
  sys_sleep(200); // Let readings stabilize
  sys_printf("-------|--------|-------|-------|-------|-------\n");

  // Final 5 samples with LED OFF again
  for (int i = 0; i < 5; i++) {
    if (driver_envirophat_read_color(&driver, &color)) {
      sys_printf(" OFF   |   %2d   | %5u | %5u | %5u | %5u\n", i + 11,
                 color.clear, color.red, color.green, color.blue);
    } else {
      sys_printf(" OFF   |   %2d   | Failed to read color data\n", i + 11);
    }
    sys_sleep(500);
  }

  sys_printf("\n");
  sys_printf("Finalizing Enviro pHAT...\n");
  driver_envirophat_finalize(&driver);
  sys_printf("Test completed successfully\n");

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  return TestHardwareMain("test_envirophat_02", test_envirophat_02);
}
