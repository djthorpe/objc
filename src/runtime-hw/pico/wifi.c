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
  hw_wifi_flag_up = (1 << 0),         ///< Up
  hw_wifi_flag_scanning = (1 << 1),   ///< Scanning
  hw_wifi_flag_joining = (1 << 2),    ///< Joining
  hw_wifi_flag_connected = (1 << 3),  ///< Connected with IP address
  hw_wifi_flag_error = (1 << 4),      ///< Error
  hw_wifi_flag_error_auth = (1 << 5), ///< Authentication Error
  hw_wifi_flag_error_ssid = (1 << 6), ///< SSID Error
  hw_wifi_flag_sta = (1 << 7), ///< Station Mode (otherwise Access Point mode)
} hw_wifi_flags_t;

/**
 * @brief Handle for Wi-Fi state.
 */
struct hw_wifi_t {
  const char *country_code;
  hw_wifi_flags_t state;
  hw_wifi_callback_t callback;
  void *user_data;
};

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

#ifdef PICO_CYW43_SUPPORTED
static hw_wifi_flags_t _hw_wifi_update_state(hw_wifi_t *wifi);
static uint32_t _hw_wifi_country_code(const char *country_code);
static int _hw_wifi_scan_callback(void *env,
                                  const cyw43_ev_scan_result_t *result);
#endif

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

#define HW_WIFI_COUNTRY_CODE_DEFAULT "XX"

// The wifi singleton
static hw_wifi_t _hw_wifi = {0};

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize Wi-Fi management.
 */
hw_wifi_t *hw_wifi_init(const char *country_code) {
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
  // We always start in station mode, which connects to other
  // Wi-Fi networks as a client, but can change this later
  if (cyw43_is_initialized(&cyw43_state)) {
    _hw_wifi.country_code = country_code;
    _hw_wifi.state = hw_wifi_flag_sta;
  }
#endif

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
  hw_wifi_flags_t state = _hw_wifi_update_state(wifi);
  if (state & hw_wifi_flag_up) {
    if (state & hw_wifi_flag_sta) {
      if (cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA)) {
        cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, false,
                          _hw_wifi_country_code(wifi->country_code));
      }
    } else {
      cyw43_wifi_leave(&cyw43_state, CYW43_ITF_AP);
    }
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
bool hw_wifi_scan(hw_wifi_t *wifi, hw_wifi_callback_t callback,
                  void *user_data) {
  sys_assert(hw_wifi_valid(wifi));
  sys_assert(callback);
  bool success = false;
#ifdef PICO_CYW43_SUPPORTED
  hw_wifi_flags_t state = _hw_wifi_update_state(wifi);
  if (state & hw_wifi_flag_scanning || state & hw_wifi_flag_joining) {
    // Already scanning, or joining a network
    return false;
  }

  // If link is down, then set-up
  if ((state & hw_wifi_flag_up) == 0) {
    if (wifi->state & hw_wifi_flag_sta) {
      // Bring Wi‑Fi up in STA mode
      cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, true,
                        _hw_wifi_country_code(wifi->country_code));
    } else {
      // Assume AP mode when not STA
      cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_AP, true,
                        _hw_wifi_country_code(wifi->country_code));
    }
  }

  // Zero-initialize scan options: 0 means scan all SSIDs, active scan by
  // default
  cyw43_wifi_scan_options_t opts;
  sys_memset(&opts, 0, sizeof(opts));

  // Pass the wifi handle as the callback environment
  wifi->callback = callback;
  wifi->user_data = user_data;
  if (cyw43_wifi_scan(&cyw43_state, &opts, wifi, _hw_wifi_scan_callback) == 0) {
    wifi->state |= hw_wifi_flag_scanning;
    success = true;
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

static hw_wifi_flags_t _hw_wifi_update_state(hw_wifi_t *wifi) {
  sys_assert(hw_wifi_valid(wifi));
#ifdef PICO_CYW43_SUPPORTED
  // Get status
  int status = CYW43_LINK_DOWN;
  if (wifi->state & hw_wifi_flag_sta) {
    // Reflect status if in station mode
    status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    if (status == CYW43_LINK_UP) {
      wifi->state |= hw_wifi_flag_up;
    } else if (status == CYW43_LINK_DOWN) {
      wifi->state &= ~hw_wifi_flag_up;
      wifi->state &= ~hw_wifi_flag_joining;
      wifi->state &= ~hw_wifi_flag_connected;
    } else if (status == CYW43_LINK_JOIN || status == CYW43_LINK_NOIP) {
      wifi->state |= hw_wifi_flag_up;
      wifi->state |= hw_wifi_flag_joining;
      wifi->state &= ~hw_wifi_flag_connected;
    } else if (status == CYW43_LINK_FAIL) {
      wifi->state |= hw_wifi_flag_error;
      wifi->state &= ~hw_wifi_flag_joining;
      wifi->state &= ~hw_wifi_flag_connected;
    } else if (status == CYW43_LINK_NONET) {
      wifi->state |= hw_wifi_flag_error;
      wifi->state |= hw_wifi_flag_error_ssid;
      wifi->state &= ~hw_wifi_flag_joining;
      wifi->state &= ~hw_wifi_flag_connected;
    } else if (status == CYW43_LINK_BADAUTH) {
      wifi->state |= hw_wifi_flag_error;
      wifi->state |= hw_wifi_flag_error_auth;
      wifi->state &= ~hw_wifi_flag_joining;
      wifi->state &= ~hw_wifi_flag_connected;
    }
  }
#endif
  return wifi->state;
}

#ifdef PICO_CYW43_SUPPORTED
static int _hw_wifi_scan_callback(void *env,
                                  const cyw43_ev_scan_result_t *result) {
  static hw_wifi_network_t network = {0};
  static char ssid_buf[33];
  hw_wifi_t *wifi = (hw_wifi_t *)env;
  sys_assert(hw_wifi_valid(wifi));
  sys_assert(result);

  if (wifi->callback) {
    // Copy SSID safely (result->ssid may not be NUL-terminated)
    size_t n = result->ssid_len;
    if (n > sizeof(ssid_buf) - 1)
      n = sizeof(ssid_buf) - 1;
    if (n > 0)
      sys_memcpy(ssid_buf, result->ssid, n);
    ssid_buf[n] = '\0';
    network.ssid = ssid_buf;
    sys_memcpy(network.bssid, result->bssid, sizeof(network.bssid));
    network.channel = result->channel;
    network.rssi = (int16_t)result->rssi;
    // Map auth (best-effort based on available macros)
    network.auth = hw_wifi_auth_open;
    uint8_t am = result->auth_mode;
#ifdef CYW43_AUTH_OPEN
    if (am == (uint8_t)CYW43_AUTH_OPEN)
      network.auth = hw_wifi_auth_open;
#endif
#ifdef CYW43_AUTH_WPA_TKIP_PSK
    if (am == (uint8_t)CYW43_AUTH_WPA_TKIP_PSK)
      network.auth = hw_wifi_auth_wpa_tkip;
#endif
#ifdef CYW43_AUTH_WPA2_AES_PSK
    if (am == (uint8_t)CYW43_AUTH_WPA2_AES_PSK)
      network.auth = hw_wifi_auth_wpa2_aes;
#endif
#ifdef CYW43_AUTH_WPA2_MIXED_PSK
    if (am == (uint8_t)CYW43_AUTH_WPA2_MIXED_PSK)
      network.auth = hw_wifi_auth_wpa2_aes;
#endif
#ifdef CYW43_AUTH_WPA3_SAE_AES_PSK
    if (am == (uint8_t)CYW43_AUTH_WPA3_SAE_AES_PSK)
      network.auth = hw_wifi_auth_wpa3_sae;
#endif

    // Deliver pointer-to-pointer to match API
    wifi->callback(wifi, &network, wifi->user_data);
  }
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
  // If not scanning, then don't continue
  if ((wifi->state & hw_wifi_flag_scanning) == 0) {
    return;
  }
  // Clear when scan is no longer active, and callback
  if (!cyw43_wifi_scan_active(&cyw43_state) &&
      (wifi->state & hw_wifi_flag_scanning)) {
    wifi->state &= ~hw_wifi_flag_scanning;
    if (wifi->callback) {
      // Completion: indicate with NULL network
      wifi->callback(wifi, NULL, wifi->user_data);
    }
  }
}
#endif
