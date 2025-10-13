#include "gpio.h"
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes the hardware system on startup.
 */
void hw_init(void) { _hw_gpio_init(); }

/**
 * @brief Cleans up the hardware system on shutdown.
 */
void hw_exit(void) { _hw_gpio_finalize(); }

/**
 * @brief Occasional polling function for the hardware system.
 */
void hw_poll(void) {
  // NO-OP
}
