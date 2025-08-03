#include <drivers/drivers.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

int main() {
  sys_printf("=== BME280 DEBUG TEST ===\n");

  // Initialize I2C (you'll need to adjust this for your setup)
  hw_i2c_t i2c = {0};
  // For testing, let's just set up a minimal i2c structure
  // In real code, you'd call hw_i2c_init() here

  sys_printf("About to call driver_bme280_i2c_init...\n");

  // Try to initialize the BME280 driver
  driver_bme280_t bme280 = driver_bme280_i2c_init(&i2c, 0x76);

  sys_printf("driver_bme280_i2c_init returned\n");
  sys_printf("Result bme280 pointer: %p\n", (void *)&bme280);
  sys_printf("Result bme280.i2c: %p\n", (void *)bme280.i2c);
  sys_printf("Result bme280.address: 0x%02X\n", bme280.address);
  sys_printf("Result bme280.timeout_ms: %u\n", bme280.timeout_ms);

  if (driver_bme280_valid(&bme280)) {
    sys_printf("BME280 driver is valid\n");

    sys_printf("About to call driver_bme280_get_mode...\n");
    driver_bme280_mode_t mode = driver_bme280_get_mode(&bme280);
    sys_printf("driver_bme280_get_mode returned: %d\n", mode);
  } else {
    sys_printf("BME280 driver is invalid\n");
  }

  sys_printf("=== END BME280 DEBUG TEST ===\n");
  return 0;
}
