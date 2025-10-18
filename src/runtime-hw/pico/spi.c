#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// INTERNAL STRUCTURE

// Internal SPI state stored in the opaque structure
typedef struct {
  uint8_t adapter;   ///< SPI adapter number
  hw_gpio_t sck;     ///< SPI Clock pin
  hw_gpio_t tx;      ///< SPI Transmit pin (Master Out Slave In)
  hw_gpio_t rx;      ///< SPI Receive pin (Master In Slave Out)
  hw_gpio_t cs;      ///< SPI Chip Select pin
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

  return hw_spi_init(spi, PICO_DEFAULT_SPI, PICO_DEFAULT_SPI_SCK_PIN,
                     PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_RX_PIN,
                     PICO_DEFAULT_SPI_CSN_PIN, cs_active_low, baudrate);
}

/**
 * @brief Initialize a SPI interface with specific adapter and pins.
 */
bool hw_spi_init(hw_spi_t *spi, uint8_t adapter, uint8_t sck, uint8_t tx,
                 uint8_t rx, uint8_t cs, bool cs_active_low,
                 uint32_t baudrate) {
  sys_assert(spi);
  sys_assert(adapter < hw_spi_count());
  sys_assert(sck < hw_gpio_count() && tx < hw_gpio_count() &&
             rx < hw_gpio_count() && cs < hw_gpio_count());
  sys_assert(baudrate > 0);

  // Clear the structure
  sys_memset(spi, 0, sizeof(hw_spi_t));

  // Get internal structure
  spi_internal_t *internal = SPI_INTERNAL(spi);

  // Set the GPIO pins to SPI mode
  internal->sck = hw_gpio_init(sck, HW_GPIO_SPI);
  internal->tx = hw_gpio_init(tx, HW_GPIO_SPI);
  internal->rx = hw_gpio_init(rx, HW_GPIO_SPI);
  bool valid = hw_gpio_valid(&internal->sck) && hw_gpio_valid(&internal->tx) &&
               hw_gpio_valid(&internal->rx);
  if (!valid) {
    return false;
  }

  // If the CS pin is > 0 then use it for chip select
  if (cs > 0) {
    internal->cs = hw_gpio_init(cs, HW_GPIO_OUTPUT);
    internal->cs_active = !cs_active_low;

    // Set CS to inactive state (idle) - which is TRUE/HIGH
    hw_gpio_set(&internal->cs, !internal->cs_active);
  }

  // Initialize the SPI interface
  internal->adapter = adapter;
  internal->baudrate = spi_init(SPI_INSTANCE(adapter), baudrate);

  // Configure SPI mode for e-paper displays (Mode 0: CPOL=0, CPHA=0)
  // This is critical for UC8151 and similar display controllers
  spi_set_format(SPI_INSTANCE(adapter), 8, SPI_CPOL_0, SPI_CPHA_0,
                 SPI_MSB_FIRST);

  return true;
}

/**
 * @brief Initialize a SPI interface using a specific device path.
 * @note This function is not supported on Pico and returns false.
 */
bool hw_spi_init_device(hw_spi_t *spi, const char *path, bool cs_active_low,
                        uint32_t baudrate) {
  (void)path;
  (void)cs_active_low;
  (void)baudrate;

  sys_assert(spi);

  // Clear the structure and return false - device path initialization not
  // supported on Pico
  sys_memset(spi, 0, sizeof(hw_spi_t));
  return false;
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

  // de-initialize the SPI interface and the GPIO pins
  spi_deinit(SPI_INSTANCE(internal->adapter));
  if (hw_gpio_valid(&internal->cs)) {
    hw_gpio_finalize(&internal->cs);
  }
  hw_gpio_finalize(&internal->sck);
  hw_gpio_finalize(&internal->tx);
  hw_gpio_finalize(&internal->rx);

  // Reset the SPI structure
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
  return internal->baudrate > 0;
}

/**
 * @brief Get the total number of available SPI adapters.
 */
uint8_t hw_spi_count(void) { return NUM_SPIS; }

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

  // Assert CS pin if configured
  if (hw_gpio_valid(&internal->cs)) {
    hw_gpio_set(&internal->cs, internal->cs_active);
  }

  size_t bytes_transferred = 0;
  if (tx > 0 && rx > 0) {
    // Perform combined write/read if both tx and rx are specified
    int ret = spi_write_read_blocking(SPI_INSTANCE(internal->adapter), data,
                                      (uint8_t *)data + tx, rx);
    if (ret == (int)rx) {
      bytes_transferred = tx + rx;
    }
  } else if (tx > 0) {
    // Perform write-only operation
    int ret = spi_write_blocking(SPI_INSTANCE(internal->adapter), data, tx);
    if (ret == (int)tx) {
      bytes_transferred = tx;
    }
  } else if (rx > 0) {
    // Perform read-only operation
    int ret =
        spi_read_blocking(SPI_INSTANCE(internal->adapter), 0x00, data, rx);
    if (ret == (int)rx) {
      bytes_transferred = rx;
    }
  }

  // Deassert CS pin if configured
  if (hw_gpio_valid(&internal->cs)) {
    hw_gpio_set(&internal->cs, !internal->cs_active);
  }

  return bytes_transferred;
}

/**
 * @brief Read data from a specific register of a SPI device.
 */
size_t hw_spi_read(hw_spi_t *spi, uint8_t reg, void *data, size_t len) {
  sys_assert(spi);
  sys_assert(data);
  sys_assert(len > 0);

  if (!hw_spi_valid(spi)) {
    return 0;
  }

  spi_internal_t *internal = SPI_INTERNAL(spi);

  // Assert CS pin if configured
  if (hw_gpio_valid(&internal->cs)) {
    hw_gpio_set(&internal->cs, internal->cs_active);
  }

  // Write the register address first
  size_t bytes_transferred = 0;
  int ret = spi_write_blocking(SPI_INSTANCE(internal->adapter), &reg, 1);
  if (ret == 1) {
    // Read the data from the register
    ret = spi_read_blocking(SPI_INSTANCE(internal->adapter), 0x00, data, len);
    if (ret == (int)len) {
      bytes_transferred = len;
    }
  }

  // Deassert CS pin if configured
  if (hw_gpio_valid(&internal->cs)) {
    hw_gpio_set(&internal->cs, !internal->cs_active);
  }

  return bytes_transferred;
}

/**
 * @brief Write data to a specific register of a SPI device.
 */
size_t hw_spi_write(hw_spi_t *spi, uint8_t reg, const void *data, size_t len) {
  sys_assert(spi);
  sys_assert(data || len == 0);

  if (!hw_spi_valid(spi)) {
    return 0;
  }

  spi_internal_t *internal = SPI_INTERNAL(spi);

  // Assert CS pin if configured
  if (hw_gpio_valid(&internal->cs)) {
    hw_gpio_set(&internal->cs, internal->cs_active);
  }

  // Write the register address first
  size_t bytes_transferred = 0;
  int ret = spi_write_blocking(SPI_INSTANCE(internal->adapter), &reg, 1);
  if (ret == 1) {
    if (len > 0) {
      // Write the data to the register
      ret = spi_write_blocking(SPI_INSTANCE(internal->adapter), data, len);
      if (ret == (int)len) {
        bytes_transferred = len;
      }
    } else {
      // Register write only (no data)
      bytes_transferred = 0;
    }
  }

  // Deassert CS pin if configured
  if (hw_gpio_valid(&internal->cs)) {
    hw_gpio_set(&internal->cs, !internal->cs_active);
  }

  return bytes_transferred;
}
