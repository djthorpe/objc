#include <runtime-hw/wifi.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#ifdef PICO_CYW43_SUPPORTED
#include "cyw43.h"
#include "cyw43_country.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Flags for Wi-Fi state.
 */
typedef enum {
  hw_wifi_flag_scanning = (1 << 1), ///< Scanning
  hw_wifi_flag_joining = (1 << 2),  ///< Joining
  hw_wifi_flag_leaving = (1 << 3),  ///< Leaving
} hw_wifi_state_t;

/**
 * @brief Handle for Wi-Fi state.
 */
struct hw_wifi_t {
  const char *country_code;
  hw_wifi_callback_t callback;
  void *user_data;
  sys_atomic_t flags;
  int state;
  hw_wifi_network_t network;
  uint64_t ts;
};

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

/**
 * @brief Return true if the link is up
 */
static inline bool _hw_wifi_up(hw_wifi_t *wifi) {
  (void)wifi;
#ifdef PICO_CYW43_SUPPORTED
  return cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP;
#else
  return false;
#endif
}

/**
 * @brief Return true if busy (leaving, joining, scanning)
 */
static inline bool _hw_wifi_get_busy(hw_wifi_t *wifi, hw_wifi_state_t state) {
  return (sys_atomic_get(&wifi->flags) & state) != 0;
}

/**
 * @brief Set busy state
 */
static inline void _hw_wifi_set_busy(hw_wifi_t *wifi, hw_wifi_state_t state,
                                     bool busy) {
  if (busy) {
    sys_atomic_set_bits(&wifi->flags, state);
  } else {
    sys_atomic_clear_bits(&wifi->flags, state);
  }
}

/**
 * @brief Return country code as uint32_t from string
 */
static uint32_t _hw_wifi_country_code(const char *country_code);

/**
 * @brief Forward declare the scanning callback
 */
static int _hw_wifi_scan_callback(void *env,
                                  const cyw43_ev_scan_result_t *result);

/**
 * @brief Get channel for connected wifi
 */
static uint8_t _hw_wifi_get_channel(hw_wifi_t *wifi);

/**
 * @brief Get bssid for connected wifi
 */
static void _hw_wifi_get_bssid(hw_wifi_t *wifi, uint8_t bssid[6]);

/**
 * @brief Get bssid for connected wifi
 */
static uint16_t _hw_wifi_get_rssi(hw_wifi_t *wifi);

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

#define HW_WIFI_COUNTRY_CODE_DEFAULT "XX"
#define HW_WIFI_STATUS_INTERVAL_MS                                             \
  (1000 * 60) // report connected status every minute

// The wifi singleton
static hw_wifi_t _hw_wifi = {0};

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize Wi-Fi management.
 */
hw_wifi_t *hw_wifi_init(const char *country_code, hw_wifi_callback_t callback,
                        void *user_data) {
  sys_assert(callback);

  // Reset the wifi handle
  hw_wifi_finalize(&_hw_wifi);

  // Country code check
  if (country_code == NULL) {
    country_code = HW_WIFI_COUNTRY_CODE_DEFAULT;
  }
  if (strlen(country_code) != 2) {
    // Invalid country code length
    return NULL;
  }

  // Set up the structure
#ifdef PICO_CYW43_SUPPORTED
  // Check to make sure initialized
  if (cyw43_is_initialized(&cyw43_state) == false) {
    return NULL;
  }
#endif

  // Set up the structure
  _hw_wifi.country_code = country_code;
  _hw_wifi.callback = callback;
  _hw_wifi.user_data = user_data;
  sys_atomic_init(&_hw_wifi.flags, 0);

  // Return the Wi-Fi handle
  return &_hw_wifi;
}

/**
 * @brief Validate the Wi-Fi state.
 */
bool hw_wifi_valid(hw_wifi_t *wifi) {
  if (wifi == NULL || wifi->country_code == NULL) {
    return false;
  }
  return true;
}

/**
 * @brief Finalize Wi-Fi management.
 */
void hw_wifi_finalize(hw_wifi_t *wifi) {
  if (hw_wifi_valid(wifi) == false) {
    return;
  }

#ifdef PICO_CYW43_SUPPORTED
  // Leave Wi-Fi network
  if (_hw_wifi_up(wifi)) {
    (void)cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
    cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, false,
                      _hw_wifi_country_code(wifi->country_code));
  }
#endif

  // Clear the Wi-Fi state
  sys_memset(wifi, 0, sizeof(hw_wifi_t));
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Initiate a Wi-Fi scan.
 */
bool hw_wifi_scan(hw_wifi_t *wifi) {
  sys_assert(hw_wifi_valid(wifi));
  bool success = false;

  // If we're already leaving, joining or scanning, don't init a new scan
  if (_hw_wifi_get_busy(wifi, hw_wifi_flag_leaving | hw_wifi_flag_joining |
                                  hw_wifi_flag_scanning)) {
    return false;
  }

#ifdef PICO_CYW43_SUPPORTED
  if (_hw_wifi_up(wifi) == false) {
    // Bring Wi‑Fi up in STA mode
    cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, true,
                      _hw_wifi_country_code(wifi->country_code));
  }

  // TODO: set power management cyw43_wifi_pm

  // Zero-initialize scan options
  cyw43_wifi_scan_options_t opts;
  sys_memset(&opts, 0, sizeof(opts));

  // Pass the wifi handle as the callback environment
  if (cyw43_wifi_scan(&cyw43_state, &opts, wifi, _hw_wifi_scan_callback) == 0) {
    _hw_wifi_set_busy(wifi, hw_wifi_flag_scanning, true);
    wifi->state = -1;
    success = true;
  }
#endif

  return success;
}

/**
 * @brief Disconnect from a previously-connected Wi‑Fi network.
 */
bool hw_wifi_disconnect(hw_wifi_t *wifi) {
  sys_assert(hw_wifi_valid(wifi));
  bool success = false;

#ifdef PICO_CYW43_SUPPORTED
  // If we're already leaving, joining or scanning, don't disconnect
  if (_hw_wifi_get_busy(wifi, hw_wifi_flag_leaving | hw_wifi_flag_joining |
                                  hw_wifi_flag_scanning)) {
    return false;
  }

  // If link is down, then already disconnected
  if (_hw_wifi_up(wifi) == false) {
    if (wifi->callback) {
      wifi->callback(wifi, hw_wifi_event_disconnected, NULL, wifi->user_data);
      sys_memset(&wifi->network, 0, sizeof(wifi->network));
    }
    return true;
  }

  // Leave the network
  if (cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA) == 0) {
    _hw_wifi_set_busy(wifi, hw_wifi_flag_leaving, true);
    wifi->state = -1;
    success = true;
  }

  // TODO: set power management cyw43_wifi_pm
#endif
  return success;
}

/**
 * @brief Begin an asynchronous connection to a Wi‑Fi network.
 */
bool hw_wifi_connect(hw_wifi_t *wifi, hw_wifi_network_t network,
                     const char *password) {
  sys_assert(hw_wifi_valid(wifi));
  bool success = false;

#ifdef PICO_CYW43_SUPPORTED
  // If we're already leaving, joining or scanning, return false
  if (_hw_wifi_get_busy(wifi, hw_wifi_flag_leaving | hw_wifi_flag_joining |
                                  hw_wifi_flag_scanning)) {
    return false;
  }

  // If link is down, then set-up
  if (_hw_wifi_up(wifi) == false) {
    // Bring Wi‑Fi up in STA mode
    cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, true,
                      _hw_wifi_country_code(wifi->country_code));
  }

  // TODO: set power management cyw43_wifi_pm

  // If auth is zero, choose a sensible default
  uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
  if (network.auth != 0) {
    auth = network.auth;
  }

  // Set BSSID to NONE if all elements of network.bssid are zero
  const uint8_t *bssid = NULL;
  if (sys_memcmp(network.bssid, (const uint8_t[6]){0}, sizeof(network.bssid)) ==
      0) {
    bssid = NULL;
  } else {
    bssid = network.bssid;
  }

  // Ignore channel
  uint32_t channel = CYW43_CHANNEL_NONE;

  // Prepare SSID length (bounded) and password pointer/length (allow NULL)
  size_t ssid_len = strnlen(network.ssid, sizeof(network.ssid) - 1);
  const char *key = (password != NULL) ? password : "";
  size_t key_len = (password != NULL) ? strlen(password) : 0;

  // Use the provided SSID and password to join the network
  if (cyw43_wifi_join(&cyw43_state, (uint32_t)ssid_len,
                      (const uint8_t *)network.ssid, (uint32_t)key_len,
                      (const uint8_t *)key, auth, bssid, channel) == 0) {
    _hw_wifi_set_busy(wifi, hw_wifi_flag_joining, true);
    wifi->state = -1;
    success = true;
  }

  // Set the network information
  if (success) {
    sys_memcpy(wifi->network.ssid, network.ssid, sizeof(network.ssid));
    sys_memcpy(wifi->network.bssid, network.bssid, sizeof(network.bssid));
    wifi->network.channel = channel;
    wifi->network.rssi = 0;
    wifi->network.auth = auth;
  } else {
    sys_memset(&wifi->network, 0, sizeof(wifi->network));
  }
#endif

  return success;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

static uint32_t _hw_wifi_country_code(const char *country_code) {
#ifdef PICO_CYW43_SUPPORTED
  if (country_code == NULL || strlen(country_code) != 2) {
    return 0;
  } else {
    return CYW43_COUNTRY(country_code[0], country_code[1], 0);
  }
#else
  (void)country_code;
  return 0;
#endif
}

uint8_t _hw_wifi_get_channel(hw_wifi_t *wifi) {
  uint32_t channel = 0;
#ifdef PICO_CYW43_SUPPORTED
  cyw43_ioctl(&cyw43_state, CYW43_IOCTL_GET_CHANNEL, sizeof(channel),
              (uint8_t *)&channel, CYW43_ITF_STA);
#endif
  return (uint8_t)channel;
}

/**
 * @brief Get bssid for connected wifi
 */
static void _hw_wifi_get_bssid(hw_wifi_t *wifi, uint8_t bssid[6]) {
  sys_assert(hw_wifi_valid(wifi));
  sys_assert(bssid != NULL);
  sys_memset(bssid, 0, sizeof(bssid));
#ifdef PICO_CYW43_SUPPORTED
  cyw43_wifi_get_bssid(&cyw43_state, bssid);
#endif
}

/**
 * @brief Get bssid for connected wifi
 */
static uint16_t _hw_wifi_get_rssi(hw_wifi_t *wifi) {
  sys_assert(hw_wifi_valid(wifi));
  uint32_t rssi = 0;
  if (cyw43_wifi_get_rssi(&cyw43_state, &rssi) == 0) {
    return (uint16_t)rssi;
  }
  return 0;
}

#ifdef PICO_CYW43_SUPPORTED
static int _hw_wifi_scan_callback(void *ctx,
                                  const cyw43_ev_scan_result_t *result) {
  static hw_wifi_network_t network = {0};
  hw_wifi_t *wifi = (hw_wifi_t *)ctx;
  sys_assert(hw_wifi_valid(wifi));

  // Stop scanning if callback is NULL
  if (wifi->callback == NULL) {
    return -1;
  }

  // Driver may invoke with result == NULL to indicate completion; we ignore
  // here and rely on poll to notify completion.
  if (result == NULL) {
    return 0;
  }

  // Copy hw_wifi_network_t ssid (clamped)
  size_t ssid_len = result->ssid_len;
  if (ssid_len >= sizeof(network.ssid)) {
    ssid_len = sizeof(network.ssid) - 1;
  }
  sys_memcpy(network.ssid, result->ssid, ssid_len);
  network.ssid[ssid_len] = '\0';

  // Copy hw_wifi_network_t bssid
  sys_assert(sizeof(network.bssid) == sizeof(result->bssid));
  sys_memcpy(network.bssid, result->bssid, sizeof(network.bssid));

  // Copy hw_wifi_network_t channel and rssi
  network.channel = result->channel;
  network.rssi = (int16_t)result->rssi;

  // Map auth: compare using 8-bit value, guard WPA3 macro size
  network.auth = 0;
  uint8_t am = result->auth_mode;
  if (am == (uint8_t)CYW43_AUTH_OPEN) {
    network.auth = hw_wifi_auth_open;
  } else if (am == (uint8_t)CYW43_AUTH_WPA_TKIP_PSK) {
    network.auth = hw_wifi_auth_wpa_tkip;
  } else if (am == (uint8_t)CYW43_AUTH_WPA2_AES_PSK ||
             am == (uint8_t)CYW43_AUTH_WPA2_MIXED_PSK) {
    network.auth = hw_wifi_auth_wpa2_aes;
  }
#if defined(CYW43_AUTH_WPA3_SAE_AES_PSK)
  else if (am == (uint8_t)CYW43_AUTH_WPA3_SAE_AES_PSK) {
    network.auth = hw_wifi_auth_wpa3_sae;
  }
#elif defined(CYW43_AUTH_WPA3_SAE_PSK)
  else if (am == (uint8_t)CYW43_AUTH_WPA3_SAE_PSK) {
    network.auth = hw_wifi_auth_wpa3_sae;
  }
#endif

  // Callback
  wifi->callback(wifi, hw_wifi_event_scan, &network, wifi->user_data);

  // Continue scanning
  return 0;
}
#endif

#ifdef PICO_CYW43_SUPPORTED
void _hw_wifi_poll(void) {
  // Only act on our singleton
  hw_wifi_t *wifi = &_hw_wifi;
  if (!hw_wifi_valid(wifi)) {
    return;
  }

  // If the timestamp field is > 0 then report occasionally on
  // connection status
  if (wifi->ts > 0) {
    uint64_t now = sys_date_get_timestamp();
    if (now - wifi->ts > HW_WIFI_STATUS_INTERVAL_MS) {
      wifi->ts = now;
      if (_hw_wifi_up(wifi)) {
        // Update the network
        wifi->network.rssi = _hw_wifi_get_rssi(wifi);
        _hw_wifi_get_bssid(wifi, wifi->network.bssid);
        wifi->network.channel = _hw_wifi_get_channel(wifi);

        // Callback
        wifi->callback(wifi, hw_wifi_event_connected, &wifi->network,
                       wifi->user_data);
      }
    }
  }

  // If we're not joining, leaving or scanning, then quit this
  if (_hw_wifi_get_busy(wifi, hw_wifi_flag_leaving | hw_wifi_flag_joining |
                                  hw_wifi_flag_scanning) == 0) {
    return;
  }

  // If we're scanning and scan becomes inactive then end the scanning
  if (_hw_wifi_get_busy(wifi, hw_wifi_flag_scanning)) {
    if (cyw43_wifi_scan_active(&cyw43_state) == false) {
      _hw_wifi_set_busy(wifi, hw_wifi_flag_scanning, false);
      wifi->callback(wifi, hw_wifi_event_scan, NULL,
                     wifi->user_data); // Notify scan completion
    }
    return;
  }

  // Get current link state, and act if it's changed
  int state = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
  if (state == wifi->state) {
    return;
  } else {
    wifi->state = state;
  }

  // Change state
  switch (state) {
  case CYW43_LINK_DOWN:
#ifdef DEBUG
    sys_printf("CYW43_LINK_DOWN\n");
#endif
    // Ends the connection, joining or scanning attempt
    _hw_wifi_set_busy(wifi,
                      hw_wifi_flag_joining | hw_wifi_flag_leaving |
                          hw_wifi_flag_scanning,
                      false);
    wifi->state = -1;
    wifi->callback(wifi, hw_wifi_event_disconnected, NULL, wifi->user_data);
    sys_memset(&wifi->network, 0, sizeof(wifi->network));
    break;
  case CYW43_LINK_JOIN:
#ifdef DEBUG
    sys_printf("CYW43_LINK_JOIN\n");
#endif
    // Starts a connection attempt
    _hw_wifi_set_busy(wifi, hw_wifi_flag_joining, true);
    wifi->callback(wifi, hw_wifi_event_joining, &wifi->network,
                   wifi->user_data);
    break;
  case CYW43_LINK_NOIP:
    // Continues connection attempt
#ifdef DEBUG
    sys_printf("CYW43_LINK_NOIP\n");
#endif
    break;
  case CYW43_LINK_UP:
    // Ends the connection attempt successfully
#ifdef DEBUG
    sys_printf("CYW43_LINK_UP\n");
#endif
    if (_hw_wifi_get_busy(wifi, hw_wifi_flag_joining)) {
      _hw_wifi_set_busy(wifi, hw_wifi_flag_joining, false);
      wifi->state = -1;

      // Update the network
      wifi->network.rssi = _hw_wifi_get_rssi(wifi);
      _hw_wifi_get_bssid(wifi, wifi->network.bssid);
      wifi->network.channel = _hw_wifi_get_channel(wifi);

      // Set the timestamp to update the network values
      wifi->ts = sys_date_get_timestamp();

      // Callback
      wifi->callback(wifi, hw_wifi_event_connected, &wifi->network,
                     wifi->user_data);
    }
    break;
  case CYW43_LINK_FAIL:
    // Ends the connection attempt unsuccessfully
#ifdef DEBUG
    sys_printf("CYW43_LINK_FAIL\n");
#endif
    _hw_wifi_set_busy(wifi, hw_wifi_flag_joining, false);
    wifi->state = -1;
    wifi->callback(wifi, hw_wifi_event_error, &wifi->network, wifi->user_data);
    sys_memset(&wifi->network, 0, sizeof(wifi->network));
    break;
  case CYW43_LINK_NONET:
    // Ends the connection attempt unsuccessfully
#ifdef DEBUG
    sys_printf("CYW43_LINK_NONET\n");
#endif
    _hw_wifi_set_busy(wifi, hw_wifi_flag_joining, false);
    wifi->state = -1;
    wifi->callback(wifi, hw_wifi_event_notfound, &wifi->network,
                   wifi->user_data);
    sys_memset(&wifi->network, 0, sizeof(wifi->network));
    break;
  case CYW43_LINK_BADAUTH:
    // Ends the connection attempt unsuccessfully
#ifdef DEBUG
    sys_printf("CYW43_LINK_BADAUTH\n");
#endif
    _hw_wifi_set_busy(wifi, hw_wifi_flag_joining, false);
    wifi->state = -1;
    wifi->callback(wifi, hw_wifi_event_badauth, &wifi->network,
                   wifi->user_data);
    sys_memset(&wifi->network, 0, sizeof(wifi->network));
    break;
  default:
    // Ends the connection, joining or scanning attempt
#ifdef DEBUG
    sys_printf("CYW43_LINK_UNKNOWN\n");
#endif
    _hw_wifi_set_busy(wifi,
                      hw_wifi_flag_joining | hw_wifi_flag_leaving |
                          hw_wifi_flag_scanning,
                      false);
    wifi->state = -1;
    wifi->callback(wifi, hw_wifi_event_error, &wifi->network, wifi->user_data);
    sys_memset(&wifi->network, 0, sizeof(wifi->network));
    break;
  }
}
#endif
