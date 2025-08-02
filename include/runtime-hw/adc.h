/**
 * @file adc.h
 * @brief ADC (Analog-to-Digital Converter) interface
 * @defgroup ADC ADC
 * @ingroup Hardware
 *
 * Analog-to-Digital Converter (ADC) interface for hardware platforms.
 * This module provides functions to initialize and read from ADC peripherals.
 */
#pragma once
#include "gpio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief ADC channel.
 * @ingroup ADC
 */
typedef struct hw_adc_t {
  uint8_t channel; ///< ADC channel number (0, 1, etc.)
  hw_gpio_t gpio;  ///< ADC GPIO pin number
} hw_adc_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Get the total number of available ADC channels.
 * @ingroup ADC
 * @return The number of ADC channels available on the hardware platform.
 *
 * Returns the number of logical ADC channels that can be used in the system.
 * These are usually numbered from 0 to hw_adc_count() - 1.
 * If zero is returned, it indicates that ADC functionality is not available.
 */
uint8_t hw_adc_count(void);

/**
 * @brief Initialize an ADC interface for a specific pin.
 * @ingroup ADC
 * @return An ADC structure representing the initialized interface.
 *
 * This function initializes the ADC interface for a specific channel and pin,
 * allowing for flexible configuration of the ADC hardware.
 */
hw_adc_t hw_adc_init_pin(uint8_t pin);

/**
 * @brief Initialize an ADC interface on which the temperature sensor is
 * connected.
 * @ingroup ADC
 * @return An ADC structure representing the initialized interface.
 *
 * This function initializes the ADC interface which is used for measuring the
 * temperature sensor.
 */
hw_adc_t hw_adc_init_temperature();

/**
 * @brief Initialize an ADC interface on which the battery level sensor is
 * connected.
 * @ingroup ADC
 * @return An ADC structure representing the initialized interface.
 *
 * This function initializes the ADC interface which is used for measuring the
 * battery level sensor (aka VSYS pin). This is typically used to monitor
 * the battery voltage level.
 *
 * If not defined, this function will return an empty ADC structure.
 */
hw_adc_t hw_adc_init_battery();

/**
 * @brief Finalize and release an ADC interface.
 * @ingroup ADC
 * @param adc Pointer to the ADC structure to finalize.
 *
 * This function releases the ADC interface and frees any associated resources.
 * After calling this function, the ADC interface should not be used for
 * further operations.
 */
void hw_adc_finalize(hw_adc_t *adc);

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Read the current value from an ADC channel.
 * @ingroup ADC
 * @param adc Pointer to the ADC structure.
 * @return The raw ADC value (0-4095 for 12-bit ADC).
 *
 * This function reads the current value from the specified ADC channel and
 * returns the raw digital value corresponding to the analog input voltage.
 */
uint16_t hw_adc_read(hw_adc_t *adc);

/**
 * @brief Read the current value from an ADC channel as a voltage.
 * @ingroup ADC
 * @param adc Pointer to the ADC structure.
 * @return The voltage value in volts (typically 0.0 to 3.3V range).
 *
 * This function reads the current value from the specified ADC channel and
 * converts the raw digital value to a voltage measurement.
 *
 * The conversion is based on the reference voltage of the
 * ADC (typically 3.3V for most microcontrollers), except for the battery
 * voltage pin which uses a voltage divider to scale the input.
 */
float hw_adc_read_voltage(hw_adc_t *adc);

/**
 * @brief Read the current value from an ADC channel as a temperature.
 * @ingroup ADC
 * @param adc Pointer to the ADC structure configured for temperature sensing.
 * @return The temperature value in degrees Celsius.
 *
 * This function reads the current value from an ADC channel that is connected
 * to a temperature sensor and converts the reading to a temperature value.
 * This function is typically used with the ADC channel initialized by
 * hw_adc_init_temperature().
 */
float hw_adc_read_temperature(hw_adc_t *adc);
