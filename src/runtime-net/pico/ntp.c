#include <runtime-net/net.h>
#include <runtime-sys/sys.h>

#ifdef PICO_CYW43_SUPPORTED
#include "cyw43.h"
#include <lwip/apps/sntp.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// TYPES

#define NET_NTP_UPDATE_INTERVAL_MS 30000 // Check state every 30 seconds

/**
 * @brief Handle for power management state.
 */
typedef struct net_ntp_t {
  net_ntp_callback_t callback;
  void *user_data;
  int state;
  uint64_t timestamp;
} net_ntp_t;

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

void _net_ntp_set_enabled(void);
void _net_ntp_set_disabled(void);

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// The NTP singleton
static net_ntp_t _net_ntp = {0};

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize NTP management.
 */
net_ntp_t *net_ntp_init(net_ntp_callback_t callback, void *user_data) {
  // Reset the NTP handle
  net_ntp_finalize(&_net_ntp);

  // Set callback
  _net_ntp.callback = callback;
  _net_ntp.user_data = user_data;
  _net_ntp.state = -1;
  _net_ntp.timestamp = 0;

  // Return initialized NTP handle
  return &_net_ntp;
}

/**
 * @brief Determine if the NTP handle is initialized and usable.
 */
bool net_ntp_valid(net_ntp_t *ntp) {
  if (ntp == NULL) {
    return false;
  }
  if (ntp->callback == NULL) {
    return false;
  }
  if (ntp->state == -1 || ntp->timestamp == 0) {
    return false;
  }
#ifdef PICO_CYW43_SUPPORTED
  return sntp_enabled() ? true : false;
#else
  return false;
#endif
}

/**
 * @brief Finalize and release any resources.
 */
void net_ntp_finalize(net_ntp_t *ntp) {
  if (ntp == NULL) {
    return;
  }
  if (ntp->callback == NULL) {
    return;
  }

  // Disable
  _net_ntp_set_disabled();

  // Reset NTP state
  sys_memset(ntp, 0, sizeof(net_ntp_t));
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

void _net_ntp_set_enabled(void) {
#ifdef PICO_CYW43_SUPPORTED
  // Start SNTP
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_init();
#endif
}

void _net_ntp_set_disabled(void) {
#ifdef PICO_CYW43_SUPPORTED
  // Stop SNTP
  sntp_stop();
#endif
}

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS

void net_ntp_set_date(uint32_t sec) {
  sys_date_t date = {
      .seconds = (int64_t)sec,
      .nanoseconds = 0,
      .tzoffset = 0,
  };
  net_ntp_t *ntp = &_net_ntp;
  if (net_ntp_valid(ntp)) {
    ntp->callback(ntp, &date, ntp->user_data);
  }
}

void _net_ntp_poll(void) {
#ifdef PICO_CYW43_SUPPORTED
  // Run this check every 30 seconds
  net_ntp_t *ntp = &_net_ntp;

  // Not initialized
  if (ntp->callback == NULL) {
    return;
  }

  // Check if we need to update the timestamp
  uint64_t timestamp = sys_date_get_timestamp();
  if (ntp->timestamp != 0 &&
      timestamp < ntp->timestamp + NET_NTP_UPDATE_INTERVAL_MS) {
    return;
  } else {
    ntp->timestamp = timestamp;
  }

  // Get current link state, and act if it's changed
  int state = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
  if (state == ntp->state) {
    return;
  } else {
    ntp->state = state;
    switch (state) {
    case CYW43_LINK_UP:
#ifdef DEBUG
      sys_printf("_net_ntp_set_enabled()\n");
#endif
      _net_ntp_set_enabled();
      break;
    default:
#ifdef DEBUG
      sys_printf("_net_ntp_set_disabled()\n");
#endif
      _net_ntp_set_disabled();
      break;
    }
  }
#endif
}
