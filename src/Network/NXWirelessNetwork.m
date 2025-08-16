#include "NXWireless+Private.h"
#include <Network/Network.h>

@implementation NXWirelessNetwork

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)initWithNetwork:(const hw_wifi_network_t *)network {
  sys_assert(network);
  self = [super init];
  if (self != nil) {
    sys_memcpy(&_network, network, sizeof(hw_wifi_network_t));
  }
  return self;
}

- (id)initWithName:(id<NXConstantStringProtocol>)name {
  sys_assert(name);
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Check length of name
  if ([name length] > HW_WIFI_SSID_MAX_LENGTH) {
    [self release];
    return nil; // SSID too long
  }

  // Copy the name
  sys_memcpy(_network.ssid, [name cStr], [name length] + 1);

  // Return success
  return self;
}

+ (NXWirelessNetwork *)networkWithName:(id<NXConstantStringProtocol>)name {
  return [[[self alloc] initWithName:name] autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

- (NXString *)ssid {
  return [NXString stringWithCString:_network.ssid];
}

- (NXString *)bssid {
  // Return NULL if BSSID is not available
  if (_network.bssid[0] == 0 && _network.bssid[1] == 0 &&
      _network.bssid[2] == 0 && _network.bssid[3] == 0 &&
      _network.bssid[4] == 0 && _network.bssid[5] == 0) {
    return NULL;
  }
  return [NXString stringWithFormat:@"%02X:%02X:%02X:%02X:%02X:%02X",
                                    _network.bssid[0], _network.bssid[1],
                                    _network.bssid[2], _network.bssid[3],
                                    _network.bssid[4], _network.bssid[5]];
}

- (uint8_t)channel {
  if (_network.channel != 0x00 && _network.channel != 0xFF) {
    return _network.channel;
  }
  return 0;
}

- (int16_t)rssi {
  return _network.rssi;
}

/**
 * @brief Gets the underlying hw_wifi_network_t context.
 */
- (const hw_wifi_network_t *)context {
  return &_network;
}

///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION

- (NXString *)description {
  NXArray *elements = [NXArray arrayWithCapacity:4];

  // Always append the SSID
  [elements append:[NXString stringWithFormat:@"ssid=%q", [self ssid]]];

  // Optionally append the BSSID
  NXString *bssid = [self bssid];
  if (bssid) {
    [elements append:[NXString stringWithFormat:@"bssid=%q", bssid]];
  }

  // Optionally append the channel
  uint8_t channel = [self channel];
  if (channel) {
    [elements append:[NXString stringWithFormat:@"channel=%u",
                                                (unsigned int)channel]];
  }

  // Optionally append the RSSI
  int16_t rssi = [self rssi];
  if (rssi != 0) {
    [elements append:[NXString stringWithFormat:@"rssi=%ddB", (int)rssi]];
  }

  // Return nice formatting
  return [NXString
      stringWithFormat:@"<NXWirelessNetwork %@>",
                       [elements stringWithObjectsJoinedByString:@" "]];
}

@end
