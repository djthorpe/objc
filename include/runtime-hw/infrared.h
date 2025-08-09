/**
 * @file infrared.h
 * @brief Infrared (IR) receiver interface
 * @defgroup Infrared Infrared
 * @ingroup Hardware
 *
 * Infrared receiver interface for capturing and processing IR remote control
 * signals. This module provides functions to initialize an IR receiver,
 * configure callbacks for signal events, and process MARK/SPACE timing data.
 *
 * The IR receiver uses PIO (Programmable I/O) to achieve high-precision
 * timing measurements of infrared pulses and spaces. Signal events are
 * delivered via callback functions for real-time processing.
 *
 * @example examples/runtime/infrared/main.c
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Infrared receiver.
 * @ingroup Infrared
 * @headerfile infrared.h runtime-hw/hw.h
 */
typedef struct hw_infrared_rx_t {
  uint8_t gpio;    ///< Infrared GPIO pin number
  void *user_data; ///< Pointer to user data
} hw_infrared_rx_t;

/**
 * @brief Infrared events
 * @ingroup Infrared
 * @headerfile infrared.h runtime-hw/hw.h
 */
typedef enum hw_infrared_event_t {
  HW_INFRARED_EVENT_TIMEOUT = (1 << 0),
  HW_INFRARED_EVENT_MARK = (1 << 1),
  HW_INFRARED_EVENT_SPACE = (1 << 2)
} hw_infrared_event_t;

/**
 * @brief Infrared receiver.
 * @ingroup Infrared
 * @headerfile infrared.h runtime-hw/hw.h
 */
typedef void (*hw_infrared_callback_t)(hw_infrared_rx_t *rx,
                                       hw_infrared_event_t event,
                                       uint32_t duration_us);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a Infrared receiver.
 * @ingroup Infrared
 * @param gpio The GPIO pin number for the Infrared receiver.
 * @return A Infrared receiver structure representing the initialized unit.
 *
 * This function initializes an Infrared receiver on the specified GPIO pin.
 * and with the specified user data.
 */
hw_infrared_rx_t hw_infrared_rx_init(uint8_t gpio, void *user_data,
                                     hw_infrared_callback_t callback);

/**
 * @brief Check if an Infrared receiver is valid.
 * @ingroup Infrared
 * @param rx Pointer to the Infrared receiver structure to check.
 * @return true if the Infrared receiver is valid, false otherwise.
 *
 * This function checks if the Infrared receiver has been properly initialized
 * and is ready to use.
 */
bool hw_infrared_rx_valid(hw_infrared_rx_t *rx);

/**
 * @brief Finalize and release an Infrared receiver.
 * @ingroup Infrared
 * @param rx Pointer to the Infrared receiver structure to finalize.
 *
 * This function stops the Infrared receiver and releases all associated
 * resources. After calling this function, the Infrared receiver should not
 * be used.
 */
void hw_infrared_rx_finalize(hw_infrared_rx_t *rx);