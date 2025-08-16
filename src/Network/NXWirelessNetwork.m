#include "NXWireless+Private.h"
#include <Network/Network.h>

@implementation NXWirelessNetwork

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)initWithNetwork:(const hw_wifi_network_t *)network {
  sys_assert(network);
  self = [super init];
  if (self != nil) {
    _network = network;
  }
  return self;
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

- (NXString *)ssid {
  sys_assert(_network);
  return [NXString stringWithCString:_network->ssid];
}

- (NXString *)bssid {
  sys_assert(_network);
  return [NXString stringWithFormat:@"%02X:%02X:%02X:%02X:%02X:%02X",
                                    _network->bssid[0], _network->bssid[1],
                                    _network->bssid[2], _network->bssid[3],
                                    _network->bssid[4], _network->bssid[5]];
}

- (uint8_t)channel {
  sys_assert(_network);
  return _network->channel;
}

- (int16_t)rssi {
  sys_assert(_network);
  return _network->rssi;
}

///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION

- (NXString *)description {
  sys_assert(_network);
  return [NXString
      stringWithFormat:
          @"<NXWirelessNetwork ssid=%q bssid=%q channel=%d rssi=%ddB>",
          [self ssid], [self bssid], [self channel], [self rssi]];
}

@end
