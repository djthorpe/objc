/**
 * @file examples/runtime/ntp/main.c
 *
 * This example demonstrates how to connect to and disconnect from a Wi‑Fi
 * network.
 */
#include <runtime-hw/hw.h>
#include <runtime-net/net.h>
#include <runtime-sys/sys.h>

#ifndef WIFI_SSID
#error "WIFI_SSID not defined"
#endif

#ifndef WIFI_PASSWORD
#error "WIFI_PASSWORD not defined"
#endif

bool printdate(sys_date_t *date);

static void ntp_callback(net_ntp_t *ntp, const sys_date_t *date,
                         void *user_data) {
  (void)ntp;
  (void)user_data;

  // Set system time
  if (sys_date_set_now(date) == false) {
    sys_printf("Failed to set system time\n");
  } else {
    sys_printf("System time set successfully\n");
  }
}

static void wifi_callback(hw_wifi_t *wifi, hw_wifi_event_t event,
                          const hw_wifi_network_t *network, void *user_data) {
  (void)wifi;
  (void)user_data;

  if (event == hw_wifi_event_connected) {
    sys_printf("Connected to a network\n");
  }
  if (event == hw_wifi_event_disconnected) {
    sys_printf("Disconnected from a network\n");
  }
  if (event == hw_wifi_event_badauth || event == hw_wifi_event_notfound ||
      event == hw_wifi_event_error) {
    sys_printf("Failed to connect to network\n");
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
  if (hw_wifi_valid(wifi) == false) {
    sys_printf("WiFi not available\n");
  }

  // Initialize NTP
  net_ntp_t *ntp = net_ntp_init(ntp_callback, NULL);
  if (net_ntp_valid(ntp)) {
    sys_printf("NTP initialized\n");
  }

  // Start connection
  hw_wifi_network_t network = {
      .ssid = WIFI_SSID,
  };

  sys_printf("Connecting WiFi\n");
  if (!hw_wifi_connect(wifi, &network, WIFI_PASSWORD)) {
    sys_printf("Failed to start WiFi connection\n");
  }

  // Poll for a short while to let connection happen
  for (int i = 0; i < 5000; ++i) {
    hw_poll();
    net_poll();
    sys_sleep(10);

    // Print out the current time occasionally
    if (i % 1000 == 0) {
      sys_date_t date;
      if (sys_date_get_now(&date)) {
        printdate(&date);
      } else {
        sys_printf("Failed to get current time\n");
      }
    }
  }

  sys_printf("Disconnecting WiFi\n");
  if (!hw_wifi_disconnect(wifi)) {
    sys_printf("Failed to disconnect WiFi\n");
  }

  // Poll for a short while to let disconnection happen
  for (int i = 0; i < 1000; ++i) {
    hw_poll();
    net_poll();
    sys_sleep(10);

    // Print out the current time occasionally
    if (i % 1000 == 0) {
      sys_date_t date;
      if (sys_date_get_now(&date)) {
        printdate(&date);
      } else {
        sys_printf("Failed to get current time\n");
      }
    }
  }

  net_ntp_finalize(ntp);
  hw_wifi_finalize(wifi);
  hw_exit();
  sys_exit();
  return 0;
}
