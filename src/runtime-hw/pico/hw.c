#include <hardware/adc.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes the hardware system on startup.
 */
void hw_init(void) { adc_init(); }

/**
 * @brief Cleans up the hardware system on shutdown.
 */
void hw_exit(void) {}
