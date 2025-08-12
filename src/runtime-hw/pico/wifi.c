#include <runtime-hw/wifi.h>
#include <runtime-sys/sys.h>

// Minimal Pico Wi‑Fi shim; real implementation should use CYW43 APIs when
// available.
struct hw_wifi {};

hw_wifi_t *hw_wifi_init() {
  // TODO: Initialize CYW43 and return a handle; for now return NULL until
  // implemented
  return NULL;
}

bool hw_wifi_valid(hw_wifi_t *wifi) { return wifi != NULL; }

void hw_wifi_finalize(hw_wifi_t *wifi) { (void)wifi; }
