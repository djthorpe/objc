#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_hw_03(void) {
  hw_adc_t temp = hw_adc_init_temperature();
  hw_adc_t battery = hw_adc_init_battery();

  for (int i = 0; i < 10; i++) {
    float temp_value = hw_adc_read_temperature(&temp);
    float battery_value = hw_adc_read_voltage(&battery);

    sys_printf("Temperature: %d C, Battery Voltage: %d V\n",
               (int)(temp_value * 1000.0f), (int)(battery_value * 1000.0f));

    sys_sleep(1000); // Sleep for 1 second between readings
  }

  hw_adc_finalize(&temp);
  hw_adc_finalize(&battery);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestHardwareMain("test_hw_03", test_hw_03); }
