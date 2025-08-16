/**
 * @file wifi.h
 * @brief Wi-Fi management interface
 * @defgroup WiFi WiFi
 * @ingroup Hardware
 *
 * Wi-Fi network management interface
 *
 * The Wi-Fi management interface provides discovery of, and connection to
 * networks with authentication. The three main methods for connecting,
 * disconnecting and scanning are asynchronous and call the provided callback
 * function to update operation status.
 *
 * When connecting, the callback will be invoked with the current status of the
 * connection attempt, including any relevant network information. The callback
 * will then be called occasionally with updates on the connection status (for
 * example, the signal strength).
 *
 * When scanning, the callback will be invoked with the results of the scan,
 * including information about any discovered networks. The scan is completed
 * when the callback is invoked with a NULL network pointer.
 *
 * @example examples/runtime/wificonnect/main.c
 * @example examples/runtime/wifiscan/main.c
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

#define HW_WIFI_SSID_MAX_LENGTH 32

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
  hw_wifi_auth_open = (1 << 0),      ///< Open (no authentication)
  hw_wifi_auth_wep = (1 << 1),       ///< WEP (legacy)
  hw_wifi_auth_wpa_tkip = (1 << 2),  ///< WPA‑PSK TKIP
  hw_wifi_auth_wpa_aes = (1 << 3),   ///< WPA‑PSK CCMP/AES
  hw_wifi_auth_wpa2_tkip = (1 << 4), ///< WPA2‑PSK TKIP
  hw_wifi_auth_wpa2_aes = (1 << 5),  ///< WPA2‑PSK CCMP/AES
  hw_wifi_auth_wpa3_sae = (1 << 6),  ///< WPA3‑SAE
  hw_wifi_auth_enterprise = (1 << 7) ///< 802.1X Enterprise (EAP)
} hw_wifi_auth_t;

/**
 * @brief Callback events.
 * @ingroup WiFi
 */
typedef enum {
  hw_wifi_event_scan = (1 << 0),         ///< Scan result available
  hw_wifi_event_joining = (1 << 1),      ///< Joining a network
  hw_wifi_event_connected = (1 << 2),    ///< Successfully connected
  hw_wifi_event_disconnected = (1 << 3), ///< Disconnected
  hw_wifi_event_badauth =
      (1 << 4), ///< Bad authentication during connection attempt
  hw_wifi_event_notfound = (1 << 5), ///< Network not found
  hw_wifi_event_error = (1 << 6)     ///< Other error occurred
} hw_wifi_event_t;

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
  char ssid[HW_WIFI_SSID_MAX_LENGTH +
            1];        ///< SSID (max 32 bytes) plus NULL termination
  uint8_t bssid[6];    ///< BSSID (MAC address) in network byte order
  hw_wifi_auth_t auth; ///< Authentication/cipher info (see hw_wifi_auth_t)
  uint8_t channel;     ///< Primary channel number
  int16_t rssi;        ///< Received signal strength (dBm)
} hw_wifi_network_t;

// Opaque Wi‑Fi handle type
typedef struct hw_wifi_t hw_wifi_t;

/**
 * @brief Callback invoked for Wi‑Fi operation notifications.
 * @param wifi     The Wi‑Fi handle associated with the operation.
 * @param event    The event type (see hw_wifi_event_t).
 * @param network  When event is hw_wifi_event_scan, this contains a
 *                pointer to the current scan result, or NULL to indicate
 *                the scan operation has completed.
 * @param user_data Opaque user pointer supplied when the operation started.
 *
 * This callback is used when connecting or disconnecting from a network,
 * and when scanning for networks.
 */
typedef void (*hw_wifi_callback_t)(hw_wifi_t *wifi, hw_wifi_event_t event,
                                   const hw_wifi_network_t *network,
                                   void *user_data);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize Wi-Fi management.
 * @ingroup WiFi
 * @param country_code Country code for the Wi-Fi region (e.g., "US", "EU"). If
 * NULL, defaults to "XX" (worldwide).
 * @param callback Callback to notify progress/completion of connection,
 * disconnection and scanning asyncronous operations (must not be NULL).
 * @param user_data Opaque user pointer supplied to the callback.
 * @return Wi-Fi handle or NULL on failure.
 *
 * This function initializes the Wi-Fi management subsystem and returns a
 * handle to the Wi-Fi instance. If initialization fails (for example, if WiFi
 * is not available), it returns NULL.
 */
hw_wifi_t *hw_wifi_init(const char *country_code, hw_wifi_callback_t callback,
                        void *user_data);

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
 * @param wifi       Initialized Wi‑Fi handle.
 * @return true if the scan was started, false otherwise.
 *
 * Starts a non‑blocking scan. The provided @p callback will be invoked for
 * each result (network != NULL) and once more with network == NULL when the
 * scan completes. The function returns immediately.
 *
 * If a operation (connection, disconnection or scanning) is already in progress
 * or Wi‑Fi is disconnected, the function returns false and no callback will be
 * invoked.
 */
bool hw_wifi_scan(hw_wifi_t *wifi);

/**
 * @brief Begin an asynchronous connection to a Wi‑Fi network.
 * @ingroup WiFi
 * @param wifi      Initialized Wi‑Fi handle.
 * @param network   Target network to connect to (SSID, BSSID, etc).
 * @param password  NUL‑terminated password string (may be empty for open
 * networks).
 * @return true if the connection attempt was started, false otherwise.
 *
 * Initiates a non‑blocking connection attempt to the specified network using
 * the provided password. The callback will be invoked to report connection
 * progress and completion. The function returns immediately.
 *
 * The ssid and auth parameters must be filled in by the caller, and the bssid
 * parameter can optionally be set to the BSSID of the target access point (if
 * known).
 *
 * The @p password must be a NUL‑terminated string (may be empty or NULL for
 * open networks).
 *
 * Only one connection attempt may be active at a time per Wi‑Fi handle, and
 * false will be returned if a connection attempt or scanning is already in
 * progress.
 */
bool hw_wifi_connect(hw_wifi_t *wifi, const hw_wifi_network_t *network,
                     const char *password);

/**
 * @brief Disconnect from a previously-connected Wi‑Fi network.
 * @ingroup WiFi
 * @param wifi Initialized Wi‑Fi handle.
 * @return true if a disconnect was initiated, false if not connected.
 *
 * Initiates a disconnect from the current network. The function returns
 * immediately. If not currently connected, this is a no‑op, and if a connection
 * attempt or scan is in progress, it will be aborted and false will be
 * returned.
 */
bool hw_wifi_disconnect(hw_wifi_t *wifi);
