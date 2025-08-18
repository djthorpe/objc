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
#include <string.h>

#ifndef WIFI_SSID
#error "WIFI_SSID not defined"
#endif

#ifndef WIFI_PASSWORD
#error "WIFI_PASSWORD not defined"
#endif

#ifndef MQTT_SERVER
#error "MQTT_SERVER not defined"
#endif

// Example publish target: root topic and fixed message
static const char MQTT_TOPIC[] = "picofuse";

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

static void publish_mqtt_message(net_mqtt_t *mqtt, const char *topic,
                                 const char *message) {
  if (net_mqtt_valid(mqtt) == false) {
    return;
  }
  char topic2[80];
  sys_sprintf(topic2, sizeof(topic2), "%s/%s/%s", MQTT_TOPIC, sys_env_serial(),
              topic);
  net_mqtt_publish_str(mqtt, topic2, message, 0);
}

static void wifi_callback(hw_wifi_t *wifi, hw_wifi_event_t event,
                          const hw_wifi_network_t *network, void *user_data) {
  (void)wifi;
  net_mqtt_t *mqtt = (net_mqtt_t *)user_data;
  char str[80];

  if (event == hw_wifi_event_connected && net_mqtt_valid(mqtt) == false) {
    if (!net_mqtt_connect(mqtt, MQTT_SERVER, 0, NULL, NULL, NULL, 60, NULL,
                          NULL)) {
      sys_printf("MQTT: failed to initiate connection\n");
    }
  }

  if (event == hw_wifi_event_connected && net_mqtt_valid(mqtt)) {
    sys_sprintf(str, sizeof(str), "%u", network->channel);
    publish_mqtt_message(mqtt, "channel", str);

    sys_sprintf(str, sizeof(str), "%d", network->rssi);
    publish_mqtt_message(mqtt, "rssi", str);

    sys_sprintf(str, sizeof(str), "%s", network->ssid);
    publish_mqtt_message(mqtt, "ssid", str);
  }

  if (event == hw_wifi_event_disconnected) {
    sys_printf("WiFi: disconnected\n");
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

static void power_callback(hw_power_t *pwr, hw_power_flag_t event,
                           uint32_t value, void *user_data) {
  (void)pwr;
  net_mqtt_t *mqtt = (net_mqtt_t *)user_data;
  char str[80];

  switch (event) {
  case HW_POWER_USB:
    sys_sprintf(str, sizeof(str), "%u", value);
    publish_mqtt_message(mqtt, "power", "USB");
    break;
  case HW_POWER_BATTERY:
    sys_sprintf(str, sizeof(str), "%u", value);
    publish_mqtt_message(mqtt, "power", "Battery");

    sys_sprintf(str, sizeof(str), "%u", value);
    publish_mqtt_message(mqtt, "battery", str);
    break;
  default:
    sys_sprintf(str, sizeof(str), "%u", value);
    publish_mqtt_message(mqtt, "power", "Unknown");
    break;
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

  // Create power callback
  hw_power_t *pwr = hw_power_init(0xFF, 0xFF, power_callback, &mqtt);

  // Start Wi‑Fi connection
  hw_wifi_network_t network = {.ssid = WIFI_SSID};
  sys_printf("WiFi: connecting to %s\n", WIFI_SSID);
  if (!hw_wifi_connect(wifi, &network, WIFI_PASSWORD)) {
    sys_printf("WiFi: failed to start connection\n");
  }

  // Run main loop
  int i = 0;
  do {
    hw_poll();
    net_poll();
    sys_sleep(10);
    if (i++ % 1000 == 0) {
      char uptime[32];
      sys_sprintf(uptime, sizeof(uptime), "%lu",
                  (unsigned long)(sys_date_get_timestamp() / 1000));
      publish_mqtt_message(&mqtt, "uptime", uptime);
    }
  } while (true);

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

  hw_power_finalize(pwr);
  hw_wifi_finalize(wifi);
  hw_exit();
  sys_exit();
  return 0;
}
