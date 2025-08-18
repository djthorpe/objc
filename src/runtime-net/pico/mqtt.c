#include <runtime-hw/hw.h>
#include <runtime-net/net.h>
#include <runtime-sys/sys.h>
#include <stdint.h>
#include <string.h>

#ifdef PICO_CYW43_SUPPORTED
#include "cyw43.h"
#include <lwip/apps/mqtt.h>
#include <lwip/dns.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Server parameters
 */
typedef struct net_mqtt_server_t {
  uint16_t port;
  struct mqtt_connect_client_info_t client_info;
} net_mqtt_server_t;

/**
 * @brief Internal implementation overlay for the public opaque handle.
 */
typedef struct net_mqtt_impl_t {
  mqtt_client_t *client;
  net_mqtt_connect_callback_t connect_cb;
  void *user_data;
  net_mqtt_server_t server;
} net_mqtt_impl_t;

/**
 * @brief Internal waitgroup for blocking operations
 */
typedef struct net_mqtt_waitgroup_t {
  sys_waitgroup_t wg; /**< Waitgroup for blocking operations */
  err_t err;          /**< Error code for the operation */
} net_mqtt_waitgroup_t;

/**
 * @brief Cast helper
 */
static inline net_mqtt_impl_t *_impl(net_mqtt_t *h) {
  return (net_mqtt_impl_t *)(void *)h;
}

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

#ifdef PICO_CYW43_SUPPORTED
/**
 * @brief DNS resolution callback for MQTT.
 */
static void _net_mqtt_dns_cb(const char *name, const ip_addr_t *ipaddr,
                             void *arg);

/**
 * @brief Connect to the MQTT broker.
 */
static void _net_mqtt_do_connect(net_mqtt_impl_t *impl,
                                 const ip_addr_t *ipaddr);

/**
 * @brief Connection callback for MQTT.
 */
static void _net_mqtt_connection_cb(mqtt_client_t *client, void *arg,
                                    mqtt_connection_status_t status);

/**
 * @brief Publish callback for MQTT (lwIP mqtt_request_cb_t signature).
 */
static void _net_mqtt_publish_cb(void *arg, err_t err);

/**
 * @brief Subscribe callback for MQTT (lwIP mqtt_request_cb_t signature).
 */
static void _net_mqtt_subscribe_cb(void *arg, err_t err);

/**
 * @brief create a server struct
 */
static net_mqtt_server_t
_net_mqtt_server_new(const char *hostname, uint16_t port, const char *client_id,
                     const char *user, const char *password,
                     uint16_t keepalive_secs, const char *will_topic,
                     const char *will_message);
/**
 * @brief Check whether a server struct is valid.
 */
static bool _net_mqtt_server_valid(net_mqtt_server_t *server);

/**
 * @brief Free resources associated with a server struct.
 */
static void _net_mqtt_server_finalize(net_mqtt_server_t *server);

/**
 * @brief Duplicate a C string using sys_malloc.
 */
static char *_net_mqtt_strdup(const char *s);

#endif

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Create and initialize an MQTT handle.
 */
net_mqtt_t net_mqtt_init(net_mqtt_connect_callback_t connect_cb,
                         void *user_data) {

  sys_assert(connect_cb);

  net_mqtt_t mqtt = {0};
  net_mqtt_impl_t *impl = _impl(&mqtt);
  mqtt_client_t *client = mqtt_client_new();
  if (client == NULL) {
    return mqtt;
  }

  // Initialize MQTT state.
  impl->client = client;
  impl->connect_cb = connect_cb;
  impl->user_data = user_data;

  return mqtt;
}

/**
 * @brief Check whether an MQTT handle is valid and connected.
 */
bool net_mqtt_valid(net_mqtt_t *mqtt) {
  net_mqtt_impl_t *impl = _impl(mqtt);
  if (mqtt == NULL || impl->client == NULL) {
    return false;
  }
  return mqtt_client_is_connected(impl->client);
}

/**
 * @brief Finalize the MQTT handle and free associated resources.
 */
void net_mqtt_finalize(net_mqtt_t *mqtt) {
  net_mqtt_impl_t *impl = _impl(mqtt);
  if (mqtt == NULL || impl->client == NULL) {
    return;
  }

  // Disconnect the MQTT client.
  mqtt_disconnect(impl->client);
  mqtt_client_free(impl->client);
  _net_mqtt_server_finalize(&impl->server);

  // Clear MQTT state.
  sys_memset(mqtt, 0, sizeof(net_mqtt_t));
}

/**
 * @brief Begin an asynchronous connection to an MQTT broker.
 */
bool net_mqtt_connect(net_mqtt_t *mqtt, const char *hostname, uint16_t port,
                      const char *client_id, const char *user,
                      const char *password, uint16_t keepalive_secs,
                      const char *will_topic, const char *will_message) {
  net_mqtt_impl_t *impl = _impl(mqtt);
  sys_assert(mqtt && impl->client);
  sys_assert(hostname);

#ifndef PICO_CYW43_SUPPORTED
  (void)client_id;
  (void)user;
  (void)password;
  (void)keepalive_secs;
  (void)will_topic;
  (void)port;
  (void)hostname;
  return false;
#else
  // Disallow concurrent connect while already connected
  if (mqtt_client_is_connected(impl->client)) {
    return false;
  }

  // Create a new server structure
  sys_assert(_net_mqtt_server_valid(&impl->server) == false);
  impl->server = _net_mqtt_server_new(hostname, port, client_id, user, password,
                                      keepalive_secs, will_topic, will_message);
  if (_net_mqtt_server_valid(&impl->server) == false) {
    return false;
  }

  // Kick off DNS resolution; may complete synchronously
  ip_addr_t addr;
  err_t err = dns_gethostbyname(hostname, &addr, _net_mqtt_dns_cb, impl);
  if (err == ERR_OK) {
    // Address is already resolved; proceed to connect synchronously
    _net_mqtt_do_connect(impl, &addr);
    return true;
  } else if (err == ERR_INPROGRESS) {
    // Will complete in DNS callback
    return true;
  } else {
    // Resolution failed immediately
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_dns_error_t,
                       impl->user_data);
    }
    _net_mqtt_server_finalize(&impl->server);
    return false;
  }
#endif
}

/**
 * @brief Disconnect from the MQTT broker.
 */
bool net_mqtt_disconnect(net_mqtt_t *mqtt) {
  net_mqtt_impl_t *impl = _impl(mqtt);
  sys_assert(mqtt && impl->client);
#ifndef PICO_CYW43_SUPPORTED
  return false;
#else
  // Disallow if not connected
  if (!mqtt_client_is_connected(impl->client)) {
    return false;
  }

  // Perform disconnect
  mqtt_disconnect(impl->client);

  // Callback
  if (impl->connect_cb) {
    impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_disconnected_t,
                     impl->user_data);
  }

  // Return success
  return true;
#endif
}

/**
 * @brief Publish a message to a topic.
 */
bool net_mqtt_publish(net_mqtt_t *mqtt, const char *topic, const void *data,
                      size_t size, uint8_t qos) {
  net_mqtt_impl_t *impl = _impl(mqtt);
  sys_assert(mqtt && impl->client);
  sys_assert(topic && topic[0] != '\0');
  sys_assert(size == 0 || data);
  sys_assert(qos <= 2);

#ifndef PICO_CYW43_SUPPORTED
  return false;
#else
  // Disallow if not connected
  if (!mqtt_client_is_connected(impl->client)) {
    return false;
  }

  // Enforce lwIP payload length limit (u16_t)
  if (size > UINT16_MAX) {
    return false;
  }

  // Perform the publish and wait for the result
  err_t err = 1;
  if (mqtt_publish(impl->client, topic, data, (u16_t)size, qos, qos != 0,
                   _net_mqtt_publish_cb, &err) != ERR_OK) {
    return false;
  }

  // Wait for the publish to complete
  sys_printf("publish WAIT\n");
  do {
    hw_poll();
    sys_sleep(10);
  } while (err == 1);

#ifdef DEBUG
  switch (err) {
  case ERR_OK:
    // Publish succeeded
    sys_printf("net_mqtt_publish: ok\n");
    break;
  case ERR_TIMEOUT:
    // Publish timed out
    sys_printf("net_mqtt_publish: timeout\n");
    break;
  case ERR_ABRT:
    // Publish aborted
    sys_printf("net_mqtt_publish: aborted\n");
    break;
  }
#endif
  // Return success or failure
  return err == ERR_OK;
#endif
}

/**
 * @brief Subscribe to a topic.
 */
bool net_mqtt_subscribe(net_mqtt_t *mqtt, const char *topic, uint8_t qos) {
  net_mqtt_impl_t *impl = _impl(mqtt);
  sys_assert(mqtt && impl->client);
  sys_assert(topic && topic[0] != '\0');
  sys_assert(qos == 0 || qos == 1 || qos == 2);

#ifndef PICO_CYW43_SUPPORTED
  return false;
#else
  // Disallow if not connected
  if (!mqtt_client_is_connected(impl->client)) {
    return false;
  }

  // Perform the subscribe
  err_t err =
      mqtt_subscribe(impl->client, topic, qos, _net_mqtt_subscribe_cb, impl);
  if (err == ERR_OK) {
    return true;
  } else {
    return false;
  }
#endif
}

#ifdef PICO_CYW43_SUPPORTED

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief DNS resolution result callback.
 */
static void _net_mqtt_dns_cb(const char *name, const ip_addr_t *ipaddr,
                             void *arg) {
  (void)name;
  net_mqtt_impl_t *impl = (net_mqtt_impl_t *)arg;
  sys_assert(impl);
  if (ipaddr == NULL) {
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_dns_error_t,
                       impl->user_data);
    }
    // Free server information
    _net_mqtt_server_finalize(&impl->server);
  } else {
    _net_mqtt_do_connect(impl, ipaddr);
  }
}

/**
 * @brief Perform connection
 */
static void _net_mqtt_do_connect(net_mqtt_impl_t *impl, const ip_addr_t *addr) {
  sys_assert(impl && addr && _net_mqtt_server_valid(&impl->server));

  err_t err = mqtt_client_connect(impl->client, addr, impl->server.port,
                                  _net_mqtt_connection_cb, impl,
                                  &impl->server.client_info);
  if (err != ERR_OK) {
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_error_t,
                       impl->user_data);
    }

    // Free server information
    _net_mqtt_server_finalize(&impl->server);
  }
}

/**
 * @brief Indicate the publish was completed, and return the error result.
 */
static void _net_mqtt_publish_cb(void *arg, err_t err) {
  err_t *cb_err = (err_t *)arg;
  sys_assert(cb_err);
  sys_printf("publish CALLBACK\n");
  *cb_err = err;
}

static void _net_mqtt_subscribe_cb(void *arg, err_t err) {
  net_mqtt_impl_t *impl = (net_mqtt_impl_t *)arg;
  sys_assert(impl);
  switch (err) {
  case ERR_OK:
    sys_printf("Topic subscribed successfully\n");
    break;
  case ERR_TIMEOUT:
    sys_printf("Topic subscribe timed out\n");
    break;
  case ERR_ABRT:
    sys_printf("Topic subscribe aborted\n");
    break;
  }
}

static void _net_mqtt_connection_cb(mqtt_client_t *client, void *arg,
                                    mqtt_connection_status_t status) {
  (void)client;
  net_mqtt_impl_t *impl = (net_mqtt_impl_t *)arg;
  sys_assert(impl);

  switch (status) {
  case MQTT_CONNECT_ACCEPTED:
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_connected_t,
                       impl->user_data);
    }
    break;
  case MQTT_CONNECT_REFUSED_PROTOCOL_VERSION:
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_protocol_error_t,
                       impl->user_data);
    }
    // Free server information
    _net_mqtt_server_finalize(&impl->server);
    break;
  case MQTT_CONNECT_REFUSED_IDENTIFIER:
  case MQTT_CONNECT_REFUSED_SERVER:
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_protocol_error_t,
                       impl->user_data);
    }
    // Free server information
    _net_mqtt_server_finalize(&impl->server);
    break;
  case MQTT_CONNECT_REFUSED_USERNAME_PASS:
  case MQTT_CONNECT_REFUSED_NOT_AUTHORIZED_:
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_auth_error_t,
                       impl->user_data);
    }
    // Free server information
    _net_mqtt_server_finalize(&impl->server);
    break;
  case MQTT_CONNECT_DISCONNECTED:
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_disconnected_t,
                       impl->user_data);
    }
    // Free server information
    _net_mqtt_server_finalize(&impl->server);
    break;
  case MQTT_CONNECT_TIMEOUT:
  default:
    if (impl->connect_cb) {
      impl->connect_cb((net_mqtt_t *)impl, net_mqtt_status_timeout_t,
                       impl->user_data);
    }
    // Free server information
    _net_mqtt_server_finalize(&impl->server);
    break;
  }
}

/**
 * @brief create a server struct
 */
static net_mqtt_server_t
_net_mqtt_server_new(const char *hostname, uint16_t port, const char *client_id,
                     const char *user, const char *password,
                     uint16_t keepalive_secs, const char *will_topic,
                     const char *will_message) {
  (void)hostname; // DNS is handled separately; hostname not stored here
  net_mqtt_server_t server = {0};

  // Set port, keep alive
  server.port = port ? port : MQTT_PORT;
  server.client_info.keep_alive = keepalive_secs;

  // Set client info, username and password
  bool success = true;
  if (client_id) {
    server.client_info.client_id = _net_mqtt_strdup(client_id);
    if (server.client_info.client_id == NULL) {
      success = false;
    }
  } else {
    // Use serial number otherwise
    server.client_info.client_id = _net_mqtt_strdup(sys_env_serial());
    if (server.client_info.client_id == NULL) {
      success = false;
    }
  }
  if (user) {
    server.client_info.client_user = _net_mqtt_strdup(user);
    if (server.client_info.client_user == NULL) {
      success = false;
    }
  }
  if (password) {
    server.client_info.client_pass = _net_mqtt_strdup(password);
    if (server.client_info.client_pass == NULL) {
      success = false;
    }
  }
  if (will_topic) {
    server.client_info.will_topic = _net_mqtt_strdup(will_topic);
    if (server.client_info.will_topic == NULL) {
      success = false;
    }
  }
  if (will_message) {
    server.client_info.will_msg = _net_mqtt_strdup(will_message);
    if (server.client_info.will_msg == NULL) {
      success = false;
    }
  }

  // If we were not successful
  if (success == false) {
    _net_mqtt_server_finalize(&server);
    return server;
  }

  // Return success
  return server;
}

/**
 * @brief Check whether a server struct is valid.
 */
static bool _net_mqtt_server_valid(net_mqtt_server_t *server) {
  if (server == NULL || server->port == 0) {
    return false;
  }
  return true;
}

/**
 * @brief Free resources associated with a server struct.
 */
static void _net_mqtt_server_finalize(net_mqtt_server_t *server) {
  if (server == NULL || server->port == 0) {
    return;
  }

  // Free
  sys_free((void *)server->client_info.client_id);
  sys_free((void *)server->client_info.client_user);
  sys_free((void *)server->client_info.client_pass);
  sys_free((void *)server->client_info.will_topic);
  sys_free((void *)server->client_info.will_msg);
  sys_memset((void *)server, 0, sizeof(net_mqtt_server_t));
}

static char *_net_mqtt_strdup(const char *s) {
  if (s == NULL) {
    return NULL;
  }
  size_t n = strlen(s);
  char *p = (char *)sys_malloc(n + 1);
  if (p == NULL) {
    return NULL;
  }
  sys_memcpy(p, s, n + 1);
  return p;
}

#endif
