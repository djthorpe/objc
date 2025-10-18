/**
 * @file envirophat.c
 * @brief Enviro pHAT sensor interface implementation
 * @ingroup Envirophat
 *
 * Enviro pHAT interface for reading temperature, humidity, pressure,
 * light, and color data using the BME280 and TCS34725 sensors,
 * and LSM303 accelerometer/magnetometer, plus ADS1015 ADC.
 */
#include <drivers/drivers_bme280.h>
#include <drivers/drivers_envirophat.h>
#include <drivers/drivers_lsm303.h>
#include <drivers/drivers_tcs3472.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// CONSTANTS

// I2C addresses for Enviro pHAT sensors
#define BME280_I2C_ADDR 0x77   // Temperature, humidity, pressure
#define TCS34725_I2C_ADDR 0x29 // Light and color sensor
#define LSM303_ACC_ADDR 0x1D   // Accelerometer/Magnetometer
#define ADS1015_ADDR 0x49      // ADC

// LED pin (GPIO 4 on Raspberry Pi)
#define LED_PIN 4

// I2C baudrate (100kHz standard mode)
#define I2C_BAUDRATE 100000

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

bool driver_envirophat_init(driver_envirophat_t *driver) {
  sys_assert(driver);

  // Zero out the structure
  sys_memset(driver, 0, sizeof(driver_envirophat_t));

  // Initialize the LED GPIO pin (bank 0, LED_PIN, output mode)
  driver->led = hw_gpio_init(0, LED_PIN, HW_GPIO_OUTPUT);

  // Check if GPIO initialization was successful
  if (!hw_gpio_valid(&driver->led)) {
#ifdef DEBUG
    sys_printf("driver_envirophat_init: failed to initialize LED GPIO\n");
#endif
    return false;
  }

  // Turn off LED by default
  hw_gpio_set(&driver->led, false);

  // Initialize I2C bus (default I2C device at 100kHz)
  if (!hw_i2c_init_default(&driver->i2c, I2C_BAUDRATE)) {
#ifdef DEBUG
    sys_printf("driver_envirophat_init: failed to initialize I2C bus\n");
#endif
    hw_gpio_finalize(&driver->led);
    return false;
  }

  // Initialize TCS3472 color sensor with default settings
  if (!driver_tcs3472_init(&driver->tcs3472, &driver->i2c)) {
#ifdef DEBUG
    sys_printf("driver_envirophat_init: failed to initialize TCS3472 sensor\n");
#endif
    hw_i2c_finalize(&driver->i2c);
    hw_gpio_finalize(&driver->led);
    return false;
  }

  // Initialize LSM303 accelerometer/magnetometer with default settings
  if (!driver_lsm303_init(&driver->lsm303, &driver->i2c)) {
#ifdef DEBUG
    sys_printf("driver_envirophat_init: failed to initialize LSM303 sensor\n");
#endif
    driver_tcs3472_finalize(&driver->tcs3472);
    hw_i2c_finalize(&driver->i2c);
    hw_gpio_finalize(&driver->led);
    return false;
  }

  // Try to initialize BME280 sensor (may not be present on all boards)
  // Try primary address first (0x76), then secondary (0x77)
  if (!driver_bme280_init_i2c(&driver->bme280, &driver->i2c, 0)) {
    driver_bme280_init_i2c(&driver->bme280, &driver->i2c, 1);
  }
#ifdef DEBUG
  if (driver_bme280_valid(&driver->bme280)) {
    sys_printf("driver_envirophat_init: BME280 sensor detected at 0x%02X\n",
               driver->bme280.addr);
  } else {
    sys_printf(
        "driver_envirophat_init: BME280 sensor not detected (optional)\n");
  }
#endif

  // TODO: Detect and initialize ADS1015 ADC

  return true;
}

void driver_envirophat_finalize(driver_envirophat_t *driver) {
  sys_assert(driver);

  // Turn off LED
  hw_gpio_set(&driver->led, false);

  // Finalize TCS3472 sensor
  driver_tcs3472_finalize(&driver->tcs3472);

  // Finalize LSM303 sensor
  driver_lsm303_finalize(&driver->lsm303);

  // Finalize BME280 sensor if present
  if (driver_bme280_valid(&driver->bme280)) {
    driver_bme280_finalize(&driver->bme280);
  }

  // Finalize I2C bus
  hw_i2c_finalize(&driver->i2c);

  // Finalize LED GPIO
  hw_gpio_finalize(&driver->led);

  // TODO: Clean up BME280 sensor
  // TODO: Clean up ADS1015 ADC

  // Zero out the structure
  sys_memset(driver, 0, sizeof(driver_envirophat_t));
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

void driver_envirophat_set_led(driver_envirophat_t *driver, bool state) {
  sys_assert(driver);
  hw_gpio_set(&driver->led, state);
}

///////////////////////////////////////////////////////////////////////////////
// COLOR SENSOR

bool driver_envirophat_read_color(driver_envirophat_t *driver,
                                  driver_tcs3472_rgbc_t *data) {
  sys_assert(driver);
  sys_assert(data);
  return driver_tcs3472_read(&driver->tcs3472, data);
}

///////////////////////////////////////////////////////////////////////////////
// ACCELEROMETER / MAGNETOMETER

bool driver_envirophat_read_accel(driver_envirophat_t *driver,
                                  driver_lsm303_vec3_t *data) {
  sys_assert(driver);
  sys_assert(data);
  return driver_lsm303_read_accel(&driver->lsm303, data);
}

bool driver_envirophat_read_mag(driver_envirophat_t *driver,
                                driver_lsm303_vec3_t *data) {
  sys_assert(driver);
  sys_assert(data);
  return driver_lsm303_read_mag(&driver->lsm303, data);
}

bool driver_envirophat_read_temp(driver_envirophat_t *driver, float *temp) {
  sys_assert(driver);
  sys_assert(temp);
  return driver_lsm303_read_temp(&driver->lsm303, temp);
}

///////////////////////////////////////////////////////////////////////////////
// BME280 ENVIRONMENTAL SENSOR

bool driver_envirophat_read_environmental(driver_envirophat_t *driver,
                                          driver_bme280_data_t *data) {
  sys_assert(driver);
  sys_assert(data);

  if (!driver_bme280_valid(&driver->bme280)) {
    return false;
  }

  return driver_bme280_read_data(&driver->bme280, data);
}
