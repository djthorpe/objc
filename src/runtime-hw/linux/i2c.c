#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
// CONSTANTS

#define I2C_MAX_ADAPTERS 8
#define I2C_SLAVE_ADDR_MAX 0x7F

// Helper macro to access file descriptor stored in reserved bytes
#define I2C_FD(i2c) (*((int *)(i2c)->reserved))

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static bool _i2c_set_slave_addr(int fd, uint8_t addr);

/**
 * @brief Get the total number of available I2C adapters.
 */
uint8_t hw_i2c_count(void) {
  uint8_t count = 0;
  char dev[32];

  // Try to open each /dev/i2c-N device to count available adapters
  for (uint8_t adapter = 0; adapter < I2C_MAX_ADAPTERS; adapter++) {
    sys_sprintf(dev, sizeof(dev), "/dev/i2c-%u", adapter);
    int fd = open(dev, O_RDWR);
    if (fd >= 0) {
      close(fd);
      count++;
    } else {
      // Stop at first missing adapter (assumes sequential numbering)
      break;
    }
  }

  return count;
}

/**
 * @brief Initialize an I2C interface with specific adapter and pins.
 *
 * Note: On Linux, I2C pins are hardware-defined and cannot be configured.
 * This function always returns false.
 * Use hw_i2c_init_device() instead on Linux.
 */
bool hw_i2c_init(hw_i2c_t *i2c, uint8_t index, uint8_t sda, uint8_t scl,
                 uint32_t baudrate) {
  sys_assert(i2c);
  (void)index;
  (void)sda;
  (void)scl;
  (void)baudrate;

  I2C_FD(i2c) = -1;
#ifdef DEBUG
  sys_printf("hw_i2c_init: not supported on Linux - use hw_i2c_init_device() "
             "instead\n");
#endif
  return false;
}

/**
 * @brief Initialize an I2C interface using default pins and adapter.
 */
bool hw_i2c_init_default(hw_i2c_t *i2c, uint32_t baudrate) {
  sys_assert(i2c);
#ifdef DEBUG
  sys_printf("hw_i2c_init_default: using /dev/i2c-1 baudrate=%u\n", baudrate);
#endif
  return hw_i2c_init_device(i2c, "/dev/i2c-1", baudrate);
}

/**
 * @brief Initialize an I2C interface with device path.
 */
bool hw_i2c_init_device(hw_i2c_t *i2c, const char *device, uint32_t baudrate) {
  sys_assert(i2c);
#ifdef DEBUG
  sys_printf("hw_i2c_init_device: device=%s baudrate=%u\n",
             device ? device : "(null)", baudrate);
#endif

  // Initialize to invalid state
  I2C_FD(i2c) = -1;

  // Validate device path
  if (!device || device[0] == '\0') {
#ifdef DEBUG
    sys_printf("hw_i2c_init_device: invalid device path\n");
#endif
    return false;
  }

  // Validate baudrate
  if (baudrate == 0) {
#ifdef DEBUG
    sys_printf("hw_i2c_init_device: invalid baudrate=0\n");
#endif
    return false;
  }

  // Open the device
  int fd = open(device, O_RDWR);
  if (fd < 0) {
#ifdef DEBUG
    sys_printf("hw_i2c_init_device: failed to open %s\n", device);
#endif
    return false;
  }

  // Store file descriptor
  I2C_FD(i2c) = fd;

#ifdef DEBUG
  sys_printf("hw_i2c_init_device: success device=%s fd=%d\n", device, fd);
#endif

  return true;
}

/**
 * @brief Finalize and release an I2C interface.
 */
void hw_i2c_finalize(hw_i2c_t *i2c) {
  sys_assert(i2c);
  if (I2C_FD(i2c) < 0) {
    return; // Already finalized or invalid
  }

  close(I2C_FD(i2c));
  I2C_FD(i2c) = -1;
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Get true if the I2C interface is valid.
 */
bool hw_i2c_valid(hw_i2c_t *i2c) { return i2c && I2C_FD(i2c) >= 0; }

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Detect if an I2C device is present at the specified address.
 */
bool hw_i2c_detect(hw_i2c_t *i2c, uint8_t addr) {
  sys_assert(i2c);
  sys_assert(addr <= I2C_SLAVE_ADDR_MAX);

  if (!hw_i2c_valid(i2c)) {
    return false;
  }

  // Use I2C_SMBUS ioctl to probe the device
  if (ioctl(I2C_FD(i2c), I2C_SLAVE, addr) < 0) {
    return false;
  }

  // Try to read a byte to detect device presence
  struct i2c_smbus_ioctl_data args;
  union i2c_smbus_data data;
  args.read_write = I2C_SMBUS_READ;
  args.command = 0;
  args.size = I2C_SMBUS_BYTE;
  args.data = &data;

  return (ioctl(I2C_FD(i2c), I2C_SMBUS, &args) >= 0);
} /**
   * @brief Perform an I2C transfer operation (read, write, or combined).
   */
size_t hw_i2c_xfr(hw_i2c_t *i2c, uint8_t addr, void *data, size_t tx, size_t rx,
                  uint32_t timeout_ms) {
  sys_assert(hw_i2c_valid(i2c));
  sys_assert(addr <= I2C_SLAVE_ADDR_MAX);
  sys_assert(data || (tx == 0 && rx == 0));
  (void)timeout_ms; // Linux I2C driver handles timeouts internally

  // Set slave address
  if (!_i2c_set_slave_addr(I2C_FD(i2c), addr)) {
    return 0;
  }

  size_t total_transferred = 0;

  // Perform write operation if tx > 0
  if (tx > 0) {
    ssize_t written = write(I2C_FD(i2c), data, tx);
    if (written < 0) {
      return 0;
    }
    total_transferred += written;

    if ((size_t)written != tx) {
      // Partial write - return what we got
      return total_transferred;
    }
  }

  // Perform read operation if rx > 0
  if (rx > 0) {
    uint8_t *read_buf = (uint8_t *)data + tx;
    ssize_t bytes_read = read(I2C_FD(i2c), read_buf, rx);
    if (bytes_read < 0) {
      // Return bytes written if any
      return total_transferred;
    }
    total_transferred += bytes_read;
  }

  return total_transferred;
}

/**
 * @brief Read data from a specific register of an I2C device.
 */
size_t hw_i2c_read(hw_i2c_t *i2c, uint8_t addr, uint8_t reg, void *data,
                   size_t len, uint32_t timeout_ms) {
  sys_assert(hw_i2c_valid(i2c));
  sys_assert(addr <= I2C_SLAVE_ADDR_MAX);
  sys_assert(data || len == 0);

  // Write the register address to the bus
  size_t bytes_written =
      hw_i2c_xfr(i2c, addr, &reg, sizeof(uint8_t), 0, timeout_ms);
  if (bytes_written == 0) {
    return 0; // Failed to write register address
  }

  // Read the data from the bus
  return hw_i2c_xfr(i2c, addr, data, 0, len, timeout_ms);
}

/**
 * @brief Write data to a specific register of an I2C device.
 */
size_t hw_i2c_write(hw_i2c_t *i2c, uint8_t addr, uint8_t reg, const void *data,
                    size_t len, uint32_t timeout_ms) {
  sys_assert(hw_i2c_valid(i2c));
  sys_assert(addr <= I2C_SLAVE_ADDR_MAX);
  sys_assert(data || len == 0);

  // Write the register address to the bus
  size_t bytes_written =
      hw_i2c_xfr(i2c, addr, (void *)&reg, sizeof(uint8_t), 0, timeout_ms);
  if (bytes_written == 0) {
    return 0; // Failed to write register address
  }

  // Write the data to the register
  return hw_i2c_xfr(i2c, addr, (void *)data, len, 0, timeout_ms);
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

static bool _i2c_set_slave_addr(int fd, uint8_t addr) {
  if (fd < 0 || addr > I2C_SLAVE_ADDR_MAX) {
    return false;
  }

  // Set the slave address for subsequent operations
  if (ioctl(fd, I2C_SLAVE, addr) < 0) {
#ifdef DEBUG
    sys_printf("_i2c_set_slave_addr: failed to set address 0x%02X\n", addr);
#endif
    return false;
  }

  return true;
}
