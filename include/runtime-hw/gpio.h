/**
 * @file gpio.h
 * @brief GPIO (General Purpose Input/Output) interface
 * @defgroup GPIO GPIO
 * @ingroup Hardware
 *
 * General Purpose Input/Output (GPIO) interface for hardware platforms.
 * This module provides functions to initialize GPIO pins, set their modes,
 * and handle interrupts.
 *
 * @example pico/gpio_runloop/main.c
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief GPIO mode flags for configuring GPIO pins.
 * @ingroup GPIO
 */
typedef enum {
  HW_GPIO_NONE = 0, ///< No GPIO mode set
  HW_GPIO_INPUT,    ///< GPIO pin floating input
  HW_GPIO_PULLUP,   ///< GPIO pin input with pull-up resistor
  HW_GPIO_PULLDOWN, ///< GPIO pin input with pull-down resistor
  HW_GPIO_OUTPUT,   ///< GPIO pin configured as output
  HW_GPIO_SPI,      ///< GPIO pin configured for SPI
  HW_GPIO_I2C,      ///< GPIO pin configured for I2C
  HW_GPIO_UART,     ///< GPIO pin configured for UART
  HW_GPIO_PWM,      ///< GPIO pin configured for PWM
  HW_GPIO_ADC,      ///< GPIO pin configured for analog input
  HW_GPIO_UNKNOWN,  ///< GPIO pin mode is not implemented
} hw_gpio_mode_t;

/**
 * @brief GPIO mode flags for configuring GPIO pins.
 * @ingroup GPIO
 */
typedef enum {
  HW_GPIO_RISING = (1 << 0),  ///< GPIO pin rising edge interrupt
  HW_GPIO_FALLING = (1 << 1), ///< GPIO pin falling edge interrupt
} hw_gpio_event_t;

/**
 * @brief GPIO logical pin structure.
 * @ingroup GPIO
 * @headerfile gpio.h runtime-hw/hw.h
 */
typedef struct hw_gpio_t {
  uint64_t mask; ///< GPIO pin mask for multiple pins
  uint8_t pin;   ///< GPIO logical pin number
} hw_gpio_t;

/**
 * @brief GPIO interrupt callback function pointer.
 * @ingroup GPIO
 * @param pin The logical pin number that triggered the event.
 * @param event The type of GPIO event that occurred (rising or falling edge).
 * @param userdata User-defined data pointer passed when setting the callback.
 *
 * This callback function is called when a GPIO interrupt occurs on a pin
 * that has been configured for interrupt detection. The callback is executed
 * in interrupt context, so it should be kept short and avoid blocking
 * operations.
 *
 * @see hw_gpio_set_callback() for setting up the callback handler.
 * @see hw_gpio_event_t for available event types.
 */
typedef void (*hw_gpio_callback_t)(uint8_t pin, hw_gpio_event_t event,
                                   void *userdata);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Get the total number of available GPIO pins.
 * @ingroup GPIO
 * @return The number of GPIO pins available on the hardware platform.
 *
 * Returns the number of logical GPIO pins that can be used in the system.
 * These are usually numbered from 0 to hw_gpio_count() - 1.
 * If zero is returned, it indicates that GPIO functionality is not available.
 */
uint8_t hw_gpio_count(void);

/**
 * @brief Validate the GPIO pin.
 * @ingroup GPIO
 * @return True if the GPIO pin is valid, false otherwise.
 *
 * The result of hw_gpio_init can return an empty GPIO structure if the
 * initialization fails. This function checks if the GPIO pin is valid.
 */
static inline bool hw_gpio_valid(hw_gpio_t *gpio) {
  return gpio && gpio->mask > 0;
}

/**
 * @brief Set the global GPIO interrupt callback handler.
 * @ingroup GPIO
 * @param callback Pointer to the callback function to handle GPIO interrupts,
 *                 or NULL to disable interrupt handling.
 * @param userdata User-defined data pointer to pass to the callback.
 *
 * This function registers a global callback handler that will be invoked
 * whenever a GPIO interrupt occurs on any pin that has been configured
 * for interrupt detection. Only one callback can be active at a time.
 *
 * @see hw_gpio_callback_t for callback function signature requirements.
 */
void hw_gpio_set_callback(hw_gpio_callback_t callback, void *userdata);

/**
 * @brief Initialize a GPIO pin with the specified mode.
 * @ingroup GPIO
 * @param pin The logical GPIO pin number to initialize.
 * @param mode The GPIO mode configuration (input, output, pull-up, etc.).
 * @return A GPIO structure representing the initialized pin.
 * @note The pin number should be valid (0 to hw_gpio_count()-1).
 * @see hw_gpio_mode_t for available mode options.
 */
hw_gpio_t hw_gpio_init(uint8_t pin, hw_gpio_mode_t mode);

/**
 * @brief Finalize and release a GPIO pin.
 * @ingroup GPIO
 * @param gpio Pointer to the GPIO structure to finalize.
 * @note After calling this function, the GPIO pin is released and should not be
 * used.
 */
void hw_gpio_finalize(hw_gpio_t *gpio);

/**
 * @brief Get the current mode configuration of a GPIO pin.
 * @ingroup GPIO
 * @param gpio Pointer to the GPIO structure.
 * @return The current GPIO mode configuration.
 */
hw_gpio_mode_t hw_gpio_get_mode(hw_gpio_t *gpio);

/**
 * @brief Set the current mode configuration of a GPIO pin.
 * @ingroup GPIO
 * @param gpio Pointer to the GPIO structure.
 * @param mode The new GPIO mode configuration to set.
 *
 * This function is typically used to change the mode of a pin after it
 * has been initialized.
 */
void hw_gpio_set_mode(hw_gpio_t *gpio, hw_gpio_mode_t mode);

/**
 * @brief Read the current state of a GPIO pin.
 * @ingroup GPIO
 * @param gpio Pointer to the GPIO structure.
 * @return true if the pin is high (logical 1), false if low (logical 0).
 *
 * This function is typically used with pins configured as input.
 */
bool hw_gpio_get(hw_gpio_t *gpio);

/**
 * @brief Set the state of a GPIO pin.
 * @ingroup GPIO
 * @param gpio Pointer to the GPIO structure.
 * @param value true to set the pin high (logical 1), false to set low (logical
 * 0).
 *
 * This function is typically used with pins configured as output.
 */
void hw_gpio_set(hw_gpio_t *gpio, bool value);
