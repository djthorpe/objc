#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

static void test_spi_basic_functionality(void) {
  sys_printf("Testing basic SPI functionality...\n");

  // Test SPI adapter count
  uint8_t spi_count = hw_spi_count();
  sys_printf("  Available SPI adapters: %u\n", spi_count);

  if (spi_count == 0) {
    sys_printf("  No SPI adapters available - skipping SPI tests\n");
    return;
  }

  // Test default initialization
  hw_spi_t spi;
  bool init_result = hw_spi_init_default(&spi, true, 1000000);
  sys_printf("  Default init (CS active low, 1MHz): %s\n",
             init_result ? "SUCCESS" : "FAILED");

  if (init_result) {
    // Test validity check
    bool is_valid = hw_spi_valid(&spi);
    sys_printf("  SPI validity check: %s\n", is_valid ? "VALID" : "INVALID");
    test_assert(is_valid);

    // Test finalization
    hw_spi_finalize(&spi);

    // Test validity after finalization
    is_valid = hw_spi_valid(&spi);
    sys_printf("  SPI validity after finalize: %s\n",
               is_valid ? "VALID" : "INVALID");
    test_assert(!is_valid);
  }

  // Test alternative initialization with CS active high
  init_result = hw_spi_init_default(&spi, false, 500000);
  sys_printf("  Default init (CS active high, 500kHz): %s\n",
             init_result ? "SUCCESS" : "FAILED");

  if (init_result) {
    test_assert(hw_spi_valid(&spi));
    hw_spi_finalize(&spi);
  }
}

static void test_spi_adapter_initialization(void) {
  sys_printf("Testing SPI adapter initialization...\n");

  uint8_t spi_count = hw_spi_count();
  if (spi_count == 0) {
    sys_printf("  No SPI adapters available - skipping adapter tests\n");
    return;
  }

  // Test initialization with specific adapter
  for (uint8_t adapter = 0; adapter < spi_count && adapter < 3; adapter++) {
    hw_spi_t spi;
    bool init_result = hw_spi_init(&spi, adapter, 0, 0, 0, 0, true, 2000000);
    sys_printf("  Adapter %u init (2MHz): %s\n", adapter,
               init_result ? "SUCCESS" : "FAILED");

    if (init_result) {
      test_assert(hw_spi_valid(&spi));
      hw_spi_finalize(&spi);
    }
  }
}

static void test_spi_device_path_initialization(void) {
  sys_printf("Testing SPI device path initialization...\n");

  hw_spi_t spi;

  // Test with common Linux device paths
  const char *device_paths[] = {"/dev/spidev0.0", "/dev/spidev0.1",
                                "/dev/spidev1.0", "/dev/spidev10.0"};

  for (size_t i = 0; i < sizeof(device_paths) / sizeof(device_paths[0]); i++) {
    bool init_result = hw_spi_init_device(&spi, device_paths[i], true, 1000000);
    sys_printf("  Device path '%s': %s\n", device_paths[i],
               init_result ? "SUCCESS" : "FAILED");

    if (init_result) {
      test_assert(hw_spi_valid(&spi));
      hw_spi_finalize(&spi);
    }
  }

  // Test with invalid device path
  bool init_result =
      hw_spi_init_device(&spi, "/dev/invalid_spi_device", true, 1000000);
  sys_printf("  Invalid device path: %s\n",
             init_result ? "UNEXPECTED SUCCESS" : "FAILED (expected)");
  test_assert(!init_result);
  test_assert(!hw_spi_valid(&spi));
}

static void test_spi_transfer_operations(void) {
  sys_printf("Testing SPI transfer operations...\n");

  hw_spi_t spi;
  bool init_result = hw_spi_init_default(&spi, true, 1000000);

  if (!init_result) {
    sys_printf("  Cannot initialize SPI - skipping transfer tests\n");
    return;
  }

  // Test buffer for transfers
  uint8_t test_buffer[16];

  // Initialize test data
  for (size_t i = 0; i < sizeof(test_buffer); i++) {
    test_buffer[i] = (uint8_t)(i + 0x10);
  }

  // Test basic transfer (write-only)
  size_t bytes_transferred = hw_spi_xfr(&spi, test_buffer, 4, 0);
  sys_printf("  Write-only transfer (4 bytes): %zu bytes\n", bytes_transferred);

  // Test read-only transfer
  sys_memset(test_buffer, 0, sizeof(test_buffer));
  bytes_transferred = hw_spi_xfr(&spi, test_buffer, 0, 4);
  sys_printf("  Read-only transfer (4 bytes): %zu bytes\n", bytes_transferred);

  // Test combined write-then-read transfer
  test_buffer[0] = 0xAA;
  bytes_transferred = hw_spi_xfr(&spi, test_buffer, 1, 3);
  sys_printf("  Write-then-read transfer (1+3 bytes): %zu bytes\n",
             bytes_transferred);

  // Test register read
  uint8_t reg_data[4];
  bytes_transferred = hw_spi_read(&spi, 0x80, reg_data, sizeof(reg_data));
  sys_printf("  Register read (reg 0x80, 4 bytes): %zu bytes\n",
             bytes_transferred);

  // Test register write
  uint8_t write_data[] = {0x01, 0x02, 0x03, 0x04};
  bytes_transferred = hw_spi_write(&spi, 0x40, write_data, sizeof(write_data));
  sys_printf("  Register write (reg 0x40, 4 bytes): %zu bytes\n",
             bytes_transferred);

  // Test register write with no data (register-only write)
  bytes_transferred = hw_spi_write(&spi, 0x20, NULL, 0);
  sys_printf("  Register-only write (reg 0x20): %zu bytes\n",
             bytes_transferred);

  hw_spi_finalize(&spi);
}

static void test_spi_error_conditions(void) {
  sys_printf("Testing SPI error conditions...\n");

  hw_spi_t spi;

  // Test operations on uninitialized SPI
  sys_memset(&spi, 0, sizeof(spi));
  test_assert(!hw_spi_valid(&spi));

  size_t result = hw_spi_xfr(&spi, NULL, 0, 0);
  sys_printf("  Transfer on invalid SPI: %zu bytes (expected 0)\n", result);
  test_assert(result == 0);

  uint8_t dummy_data[4];
  result = hw_spi_read(&spi, 0x00, dummy_data, sizeof(dummy_data));
  sys_printf("  Read on invalid SPI: %zu bytes (expected 0)\n", result);
  test_assert(result == 0);

  result = hw_spi_write(&spi, 0x00, dummy_data, sizeof(dummy_data));
  sys_printf("  Write on invalid SPI: %zu bytes (expected 0)\n", result);
  test_assert(result == 0);

  // Test NULL pointer handling (This will trigger assertion in debug builds)
  // We expect this to fail in stub implementation due to sys_assert()
  sys_printf(
      "  Note: NULL pointer tests will trigger assertions in debug builds\n");

  // Uncomment the following lines only for release builds where assertions are
  // disabled:
  /*
  bool init_result = hw_spi_init_default(NULL, true, 1000000);
  sys_printf("  Init with NULL pointer: %s (expected FAILED)\n",
             init_result ? "SUCCESS" : "FAILED");
  test_assert(!init_result);

  // Test init_device with NULL path
  init_result = hw_spi_init_device(&spi, NULL, true, 1000000);
  sys_printf("  Init device with NULL path: %s (expected FAILED)\n",
             init_result ? "SUCCESS" : "FAILED");
  test_assert(!init_result);
  */
}

static void test_spi_baudrate_variations(void) {
  sys_printf("Testing SPI baudrate variations...\n");

  uint32_t test_baudrates[] = {
      100000,  // 100 kHz
      500000,  // 500 kHz
      1000000, // 1 MHz
      2000000, // 2 MHz
      5000000, // 5 MHz
      10000000 // 10 MHz
  };

  for (size_t i = 0; i < sizeof(test_baudrates) / sizeof(test_baudrates[0]);
       i++) {
    hw_spi_t spi;
    bool init_result = hw_spi_init_default(&spi, true, test_baudrates[i]);
    sys_printf("  Baudrate %u Hz: %s\n", test_baudrates[i],
               init_result ? "SUCCESS" : "FAILED");

    if (init_result) {
      test_assert(hw_spi_valid(&spi));
      hw_spi_finalize(&spi);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// MAIN TEST FUNCTION

int test_hw_04(void) {
  sys_printf("=== SPI Hardware Interface Tests ===\n\n");

  test_spi_basic_functionality();
  sys_printf("\n");

  test_spi_adapter_initialization();
  sys_printf("\n");

  test_spi_device_path_initialization();
  sys_printf("\n");

  test_spi_transfer_operations();
  sys_printf("\n");

  test_spi_error_conditions();
  sys_printf("\n");

  test_spi_baudrate_variations();
  sys_printf("\n");

  sys_printf("=== All SPI tests completed ===\n");
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestHardwareMain("test_hw_04", test_hw_04); }