#include "infrared_rx.h"
#include "infrared_rx.pio.h"
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize an infrared receiver.
 */
hw_infrared_rx_t hw_infrared_rx_init(uint8_t gpio,
                                     hw_infrared_rx_callback_t callback,
                                     void *user_data) {
  sys_assert(gpio < hw_gpio_count());
  sys_assert(callback);

  // Initialize receiver structure first
  hw_infrared_rx_t rx = {0};

  // Try and initialize the PIO program with interrupt support
  rx.ctx = _hw_infrared_rx_pio_init(gpio, callback, user_data);

  // Note: If initialization fails, rx.ctx will be NULL
  // Caller should check with hw_infrared_rx_valid()
  return rx;
}

/**
 * @brief Check if an Infrared receiver is valid.
 */
bool hw_infrared_rx_valid(hw_infrared_rx_t *rx) {
  if (rx && rx->ctx != NULL) {
    return true;
  }
  return false;
}

/**
 * @brief Finalize and release an Infrared receiver.
 */
void hw_infrared_rx_finalize(hw_infrared_rx_t *rx) {
  // Ignore if not valid
  if (!hw_infrared_rx_valid(rx)) {
    return;
  }

  _hw_infrared_rx_pio_finalize(rx->ctx);
  rx->ctx = NULL;
}
