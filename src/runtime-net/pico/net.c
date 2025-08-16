#include "ntp.h"
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Occasional polling function for the network system.
 */
void net_poll(void) {
  // Call _net_ntp_poll which will stop/start the NTP service
  // depending on the connection state
  _net_ntp_poll();
}
