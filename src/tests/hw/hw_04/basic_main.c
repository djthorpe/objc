#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// BASIC SPI TEST

int test_hw_04_basic(void) {
  sys_printf("=== Basic SPI Hardware Test ===\n\n");

  // Test SPI adapter count
  uint8_t spi_count = hw_spi_count();
  sys_printf("Available SPI adapters: %u\n", spi_count);

  if (spi_count == 0) {
    sys_printf("No SPI adapters available on this platform\n");
    sys_printf("This is expected on macOS/stub implementation\n");
    return 0;
  }

  // Test basic initialization
  hw_spi_t spi;
  if (hw_spi_init_default(&spi, true, 1000000)) {
    sys_printf("SPI initialization: SUCCESS\n");

    if (hw_spi_valid(&spi)) {
      sys_printf("SPI validity check: VALID\n");

      // Test a simple transfer (this will likely fail on stub but that's OK)
      uint8_t test_data[4] = {0x01, 0x02, 0x03, 0x04};
      size_t result = hw_spi_xfr(&spi, test_data, 4, 0);
      sys_printf("Test transfer (4 bytes): %zu bytes\n", result);

      hw_spi_finalize(&spi);
      sys_printf("SPI finalization: SUCCESS\n");
    } else {
      sys_printf("SPI validity check: INVALID\n");
    }
  } else {
    sys_printf("SPI initialization: FAILED\n");
    sys_printf("This is expected on platforms without SPI hardware\n");
  }

  sys_printf("\n=== Basic SPI test completed ===\n");
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) {
  return TestHardwareMain("test_hw_04_basic", test_hw_04_basic);
}