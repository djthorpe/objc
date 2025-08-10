#include <hardware/adc.h>
#include <runtime-sys/assert.h>
#if PICO_CYW43_SUPPORTED
#include <pico/cyw43_arch.h>
#endif
#include "power.h"
#include "watchdog.h"

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes the hardware system on startup.
 */
void hw_init(void) {
  adc_init();
#if PICO_CYW43_SUPPORTED
  if (cyw43_arch_init()) {
    sys_panicf("cyw43_arch_init failed");
  }
#endif
}

/**
 * @brief Cleans up the hardware system on shutdown.
 */
void hw_exit(void) {
#if PICO_CYW43_SUPPORTED
  cyw43_arch_deinit();
#endif
}

/**
 * @brief Occasional polling function for the hardware system.
 */
void hw_poll(void) {
#if PICO_CYW43_SUPPORTED
  cyw43_arch_poll();
#endif
  _hw_power_poll();
  _hw_watchdog_poll();
}
