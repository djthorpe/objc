#include <hardware/adc.h>
#if CYW43_USES_VSYS_PIN
#include <pico/cyw43_arch.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes the hardware system on startup.
 */
void hw_init(void) {
  adc_init();
#if CYW43_USES_VSYS_PIN
  if (cyw43_arch_init()) {
    sys_panicf("cyw43_arch_init failed");
  }
#endif
}

/**
 * @brief Cleans up the hardware system on shutdown.
 */
void hw_exit(void) {
#if CYW43_USES_VSYS_PIN
  cyw43_arch_deinit();
#endif
}
