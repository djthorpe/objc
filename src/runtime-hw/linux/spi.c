#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
// CONSTANTS

#define SPI_MAX_ADAPTERS 8
#define SPI_DEFAULT_BITS_PER_WORD 8

///////////////////////////////////////////////////////////////////////////////
// INTERNAL STRUCTURE

// Internal SPI state stored in the opaque structure
typedef struct {
  int fd;            ///< File descriptor for the SPI device
  uint8_t adapter;   ///< SPI adapter number
  bool cs_active;    ///< CS active state (false if active low)
  uint32_t baudrate; ///< SPI baud rate in Hz
} spi_internal_t;

// Static assert to ensure reserved bytes are sufficient for internal structure
_Static_assert(sizeof(spi_internal_t) <= sizeof(((hw_spi_t *)0)->reserved),
               "spi_internal_t is too large for hw_spi_t.reserved");

// Helper macro to access internal state from opaque structure
#define SPI_INTERNAL(spi) ((spi_internal_t *)(spi)->reserved)

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a SPI interface using default pins and adapter.
 */
bool hw_spi_init_default(hw_spi_t *spi, bool cs_active_low, uint32_t baudrate) {
  sys_assert(spi);

  // Clear the structure
  sys_memset(spi, 0, sizeof(hw_spi_t));

#ifdef DEBUG
  sys_printf("hw_spi_init_default: using /dev/spidev0.0 cs_active_low=%d "
             "baudrate=%u\n",
             cs_active_low, baudrate);
#endif

  // Open default SPI device /dev/spidev0.0
  const char *device = "/dev/spidev0.0";
  int fd = open(device, O_RDWR);
  if (fd < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init_default: failed to open %s\n", device);
#endif
    return false;
  }

  // Set SPI mode
  uint8_t mode = cs_active_low ? 0 : SPI_CS_HIGH;
  if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init_default: failed to set SPI mode\n");
#endif
    close(fd);
    return false;
  }

  // Set bits per word
  uint8_t bits = SPI_DEFAULT_BITS_PER_WORD;
  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init_default: failed to set bits per word\n");
#endif
    close(fd);
    return false;
  }

  // Set baud rate
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &baudrate) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init_default: failed to set baud rate\n");
#endif
    close(fd);
    return false;
  }

  // Populate internal SPI structure
  spi_internal_t *internal = SPI_INTERNAL(spi);
  internal->fd = fd;
  internal->adapter = 0;
  internal->cs_active = !cs_active_low;
  internal->baudrate = baudrate;

#ifdef DEBUG
  sys_printf("hw_spi_init_default: success device=%s fd=%d\n", device, fd);
#endif

  return true;
}

/**
 * @brief Initialize a SPI interface with specific adapter and pins.
 *
 * Note: On Linux, SPI pins are hardware-defined and cannot be configured.
 * The pin parameters are ignored. Only adapter number is used.
 */
bool hw_spi_init(hw_spi_t *spi, uint8_t adapter, uint8_t sck, uint8_t tx,
                 uint8_t rx, uint8_t cs, bool cs_active_low,
                 uint32_t baudrate) {
  sys_assert(spi);

  // Clear the structure
  sys_memset(spi, 0, sizeof(hw_spi_t));

  char device[32];

  // Ignore pin parameters on Linux (hardware-defined)
  (void)sck;
  (void)tx;
  (void)rx;
  (void)cs;

#ifdef DEBUG
  sys_printf("hw_spi_init: adapter=%u cs_active_low=%d baudrate=%u\n", adapter,
             cs_active_low, baudrate);
#endif

  // Construct device path: /dev/spidevX.0
  sys_sprintf(device, sizeof(device), "/dev/spidev%u.0", adapter);

  // Open SPI device
  int fd = open(device, O_RDWR);
  if (fd < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init: failed to open %s\n", device);
#endif
    return false;
  }

  // Set SPI mode
  uint8_t mode = cs_active_low ? 0 : SPI_CS_HIGH;
  if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init: failed to set SPI mode\n");
#endif
    close(fd);
    return false;
  }

  // Set bits per word
  uint8_t bits = SPI_DEFAULT_BITS_PER_WORD;
  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init: failed to set bits per word\n");
#endif
    close(fd);
    return false;
  }

  // Set baud rate
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &baudrate) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init: failed to set baud rate\n");
#endif
    close(fd);
    return false;
  }

  // Populate internal SPI structure
  spi_internal_t *internal = SPI_INTERNAL(spi);
  internal->fd = fd;
  internal->adapter = adapter;
  internal->cs_active = !cs_active_low;
  internal->baudrate = baudrate;

#ifdef DEBUG
  sys_printf("hw_spi_init: success device=%s fd=%d\n", device, fd);
#endif

  return true;
}

/**
 * @brief Initialize a SPI interface using a specific device path.
 */
bool hw_spi_init_device(hw_spi_t *spi, const char *path, bool cs_active_low,
                        uint32_t baudrate) {
  sys_assert(spi);
  sys_assert(path);

  // Clear the structure
  sys_memset(spi, 0, sizeof(hw_spi_t));

#ifdef DEBUG
  sys_printf("hw_spi_init_device: path=%s cs_active_low=%d baudrate=%u\n", path,
             cs_active_low, baudrate);
#endif

  // Open SPI device at specified path
  int fd = open(path, O_RDWR);
  if (fd < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init_device: failed to open %s\n", path);
#endif
    return false;
  }

  // Set SPI mode
  uint8_t mode = cs_active_low ? 0 : SPI_CS_HIGH;
  if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init_device: failed to set SPI mode\n");
#endif
    close(fd);
    return false;
  }

  // Set bits per word
  uint8_t bits = SPI_DEFAULT_BITS_PER_WORD;
  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init_device: failed to set bits per word\n");
#endif
    close(fd);
    return false;
  }

  // Set baud rate
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &baudrate) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_init_device: failed to set baud rate\n");
#endif
    close(fd);
    return false;
  }

  // Extract adapter number from path (e.g., "/dev/spidev1.0" -> adapter = 1)
  uint8_t adapter = 0;
  const char *spidev_pos = strstr(path, "spidev");
  if (spidev_pos) {
    adapter = (uint8_t)atoi(spidev_pos + 6); // Skip "spidev"
  }

  // Populate internal SPI structure
  spi_internal_t *internal = SPI_INTERNAL(spi);
  internal->fd = fd;
  internal->adapter = adapter;
  internal->cs_active = !cs_active_low;
  internal->baudrate = baudrate;

#ifdef DEBUG
  sys_printf("hw_spi_init_device: success path=%s fd=%d adapter=%u\n", path, fd,
             adapter);
#endif

  return true;
}

/**
 * @brief Finalize and release a SPI interface.
 */
void hw_spi_finalize(hw_spi_t *spi) {
  sys_assert(spi);

  if (!hw_spi_valid(spi)) {
    return;
  }

  spi_internal_t *internal = SPI_INTERNAL(spi);
  if (internal->fd >= 0) {
#ifdef DEBUG
    sys_printf("hw_spi_finalize: closing fd=%d\n", internal->fd);
#endif
    close(internal->fd);
  }

  // Zero out the structure
  sys_memset(spi, 0, sizeof(hw_spi_t));
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Get true if the SPI interface is valid.
 */
bool hw_spi_valid(hw_spi_t *spi) {
  if (!spi) {
    return false;
  }

  spi_internal_t *internal = SPI_INTERNAL(spi);
  return internal->fd >= 0 && internal->baudrate > 0;
}

/**
 * @brief Get the total number of available SPI adapters.
 */
uint8_t hw_spi_count(void) {
  uint8_t count = 0;
  char device[32];

  // Try to open each /dev/spidevN.0 device to count available adapters
  for (uint8_t adapter = 0; adapter < SPI_MAX_ADAPTERS; adapter++) {
    sys_sprintf(device, sizeof(device), "/dev/spidev%u.0", adapter);
    int fd = open(device, O_RDWR);
    if (fd >= 0) {
      close(fd);
      count++;
    }
  }

  return count;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Perform a SPI transfer operation (read, write, or combined).
 */
size_t hw_spi_xfr(hw_spi_t *spi, void *data, size_t tx, size_t rx) {
  sys_assert(spi);
  sys_assert(data || (tx == 0 && rx == 0));

  if (!hw_spi_valid(spi)) {
    return 0;
  }

  spi_internal_t *internal = SPI_INTERNAL(spi);
  int fd = internal->fd;
  if (fd < 0) {
    return 0;
  }

  struct spi_ioc_transfer xfer[2];
  sys_memset(xfer, 0, sizeof(xfer));
  int num_xfers = 0;

  // Setup transmit transfer
  if (tx > 0) {
    xfer[num_xfers].tx_buf = (unsigned long)data;
    xfer[num_xfers].len = tx;
    xfer[num_xfers].speed_hz = internal->baudrate;
    xfer[num_xfers].bits_per_word = SPI_DEFAULT_BITS_PER_WORD;
    num_xfers++;
  }

  // Setup receive transfer
  if (rx > 0) {
    xfer[num_xfers].rx_buf = (unsigned long)((uint8_t *)data + tx);
    xfer[num_xfers].len = rx;
    xfer[num_xfers].speed_hz = internal->baudrate;
    xfer[num_xfers].bits_per_word = SPI_DEFAULT_BITS_PER_WORD;
    num_xfers++;
  }

  // Perform the transfer
  if (num_xfers > 0) {
    if (ioctl(fd, SPI_IOC_MESSAGE(num_xfers), xfer) < 0) {
#ifdef DEBUG
      sys_printf("hw_spi_xfr: ioctl failed\n");
#endif
      return 0;
    }
  }

  return tx + rx;
}

/**
 * @brief Read data from a specific register of a SPI device.
 */
size_t hw_spi_read(hw_spi_t *spi, uint8_t reg, void *data, size_t len) {
  sys_assert(spi);
  sys_assert(data || len == 0);

  if (!hw_spi_valid(spi)) {
    return 0;
  }

  spi_internal_t *internal = SPI_INTERNAL(spi);
  int fd = internal->fd;
  if (fd < 0) {
    return 0;
  }

  // SPI read: send register address with read bit set (MSB=1)
  uint8_t tx_buf = reg | 0x80; // Set MSB for read operation
  uint8_t *rx_buf = (uint8_t *)data;

  struct spi_ioc_transfer xfer[2];
  sys_memset(xfer, 0, sizeof(xfer));

  // Transfer 1: Send register address
  xfer[0].tx_buf = (unsigned long)&tx_buf;
  xfer[0].len = 1;
  xfer[0].speed_hz = internal->baudrate;
  xfer[0].bits_per_word = SPI_DEFAULT_BITS_PER_WORD;

  // Transfer 2: Receive data
  xfer[1].rx_buf = (unsigned long)rx_buf;
  xfer[1].len = len;
  xfer[1].speed_hz = internal->baudrate;
  xfer[1].bits_per_word = SPI_DEFAULT_BITS_PER_WORD;

  if (ioctl(fd, SPI_IOC_MESSAGE(2), xfer) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_read: ioctl failed\n");
#endif
    return 0;
  }

  return len;
}

/**
 * @brief Write data to a specific register of a SPI device.
 */
size_t hw_spi_write(hw_spi_t *spi, uint8_t reg, const void *data, size_t len) {
  sys_assert(spi);

  if (!hw_spi_valid(spi)) {
    return 0;
  }

  spi_internal_t *internal = SPI_INTERNAL(spi);
  int fd = internal->fd;
  if (fd < 0) {
    return 0;
  }

  // SPI write: send register address with write bit cleared (MSB=0)
  uint8_t tx_buf[256]; // Limited buffer for simplicity
  if (len + 1 > sizeof(tx_buf)) {
#ifdef DEBUG
    sys_printf("hw_spi_write: data too large\n");
#endif
    return 0;
  }

  tx_buf[0] = reg & 0x7F; // Clear MSB for write operation
  if (len > 0 && data) {
    sys_memcpy(&tx_buf[1], data, len);
  }

  struct spi_ioc_transfer xfer;
  sys_memset(&xfer, 0, sizeof(xfer));

  xfer.tx_buf = (unsigned long)tx_buf;
  xfer.len = len + 1;
  xfer.speed_hz = internal->baudrate;
  xfer.bits_per_word = SPI_DEFAULT_BITS_PER_WORD;

  if (ioctl(fd, SPI_IOC_MESSAGE(1), &xfer) < 0) {
#ifdef DEBUG
    sys_printf("hw_spi_write: ioctl failed\n");
#endif
    return 0;
  }

  return len;
}
