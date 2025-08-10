#include <hardware/adc.h>
#include <hardware/gpio.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(PICO_RP2040) || defined(PICO_RP2050A) || defined(PICO_RP2350A)
// Package with ADC-capable GPIOs starting at 26 (RP2040 / RP2050A / RP2350A)
#define ADC_CHANNEL_OFFSET 26
#elif defined(PICO_RP2350B)
// RP2350B package exposes additional GPIOs; ADC GPIOs start at 40
#define ADC_CHANNEL_OFFSET 40
#elif defined(PICO_RP2350)
// RP2350 (generic) but no package variant macro set by board header:
// Try to infer: many RP2350 boards without explicit A/B still map ADC like A.
// Use 26 as safe default; adjust here if a specific board proves different.
#define ADC_CHANNEL_OFFSET 26
#endif

#ifndef ADC_CHANNEL_OFFSET
#warning                                                                       \
    "ADC_CHANNEL_OFFSET not defined for this platform; defaulting to 26. Define PICO_RP2350A or PICO_RP2350B in board header if needed."
#define ADC_CHANNEL_OFFSET 26
#endif

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Get the total number of available ADC channels.
 */
uint8_t hw_adc_count(void) { return NUM_ADC_CHANNELS; }

/**
 * @brief Initialize an ADC interface for a specific pin.
 */
hw_adc_t hw_adc_init_pin(uint8_t pin) {
  sys_assert(pin < hw_gpio_count());
  // Pin must be within the valid ADC channel range. Last channel is
  // reserved for temperature sensor.
  sys_assert(pin >= ADC_CHANNEL_OFFSET &&
             pin < ADC_CHANNEL_OFFSET + NUM_ADC_CHANNELS - 1);

  // Initialize the ADC for the specified pin
  hw_adc_t adc = {0};
  adc.gpio = hw_gpio_init(pin, HW_GPIO_ADC);
  adc.channel = pin - ADC_CHANNEL_OFFSET;

  // Set mode
  hw_gpio_set_mode(&adc.gpio, HW_GPIO_ADC);
  sys_assert(adc.gpio.mask > 0);

  // Return the initialized ADC structure
  return adc;
}

/**
 * @brief Initialize an ADC interface on which the temperature sensor is
 * connected.
 */
hw_adc_t hw_adc_init_temperature() {
  // Temperature sensor is always on ADC_TEMPERATURE_CHANNEL_NUM and not exposed
  // as a GPIO pin
  hw_adc_t adc = {0};
  adc.channel = ADC_TEMPERATURE_CHANNEL_NUM;

  // Enable the temperature sensor
  adc_set_temp_sensor_enabled(true);

  // Return the initialized ADC structure
  return adc;
}

/**
 * @brief Finalize and release an ADC interface.
 */
void hw_adc_finalize(hw_adc_t *adc) {
  sys_assert(adc);

  if (adc->channel == ADC_TEMPERATURE_CHANNEL_NUM) {
    // Disable the temperature sensor
    adc_set_temp_sensor_enabled(false);
  } else if (adc->gpio.mask > 0) {
    // Finalize the GPIO pin if it was initialized
    hw_gpio_finalize(&adc->gpio);
  }
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Get the ADC channel number for a specific GPIO pin.
 */
uint8_t hw_adc_gpio_channel(uint8_t gpio) {
  sys_assert(gpio < hw_gpio_count());

  // Check if the GPIO pin is within the ADC channel range
  if (gpio >= ADC_CHANNEL_OFFSET &&
      gpio < ADC_CHANNEL_OFFSET + NUM_ADC_CHANNELS) {
    return gpio - ADC_CHANNEL_OFFSET; // Return the corresponding ADC channel
  }
  return 0xFF; // Return 0xFF if not a valid ADC GPIO pin
}

/**
 * @brief Read the current value from an ADC channel.
 */
uint16_t hw_adc_read(hw_adc_t *adc) {
  sys_assert(adc);

  // Disable free running mode and set the ADC channel
  adc_run(false);
  adc_select_input(adc->channel);

  // Read the ADC value
  return adc_read();
}

#define ADC_VREF (3.3f)

/**
 * @brief Read the current value from an ADC channel as a voltage.
 */
float hw_adc_read_voltage(hw_adc_t *adc) {
  sys_assert(adc);
  uint16_t value = hw_adc_read(adc);
  const float conversion_factor = ADC_VREF / (float)(1 << 12);

  // If this is the vsys pin, we need to adjust the conversion factor
#ifdef PICO_VSYS_PIN
  if (adc->gpio.pin == PICO_VSYS_PIN) {
    // VSYS pin is typically 3.3V, so we can use a different conversion factor
    return (float)value * 3.0f * conversion_factor;
  }
#endif
  return (float)value * conversion_factor;
}

/**
 * @brief Read the current value from an ADC channel as a temperature
 */
float hw_adc_read_temperature(hw_adc_t *adc) {
  sys_assert(adc);
  sys_assert(adc->channel == ADC_TEMPERATURE_CHANNEL_NUM);

  // RP2040/RP2350 temperature sensor formula:
  // T = 27 - (V_sense - 0.706) / 0.001721
  // Where: V_sense is the voltage from the temperature sensor
  //        0.706V is the voltage at 27°C
  //        0.001721 V/°C is the temperature coefficient
  float value = hw_adc_read_voltage(adc);
  float celsius = 27.0f - (value - 0.706f) / 0.001721f;
  return celsius;
}
