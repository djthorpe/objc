#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Get the total number of available I2C adapters.
 */
uint8_t hw_i2c_count(void) { return 2; }

/**
 * @brief Initialize an I2C interface using default pins and adapter.
 */
hw_i2c_t hw_i2c_init_default(uint32_t baudrate) {
  return hw_i2c_init(PICO_DEFAULT_I2C, PICO_DEFAULT_I2C_SDA_PIN,
                     PICO_DEFAULT_I2C_SCL_PIN, baudrate);
}

/**
 * @brief Initialize an I2C interface with specific adapter and pins.
 */
hw_i2c_t hw_i2c_init(uint8_t adapter, uint8_t sda, uint8_t scl,
                     uint32_t baudrate) {
  sys_assert(adapter < hw_i2c_count());
  sys_assert(sda < hw_gpio_count() && scl < hw_gpio_count());
  sys_assert(baudrate > 0);

  // Set the GPIO pins
  hw_i2c_t i2c = {0};
  i2c.adapter = adapter;
  i2c.sda = hw_gpio_init(sda, HW_GPIO_I2C);
  sys_assert(i2c.sda.mask > 0);
  i2c.scl = hw_gpio_init(scl, HW_GPIO_I2C);
  sys_assert(i2c.scl.mask > 0);

  // Initialize the I2C interface
  i2c.baudrate = i2c_init(I2C_INSTANCE(i2c.adapter), baudrate);
  sys_assert(i2c.baudrate != 0);

  // Return success
  return i2c;
}

/**
 * @brief Finalize and release an I2C interface.
 */
void hw_i2c_finalize(hw_i2c_t *i2c) {
  sys_assert(i2c);
  sys_assert(i2c->baudrate > 0);

  // Deinitialize the I2C interface
  i2c_deinit(I2C_INSTANCE(i2c->adapter));

  // Reset the GPIO pins
  hw_gpio_finalize(&i2c->sda);
  hw_gpio_finalize(&i2c->scl);
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

// Addresses of the form 000 0xxx or 111 1xxx are reserved. No slave should
// have these addresses.
#define i2c_reserved_addr(addr)                                                \
  (((addr) & 0x78) == 0 || ((addr) & 0x78) == 0x78)

/**
 * @brief Detect if an I2C device is present at the specified address.
 */
bool hw_i2c_detect(hw_i2c_t *i2c, uint8_t addr) {
  sys_assert(i2c);

  // Check addresses
  if (addr < 0x08 || addr > 0x77) {
    return false;
  }
  if (i2c_reserved_addr(addr)) {
    return false;
  }

  // Perform a zero-length write to check if the device is present
  uint8_t dummy;
  return hw_i2c_xfr(i2c, addr, &dummy, 0, 1, 100) > 0;
}

/**
 * @brief Perform an I2C transfer operation (read, write, or combined).
 */
size_t hw_i2c_xfr(hw_i2c_t *i2c, uint8_t addr, uint8_t *data, size_t tx,
                  size_t rx, uint32_t timeout_ms) {
  sys_assert(i2c);
  sys_assert(addr >= 0x08 && addr <= 0x77);
  sys_assert(!i2c_reserved_addr(addr));
  sys_assert(data || (tx == 0 && rx == 0));

  // Number of bytes transferred
  size_t bytes_transferred = 0;

  // Perform writes
  if (tx > 0) {
    int ret;
    sys_assert(data);
    if (timeout_ms == 0) {
      ret = i2c_write_blocking(I2C_INSTANCE(i2c->adapter), addr & 0x7F, data,
                               tx, false);
    } else {
      ret = i2c_write_timeout_us(I2C_INSTANCE(i2c->adapter), addr & 0x7F, data,
                                 tx, false, timeout_ms * 1000);
    }
    if (ret == 0 || ret == PICO_ERROR_TIMEOUT || ret == PICO_ERROR_GENERIC) {
      return 0;
    } else {
      bytes_transferred += tx;
    }
  }

  // Perform reads
  if (rx > 0) {
    int ret;
    sys_assert(data);
    if (timeout_ms == 0) {
      ret = i2c_read_blocking(I2C_INSTANCE(i2c->adapter), addr & 0x7F,
                              data + tx, rx, false);
    } else {
      ret = i2c_read_timeout_us(I2C_INSTANCE(i2c->adapter), addr & 0x7F,
                                data + tx, rx, false, timeout_ms * 1000);
    }
    if (ret == 0 || ret == PICO_ERROR_TIMEOUT || ret == PICO_ERROR_GENERIC) {
      return 0;
    } else {
      bytes_transferred += rx;
    }
  }

  return bytes_transferred;
}
