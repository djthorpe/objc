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
 *
 * It's used both for discovering networks, connecting to networks
 * and reporting on network status.
 */
@interface NXWirelessNetwork : NXObject {
@private
  const hw_wifi_network_t *_network; ///< The wireless network information
}

/**
 * @brief Create a wireless network descriptor with the given SSID.
 * @param name The SSID string (conforming to NXConstantStringProtocol).
 *             Typical SSIDs are 0–32 bytes long.
 * @return A new NXWirelessNetwork instance initialized with the provided
 *         name, or nil on error.
 */
+ (NXWirelessNetwork *)networkWithName:(id<NXConstantStringProtocol>)name;

/**
 * @brief The Service Set Identifier (network name).
 * @return SSID as an NXString.
 */
- (NXString *)ssid;

/**
 * @brief The Basic Service Set Identifier (access point MAC address).
 * @return BSSID as an NXString (implementation-defined formatting; typically
 *         a 6‑octet hexadecimal string such as "AA:BB:CC:DD:EE:FF"). If not
 * defined, will return NULL.
 */
- (NXString *)bssid;

/**
 * @brief Primary channel number for this network.
 * @return The Wi‑Fi channel number, or zero if not defined.
 */
- (uint8_t)channel;

/**
 * @brief Received signal strength indicator.
 * @return RSSI in dBm (typically negative;  values closer to zero indicate
 * stronger signal). Returns zero if not defined.
 */
- (int16_t)rssi;

@end
