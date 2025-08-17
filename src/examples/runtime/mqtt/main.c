/**
 * @file examples/runtime/mqtt/main.c
 *
 * This example connects to Wi‑Fi and then initiates an MQTT connection to
 * MQTT_SERVER. It prints status changes from the MQTT client, and then
 * after some time, will disconnect.
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

#ifndef MQTT_SERVER
#error "MQTT_SERVER not defined"
#endif

static void mqtt_connect_cb(net_mqtt_t *mqtt, net_mqtt_status_t status,
                            void *user_data) {
  (void)mqtt;
  (void)user_data;

  switch (status) {
  case net_mqtt_status_connected_t:
    sys_printf("MQTT: connected to %s\n", MQTT_SERVER);
    break;
  case net_mqtt_status_disconnected_t:
    sys_printf("MQTT: disconnected\n");
    break;
  case net_mqtt_status_dns_error_t:
    sys_printf("MQTT: DNS error resolving %s\n", MQTT_SERVER);
    break;
  case net_mqtt_status_protocol_error_t:
    sys_printf("MQTT: protocol error\n");
    break;
  case net_mqtt_status_auth_error_t:
    sys_printf("MQTT: auth error\n");
    break;
  case net_mqtt_status_timeout_t:
    sys_printf("MQTT: timeout\n");
    break;
  default:
    sys_printf("MQTT: error\n");
    break;
  }
}

static void wifi_callback(hw_wifi_t *wifi, hw_wifi_event_t event,
                          const hw_wifi_network_t *network, void *user_data) {
  (void)wifi;

  net_mqtt_t *mqtt = (net_mqtt_t *)user_data;
  if (event == hw_wifi_event_connected) {
    sys_printf("WiFi: connected, starting MQTT to %s\n", MQTT_SERVER);
    // Start MQTT connect: default port (0 -> 1883), default client_id, no auth,
    // keepalive 60s
    if (!net_mqtt_connect(mqtt, MQTT_SERVER, 0, NULL, NULL, NULL, 60, NULL,
                          NULL)) {
      sys_printf("MQTT: failed to initiate connection\n");
    }
  }
  if (event == hw_wifi_event_disconnected) {
    sys_printf("WiFi: disconnected\n");
    // Ensure MQTT is disconnected too
    net_mqtt_disconnect(mqtt);
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

int main(void) {
  sys_init();
  hw_init();

  // Init
  net_mqtt_t mqtt = net_mqtt_init(mqtt_connect_cb, NULL);
  hw_wifi_t *wifi = hw_wifi_init(NULL, wifi_callback, &mqtt);
  if (!hw_wifi_valid(wifi)) {
    sys_printf("WiFi not available on this board\n");
  }

  // Start Wi‑Fi connection
  hw_wifi_network_t network = {.ssid = WIFI_SSID};
  sys_printf("WiFi: connecting to %s\n", WIFI_SSID);
  if (!hw_wifi_connect(wifi, &network, WIFI_PASSWORD)) {
    sys_printf("WiFi: failed to start connection\n");
  }

  // Run main loop
  int i;
  for (i = 0; i < 1000; i++) {
    hw_poll();
    net_poll();
    sys_sleep(10);
  }

  // Cleanup if you add a termination path:
  sys_printf("MQTT disconnecting...\n");
  net_mqtt_disconnect(&mqtt);

  for (i = 0; i < 1000; i++) {
    hw_poll();
    net_poll();
    sys_sleep(10);
  }

  sys_printf("WiFi disconnecting...\n");
  hw_wifi_disconnect(wifi);

  for (i = 0; i < 1000; i++) {
    hw_poll();
    net_poll();
    sys_sleep(10);
  }

  hw_wifi_finalize(wifi);
  hw_exit();
  sys_exit();
  return 0;
}
