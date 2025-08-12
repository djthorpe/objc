#include <runtime-hw/wifi.h>
#include <runtime-sys/sys.h>
#include <stdlib.h>

// Minimal opaque handle definition for the stub
struct hw_wifi {};

hw_wifi_t *hw_wifi_init() {
  // Wi‑Fi not implemented in stub; return a non-NULL dummy to allow API use
  // or return NULL to indicate unavailable. Here we return NULL to be explicit.
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
