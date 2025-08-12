#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_hw_03(void) {
  hw_adc_t temp = hw_adc_init_temperature();
  for (int i = 0; i < 10; i++) {
    float temp_value = hw_adc_read_temperature(&temp);
    sys_printf("Temperature: %d C\n", (int)(temp_value * 1000.0f));

    sys_sleep(100); // Sleep for 100 milliseconds between readings
  }

  hw_adc_finalize(&temp);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestHardwareMain("test_hw_03", test_hw_03); }
