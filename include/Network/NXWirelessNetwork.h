/**
 * @file NXWirelessNetwork.h
 * @brief Defines the NXWirelessNetwork class for specifying wireless network
 * information.
 *
 * This file provides the definition for NXWirelessNetwork, which is used
 * to represent a Wi-Fi network and its associated properties.
 */
#pragma once

/**
 * @brief A class for representing a wireless network.
 * @ingroup Network
 * @headerfile NXWirelessNetwork.h Network/Network.h
 *
 * This class provides an interface for accessing information about a
 * specific Wi-Fi network, including its SSID, signal strength, and
 * security type.
 */
@interface NXWirelessNetwork : NXObject {
@private
  const hw_wifi_network_t *_network; ///< The wireless network information
}

@end
