#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_hw_02(void) {
  hw_i2c_t i2c;
  if (!hw_i2c_init_default(&i2c, 100000)) {
    sys_printf("Failed to initialize I2C\n");
    return 1;
  }

  sys_printf("Scanning I2C bus from 0x08 to 0x77...\n");
  sys_printf("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

  for (int row = 0; row < 8; row++) {
    sys_printf("%02X: ", row * 16);
    for (int col = 0; col < 16; col++) {
      int addr = row * 16 + col;
      if (addr < 0x08 || addr > 0x77) {
        sys_printf("   ");
      } else {
        bool detected = hw_i2c_detect(&i2c, addr);
        if (detected) {
          sys_printf("%02X ", addr);
        } else {
          sys_printf("-- ");
        }
        fflush(stdout);
      }
    }
    sys_printf("\n");
  }
  sys_printf("Scan complete.\n");

  hw_i2c_finalize(&i2c);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestHardwareMain("test_hw_02", test_hw_02); }
