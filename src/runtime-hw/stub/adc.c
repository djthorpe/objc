#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Get the total number of available ADC channels.
 */
uint8_t hw_adc_count(void) {
  // ADC not implemented in stub
  return 0;
}

/**
 * @brief Initialize an ADC interface for a specific pin.
 */
hw_adc_t hw_adc_init_pin(uint8_t pin) {
  // ADC not implemented in stub
  (void)pin; // Suppress unused parameter warning
  hw_adc_t adc = {0};
  return adc;
}

/**
 * @brief Initialize an ADC interface on which the temperature sensor is connected.
 */
hw_adc_t hw_adc_init_temperature(void) {
  // ADC not implemented in stub
  hw_adc_t adc = {0};
  return adc;
}

/**
 * @brief Initialize an ADC interface on which the battery level sensor is
 * connected.
 */
hw_adc_t hw_adc_init_battery() {
  // ADC not implemented in stub
  hw_adc_t adc = {0};
  return adc;
}

/**
 * @brief Finalize and release an ADC interface.
 */
void hw_adc_finalize(hw_adc_t *adc) {
  sys_assert(adc);
  // ADC not implemented in stub
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Read the current value from an ADC channel.
 */
uint16_t hw_adc_read(hw_adc_t *adc) {
  sys_assert(adc);
  // ADC not implemented in stub
  return 0; // Always return 0 since reading is not implemented
}

/**
 * @brief Read the current value from an ADC channel as a voltage.
 */
float hw_adc_read_voltage(hw_adc_t *adc) {
  sys_assert(adc);
  return 0.0f; // Not implemented in stub
}

/**
 * @brief Read the current value from an ADC channel as a temperature.
 */
float hw_adc_read_temperature(hw_adc_t *adc) {
  sys_assert(adc);
  return 0.0f; // Not implemented in stub
}
