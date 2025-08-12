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
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Authentication and cipher modes for Wi‑Fi networks.
 * @ingroup WiFi
 *
 * Bitmask describing the advertised/required authentication/cipher modes.
 * Multiple bits may be set if a network supports more than one.
 */
typedef enum {
  hw_wifi_auth_open = 0x0001,      ///< Open (no authentication)
  hw_wifi_auth_wep = 0x0002,       ///< WEP (legacy)
  hw_wifi_auth_wpa_tkip = 0x0004,  ///< WPA‑PSK TKIP
  hw_wifi_auth_wpa_aes = 0x0008,   ///< WPA‑PSK CCMP/AES
  hw_wifi_auth_wpa2_tkip = 0x0010, ///< WPA2‑PSK TKIP
  hw_wifi_auth_wpa2_aes = 0x0020,  ///< WPA2‑PSK CCMP/AES
  hw_wifi_auth_wpa3_sae = 0x0040,  ///< WPA3‑SAE
  hw_wifi_auth_enterprise = 0x0080 ///< 802.1X Enterprise (EAP)
} hw_wifi_auth_t;

/**
 * @brief Describes a discovered Wi‑Fi network (scan result).
 * @ingroup WiFi
 *
 * This is a compact, platform‑neutral representation of a single access point
 * reported during a scan.
 *
 * Notes:
 * - The SSID is a NULL-terminated string. The pointer value may reference a
 *   temporary buffer that is only valid for the duration of the callback; copy
 *   it if you need to retain it after the callback returns.
 * - RSSI is in dBm (negative values typical; higher is better).
 * - The auth field encodes authentication/cipher information; see
 *   hw_wifi_auth_t for details.
 */
typedef struct {
  const char *ssid;    ///< SSID (max 32 bytes) as a NUL‑terminated string
  uint8_t bssid[6];    ///< BSSID (MAC address) in network byte order
  uint8_t channel;     ///< Primary channel number
  int16_t rssi;        ///< Received signal strength (dBm)
  hw_wifi_auth_t auth; ///< Authentication/cipher info (see hw_wifi_auth_t)
} hw_wifi_network_t;

// Opaque Wi‑Fi handle type
typedef struct hw_wifi_t hw_wifi_t;

/** @brief Callback invoked for Wi‑Fi operation notifications.
 *
 * This callback is used by non‑blocking Wi‑Fi APIs (for example, scans)
 * to deliver progress (per‑result) and completion notifications.
 *
 * Semantics for scanning:
 * - For each discovered access point, the callback is invoked with
 *   network != NULL containing the scan result.
 * - When the scan completes, the callback is invoked once more with
 *   network == NULL to signal completion.
 *
 * Keep the callback lightweight and non‑blocking.
 *
 * @param wifi     The Wi‑Fi handle associated with the operation.
 * @param network  A pointer to the current scan result, or NULL to indicate
 *                 the operation has completed.
 * @param user_data Opaque user pointer supplied when the operation started.
 */
typedef void (*hw_wifi_callback_t)(hw_wifi_t *wifi,
                                   const hw_wifi_network_t *network,
                                   void *user_data);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize Wi-Fi management.
 * @ingroup WiFi
 * @param country_code Country code for the Wi-Fi region (e.g., "US", "EU"). If
 * NULL, defaults to "XX" (worldwide).
 * @return Wi-Fi handle or NULL on failure.
 *
 * This function initializes the Wi-Fi management subsystem and returns a
 * handle to the Wi-Fi instance. If initialization fails (for example, if WiFi
 * is not available), it returns NULL.
 */
hw_wifi_t *hw_wifi_init(const char *country_code);

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

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Begin an asynchronous scan for nearby Wi‑Fi networks.
 * @ingroup WiFi
 *
 * Starts a non‑blocking scan. The provided @p callback will be invoked for
 * each result (network != NULL) and once more with network == NULL when the
 * scan completes. The function returns immediately.
 *
 * Notes:
 * - If a scan is already in progress or Wi‑Fi is not up, the function returns
 *   false and no callback will be invoked.
 * - The callback must be lightweight and non‑blocking. Use @p user_data to
 *   carry application state.
 *
 * @param wifi       Initialized Wi‑Fi handle.
 * @param callback   Callback to notify progress/completion (must not be NULL).
 * @param user_data  Opaque user pointer supplied to the callback.
 * @return true if the scan was started, false otherwise.
 */
bool hw_wifi_scan(hw_wifi_t *wifi, hw_wifi_callback_t callback,
                  void *user_data);