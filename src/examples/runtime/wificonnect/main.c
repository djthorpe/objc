/**
 * @file examples/runtime/wificonnect/main.c
 *
 * This example demonstrates how to connect to and disconnect from a Wi‑Fi
 * network.
 */
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

static void wifi_callback(hw_wifi_t *wifi, hw_wifi_event_t event,
                          const hw_wifi_network_t *network, void *user_data) {
  (void)user_data;
  (void)wifi;

  if (event == hw_wifi_event_scan && network) {
    sys_printf("Scan result\n");
  }
  if (event == hw_wifi_event_scan && network == NULL) {
    sys_printf("End of scan\n");
  }
  if (event == hw_wifi_event_joining) {
    sys_printf("Joining a network\n");
  }
  if (event == hw_wifi_event_connected) {
    sys_printf("Connected to a network\n");
  }
  if (event == hw_wifi_event_disconnected) {
    sys_printf("Disconnected from a network\n");
  }
  if (event == hw_wifi_event_badauth) {
    sys_printf("Failed to connect: Bad Auth\n");
  }
  if (event == hw_wifi_event_notfound) {
    sys_printf("Failed to connect: Network Not Found\n");
  }
  if (event == hw_wifi_event_error) {
    sys_printf("Failed to connect: Error\n");
  }

  if (network) {
    sys_printf("AP: %02X:%02X:%02X:%02X:%02X:%02X SSID='%s' CH=%u RSSI=%d "
               "AUTH=0x%02X\n",
               network->bssid[0], network->bssid[1], network->bssid[2],
               network->bssid[3], network->bssid[4], network->bssid[5],
               network->ssid, (unsigned)network->channel, (int)network->rssi,
               (unsigned)network->auth);
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

  // Start connection
  hw_wifi_network_t network = {
      .ssid = "SSID",
  };

  sys_printf("Connecting WiFi\n");
  if (!hw_wifi_connect(wifi, &network, "PASSWORD")) {
    sys_printf("Failed to start WiFi connection\n");
  }

  // Poll for a short while to let connection happen
  for (int i = 0; i < 5000; ++i) {
    hw_poll();
    sys_sleep(10);
  }

  sys_printf("Disconnecting WiFi\n");
  if (!hw_wifi_disconnect(wifi)) {
    sys_printf("Failed to disconnect WiFi\n");
  }

  // Poll for a short while to let connection happen
  for (int i = 0; i < 1000; ++i) {
    hw_poll();
    sys_sleep(10);
  }

done:
  hw_wifi_finalize(wifi);
  hw_exit();
  sys_exit();
  return 0;
}
