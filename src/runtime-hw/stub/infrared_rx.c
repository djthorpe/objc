#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a Infrared receiver.
 */
hw_infrared_rx_t hw_infrared_rx_init(uint8_t gpio,
                                     hw_infrared_rx_callback_t callback,
                                     void *user_data) {
  // Infrared not implemented in stub
  (void)gpio;      // Suppress unused parameter warning
  (void)callback;  // Suppress unused parameter warning
  (void)user_data; // Suppress unused parameter warning

  hw_infrared_rx_t rx = {0};
  // Return invalid receiver - ctx will be NULL indicating failure
  return rx;
}

/**
 * @brief Check if an Infrared receiver is valid.
 */
bool hw_infrared_rx_valid(hw_infrared_rx_t *rx) {
  // Always return false in stub implementation
  (void)rx; // Suppress unused parameter warning
  return false;
}

/**
 * @brief Finalize and release an Infrared receiver.
 */
void hw_infrared_rx_finalize(hw_infrared_rx_t *rx) {
  // Infrared not implemented in stub
  if (rx) {
    sys_memset(rx, 0, sizeof(hw_infrared_rx_t));
  }
}
