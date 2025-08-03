#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a SPI interface using default pins and adapter.
 */
hw_spi_t hw_spi_init_default(bool cs_active_low, uint32_t baudrate) {
  return hw_spi_init(PICO_DEFAULT_SPI, PICO_DEFAULT_SPI_SCK_PIN,
                     PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_RX_PIN,
                     PICO_DEFAULT_SPI_CSN_PIN, cs_active_low, baudrate);
}

/**
 * @brief Initialize a SPI interface with specific adapter and pins.
 */
hw_spi_t hw_spi_init(uint8_t adapter, uint8_t sck, uint8_t tx, uint8_t rx,
                     uint8_t cs, bool cs_active_low, uint32_t baudrate) {
  sys_assert(adapter < hw_spi_count());
  sys_assert(sck < hw_gpio_count() && tx < hw_gpio_count() &&
             rx < hw_gpio_count() && cs < hw_gpio_count());
  sys_assert(baudrate > 0);

  // Initialize the SPI interface
  hw_spi_t spi = {0};

  // Set the GPIO pins to SPI mode
  spi.sck = hw_gpio_init(sck, HW_GPIO_SPI);
  spi.tx = hw_gpio_init(tx, HW_GPIO_SPI);
  spi.rx = hw_gpio_init(rx, HW_GPIO_SPI);
  bool valid = hw_gpio_valid(&spi.sck) && hw_gpio_valid(&spi.tx) &&
               hw_gpio_valid(&spi.rx);
  sys_assert(valid);

  // If the CS pin is > 0 then use it for chip select
  if (cs > 0) {
    spi.cs = hw_gpio_init(cs, HW_GPIO_OUTPUT);
    spi.cs_active = !cs_active_low;

    // Set CS to inactive state (idle) - which is TRUE/HIGH
    hw_gpio_set(&spi.cs, !spi.cs_active);
  }

  // Initialize the SPI interface
  spi.adapter = adapter;
  spi.baudrate = spi_init(SPI_INSTANCE(adapter), baudrate);

  // Configure SPI mode for e-paper displays (Mode 0: CPOL=0, CPHA=0)
  // This is critical for UC8151 and similar display controllers
  spi_set_format(SPI_INSTANCE(adapter), 8, SPI_CPOL_0, SPI_CPHA_0,
                 SPI_MSB_FIRST);

  // Return the initialized SPI structure
  return spi;
}

/**
 * @brief Finalize and release a SPI interface.
 */
void hw_spi_finalize(hw_spi_t *spi) {
  sys_assert(spi && hw_spi_valid(spi));

  // de-initialize the SPI interface and the GPIO pins
  spi_deinit(SPI_INSTANCE(spi->adapter));
  if (hw_gpio_valid(&spi->cs)) {
    hw_gpio_finalize(&spi->cs);
  }
  hw_gpio_finalize(&spi->sck);
  hw_gpio_finalize(&spi->tx);
  hw_gpio_finalize(&spi->rx);

  // Reset the SPI structure
  sys_memset(spi, 0, sizeof(hw_spi_t));
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

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
  sys_assert(spi && hw_spi_valid(spi));
  sys_assert(data || (tx == 0 && rx == 0));

  // Assert CS pin if configured
  if (hw_gpio_valid(&spi->cs)) {
    hw_gpio_set(&spi->cs, spi->cs_active);
  }

  size_t bytes_transferred = 0;
  if (tx > 0 && rx > 0) {
    // Perform combined write/read if both tx and rx are specified
    int ret = spi_write_read_blocking(SPI_INSTANCE(spi->adapter), data,
                                      (uint8_t *)data + tx, rx);
    if (ret == (int)rx) {
      bytes_transferred = tx + rx;
    }
  } else if (tx > 0) {
    // Perform write-only operation
    int ret = spi_write_blocking(SPI_INSTANCE(spi->adapter), data, tx);
    if (ret == (int)tx) {
      bytes_transferred = tx;
    }
  } else if (rx > 0) {
    // Perform read-only operation
    int ret = spi_read_blocking(SPI_INSTANCE(spi->adapter), 0x00, data, rx);
    if (ret == (int)rx) {
      bytes_transferred = rx;
    }
  }

  // Deassert CS pin if configured
  if (hw_gpio_valid(&spi->cs)) {
    hw_gpio_set(&spi->cs, !spi->cs_active);
  }

  return bytes_transferred;
}

/**
 * @brief Read data from a specific register of a SPI device.
 */
size_t hw_spi_read(hw_spi_t *spi, uint8_t reg, void *data, size_t len) {
  sys_assert(spi && hw_spi_valid(spi));
  sys_assert(data);
  sys_assert(len > 0);

  // Assert CS pin if configured
  if (hw_gpio_valid(&spi->cs)) {
    hw_gpio_set(&spi->cs, spi->cs_active);
  }

  // Write the register address first
  size_t bytes_transferred = 0;
  int ret = spi_write_blocking(SPI_INSTANCE(spi->adapter), &reg, 1);
  if (ret == 1) {
    // Read the data from the register
    ret = spi_read_blocking(SPI_INSTANCE(spi->adapter), 0x00, data, len);
    if (ret == (int)len) {
      bytes_transferred = len;
    }
  }

  // Deassert CS pin if configured
  if (hw_gpio_valid(&spi->cs)) {
    hw_gpio_set(&spi->cs, !spi->cs_active);
  }

  return bytes_transferred;
}

/**
 * @brief Write data to a specific register of a SPI device.
 */
size_t hw_spi_write(hw_spi_t *spi, uint8_t reg, const void *data, size_t len) {
  sys_assert(spi && hw_spi_valid(spi));
  sys_assert(data || len == 0);

  // Assert CS pin if configured
  if (hw_gpio_valid(&spi->cs)) {
    hw_gpio_set(&spi->cs, spi->cs_active);
  }

  // Write the register address first
  size_t bytes_transferred = 0;
  int ret = spi_write_blocking(SPI_INSTANCE(spi->adapter), &reg, 1);
  if (ret == 1) {
    if (len > 0) {
      // Write the data to the register
      ret = spi_write_blocking(SPI_INSTANCE(spi->adapter), data, len);
      if (ret == (int)len) {
        bytes_transferred = len;
      }
    } else {
      // Register write only (no data)
      bytes_transferred = 0;
    }
  }

  // Deassert CS pin if configured
  if (hw_gpio_valid(&spi->cs)) {
    hw_gpio_set(&spi->cs, !spi->cs_active);
  }

  return bytes_transferred;
}
