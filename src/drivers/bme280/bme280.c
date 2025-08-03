#include <BME280_SensorAPI/bme280.h>
#include <drivers/drivers.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

BME280_INTF_RET_TYPE _driver_bme280_i2c_read(uint8_t reg, uint8_t *data,
                                             uint32_t len, void *userdata);

BME280_INTF_RET_TYPE _driver_bme280_i2c_write(uint8_t reg, const uint8_t *data,
                                              uint32_t len, void *userdata);

void _driver_bme280_delay_us(uint32_t period, void *intf_ptr);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a BME280 sensor driver.
 */
driver_bme280_t driver_bme280_i2c_init(hw_i2c_t *i2c, uint8_t address) {
  sys_assert(i2c);
  sys_assert(hw_i2c_valid(i2c));

  // Create the BME280 driver structure
  driver_bme280_t bme280 = {0};
  struct bme280_dev dev = {0};

  // Set the address to the default if not specified
  if (address == 0) {
    address = BME280_I2C_ADDR_PRIM;
  }

  // Set up the I2C interface
  dev.intf = BME280_I2C_INTF;
  dev.intf_ptr = i2c; // Pass the I2C interface pointer
  dev.read = _driver_bme280_i2c_read;
  dev.write = _driver_bme280_i2c_write;
  dev.delay_us = _driver_bme280_delay_us;

  // Initialize the BME280 sensor - set the address and read/write timeout
  i2c->reserved[0] = address;
  i2c->reserved[1] = 100;
  uint8_t rslt = bme280_init(&dev);
  if (rslt != BME280_OK) {
#ifdef DEBUG
    sys_printf("bme280_init() error code %d\n", rslt);
#endif
    return bme280; // Return empty structure on failure
  }

  // Success
  bme280.i2c = i2c;
  bme280.address = address;

  // Return success
  return bme280;
}

/**
 * @brief Finalize and release a BME280 sensor driver.
 */
void driver_bme280_finalize(driver_bme280_t *bme280) {
  sys_assert(bme280 && driver_bme280_valid(bme280));

  // TODO: Put BME280 into sleep mode if needed

  bme280->i2c = NULL;  // Clear the I2C interface pointer
  bme280->address = 0; // Reset the address
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

BME280_INTF_RET_TYPE _driver_bme280_i2c_read(uint8_t reg, uint8_t *data,
                                             uint32_t len, void *userdata) {
  sys_assert(userdata);
  sys_assert(data || len == 0);

  hw_i2c_t *i2c = (hw_i2c_t *)userdata;
  uint8_t addr = i2c->reserved[0];
  uint8_t timeout_ms = i2c->reserved[1];

#ifdef DEBUG
  sys_printf("Reading BME280 %#02X/%#02X, length %u\n", addr, reg, len);
#endif

  size_t bytes_read = hw_i2c_read(i2c, addr, reg, data, len, timeout_ms);
  return (bytes_read == len) ? BME280_OK : BME280_E_COMM_FAIL;
}

BME280_INTF_RET_TYPE _driver_bme280_i2c_write(uint8_t reg, const uint8_t *data,
                                              uint32_t len, void *userdata) {
  sys_assert(userdata);
  sys_assert(data || len == 0);

  hw_i2c_t *i2c = (hw_i2c_t *)userdata;
  uint8_t addr = i2c->reserved[0];
  uint8_t timeout_ms = i2c->reserved[1];

#ifdef DEBUG
  sys_printf("Writing BME280 %#02X/%#02X, length %u\n", addr, reg, len);
#endif

  size_t bytes_written = hw_i2c_write(i2c, addr, reg, data, len, timeout_ms);
  return (bytes_written == len) ? BME280_OK : BME280_E_COMM_FAIL;
}

/*!
 * Delay function map to COINES platform
 */
void _driver_bme280_delay_us(uint32_t period, void *userdata) {
  (void)userdata; // Unused parameter
  (void)period;   // Unused parameter
#ifdef DEBUG
  sys_printf("TODO: Setting BME280 delay: %u us\n", period);
#endif
}
