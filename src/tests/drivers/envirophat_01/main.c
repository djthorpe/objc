#include <drivers/drivers.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_envirophat_01(void) {
  driver_envirophat_t driver;

  // Initialize the envirophat driver
  sys_printf("Initializing Enviro pHAT...\n");
  if (!driver_envirophat_init(&driver)) {
    sys_printf("Failed to initialize Enviro pHAT\n");
    return -1;
  }
  sys_printf("Enviro pHAT initialized successfully\n");

  // Blink the LED 5 times
  sys_printf("Blinking LED...\n");
  for (int i = 0; i < 5; i++) {
    sys_printf("  LED ON\n");
    driver_envirophat_set_led(&driver, true);
    sys_sleep(500);

    sys_printf("  LED OFF\n");
    driver_envirophat_set_led(&driver, false);
    sys_sleep(500);
  }

  // Finalize the driver
  sys_printf("Finalizing Enviro pHAT...\n");
  driver_envirophat_finalize(&driver);
  sys_printf("Test completed successfully\n");

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  return TestHardwareMain("test_envirophat_01", test_envirophat_01);
}
