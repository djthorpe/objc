#include "ntp.h"
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC HOOK IMPLEMENTATION

void net_poll(void) {
  // Call the NTP poll function to check for updates when WiFi connected
  _net_ntp_poll();
}
