/**
 * @file wifi.h
 * @brief Wi-Fi management interface
 * @defgroup WiFi WiFi
 * @ingroup Hardware
 *
 * Wi-Fi network management interface
 *
 * The Wi-Fi management interface provides discovery of, and connection to
 * networks with authentication.
 */
#pragma once
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Opaque Wi‑Fi handle type
typedef struct hw_wifi_t hw_wifi_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize Wi-Fi management.
 * @ingroup WiFi
 * @return Wi-Fi handle or NULL on failure.
 *
 * This function initializes the Wi-Fi management subsystem and returns a handle
 * to the Wi-Fi instance. If initialization fails (for example, if WiFi is not
 * available), it returns NULL.
 */
hw_wifi_t *hw_wifi_init();

/**
 * @brief Determine if the Wi-Fi handle is initialized and usable.
 * @ingroup WiFi
 * @param wifi Wi-Fi handle
 * @return True if initialized and not finalized; false otherwise.
 */
bool hw_wifi_valid(hw_wifi_t *wifi);

/**
 * @brief Finalize and release any resources.
 * @ingroup WiFi
 * @param wifi Wi-Fi handle
 *
 * Safe to call on an already finalized; in that
 * case it is a no‑op. After finalization, hw_wifi_valid() returns false.
 */
void hw_wifi_finalize(hw_wifi_t *wifi);
