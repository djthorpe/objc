#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_hw_02(void) {
  hw_i2c_t i2c = hw_i2c_init_default(100000);

  for (int addr = 0x08; addr <= 0x77; addr++) {
    bool detected = hw_i2c_detect(&i2c, addr);
    if (detected) {
      sys_printf("Device at address 0x%02X detected.\n", addr);
    }
  }

  hw_i2c_finalize(&i2c);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestHardwareMain("test_hw_02", test_hw_02); }
