#include <runtime-hw/wifi.h>
#include <runtime-sys/sys.h>
#include <stdlib.h>

// Minimal opaque handle definition for the stub (header forward-declares struct
// hw_wifi_t)
struct hw_wifi_t {};

hw_wifi_t *hw_wifi_init(const char *country_code) {
  // Wi‑Fi not implemented in stub; explicitly unavailable
  (void)country_code;
  return NULL;
}

bool hw_wifi_valid(hw_wifi_t *wifi) {
  // Valid only if non-NULL; stub has no additional state
  return wifi != NULL;
}

void hw_wifi_finalize(hw_wifi_t *wifi) {
  // Stub: nothing to free, but accept NULL safely
  (void)wifi;
}

bool hw_wifi_scan(hw_wifi_t *wifi, hw_wifi_callback_t callback,
                  void *user_data) {
  // Not implemented: return false to indicate failure to start scan.
  (void)wifi;
  (void)callback;
  (void)user_data;
  return false;
}
