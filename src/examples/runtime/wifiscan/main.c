/**
 * @file examples/runtime/wifiscan/main.c
 *
 * This example demonstrates how to start a Wi‑Fi scan and print results.
 */
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

static void wifi_callback(hw_wifi_t *wifi, hw_wifi_event_t event,
                          const hw_wifi_network_t *network, void *user_data) {
  (void)user_data;
  (void)wifi;

  if (event == hw_wifi_event_scan && network) {
    sys_printf("AP: %02X:%02X:%02X:%02X:%02X:%02X SSID='%s' CH=%u RSSI=%d "
               "AUTH=0x%02X\n",
               network->bssid[0], network->bssid[1], network->bssid[2],
               network->bssid[3], network->bssid[4], network->bssid[5],
               network->ssid, (unsigned)network->channel, (int)network->rssi,
               (unsigned)network->auth);
  } else if (event == hw_wifi_event_scan && network == NULL) {
    sys_printf("Scan: complete\n");
  }
}

int main() {
  sys_init();
  hw_init();

  // Initialize Wi‑Fi (use default country)
  hw_wifi_t *wifi = hw_wifi_init(NULL, wifi_callback, NULL);
  if (!hw_wifi_valid(wifi)) {
    sys_printf("WiFi not available\n");
    goto done;
  }

  // Start scan (completion callback may not be invoked on Pico; results print
  // from driver)
  if (!hw_wifi_scan(wifi)) {
    sys_printf("Failed to start WiFi scan\n");
  }

  // Poll for a short while to let scan results print
  for (int i = 0; i < 500; ++i) {
    hw_poll();
    sys_sleep(10);
  }

done:
  hw_wifi_finalize(wifi);
  hw_exit();
  sys_exit();
  return 0;
}
