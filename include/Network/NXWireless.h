/**
 * @file NXWireless.h
 * @brief Defines the NXWireless class for managing wireless connections.
 *
 * This file provides the definition for NXWireless, which is used
 * to manage the shared wireless connection instance for managing
 * scanning, connection and disconnection from Wi-Fi networks.
 *
 * @example examples/Network/scan/main.m
 */
#pragma once

/**
 * @brief A class for managing wireless connections.
 * @ingroup Network
 * @headerfile NXWireless.h Network/Network.h
 *
 * This class provides an interface for managing Wi-Fi connections. There are
 * currently three main operations, which are all handled asynchronously:
 *
 * - Scanning for available networks
 * - Connecting to a selected network
 * - Disconnecting from the current network
 *
 * The NXWireless class is designed to be a singleton, ensuring that there is
 * only one instance managing the wireless connection at any given time.
 */
@interface NXWireless : NXObject {
@private
  id<WirelessDelegate> _delegate; ///< The wireless delegate
  hw_wifi_t *_wifi;               ///< The underlying Wi-Fi handle
  hw_wifi_network_t _network;     ///< The current network information
}

/**
 * @brief A shared instance of the wireless manager.
 *
 * This variable holds the current wireless manager instance that is being used.
 * It is automatically set when a wireless manager is created.
 */
+ (id)sharedInstance;

/**
 * @brief Gets the current wireless delegate.
 * @return The current wireless delegate, or nil if no delegate is set.
 *
 * This method returns the object that serves as the wireless delegate.
 *
 * @see setDelegate: for setting the wireless delegate.
 */
- (id<WirelessDelegate>)delegate;

/**
 * @brief Sets the wireless delegate.
 * @param delegate The object to set as the wireless delegate, or nil to remove
 * the current delegate.
 *
 * The delegate object will receive wireless-related callbacks when the wireless
 * manager detects networks or changes state. The delegate should conform to the
 * WirelessDelegate protocol.
 */
- (void)setDelegate:(id<WirelessDelegate>)delegate;

/**
 * @brief Initiates a scan for available Wi-Fi networks.
 * @return YES if the scan was started successfully, NO otherwise.
 *
 * This method initiates a scan for available Wi-Fi networks.
 * It returns YES if the scan was started successfully, or NO if
 * there was an error (for example, another operation is already
 * in progress).
 *
 * The scanning process is asynchronous, and the results will be
 * delivered via the delegate.
 */
- (BOOL)scan;

/**
 * @brief Begin an asynchronous connection to a Wi‑Fi network.
 * @param network The target network descriptor (SSID/BSSID/etc.).
 * @return YES if the connection attempt was started; NO if another operation
 *         is in progress or parameters are invalid.
 *
 * This variant is intended for open networks or cases where credentials are
 * already known to the platform. The call returns immediately; completion or
 * failure will occur asynchronously.
 */
- (BOOL)connect:(NXWirelessNetwork *)network;

/**
 * @brief Begin an asynchronous connection with an explicit password.
 * @param network  The target network descriptor (SSID/BSSID/etc.).
 * @param password NULL‑terminated password string (may be empty for open
 *                 networks).
 * @return YES if the connection attempt was started; NO otherwise.
 *
 * The call returns immediately; the outcome will be determined asynchronously.
 */
- (BOOL)connect:(NXWirelessNetwork *)network
    withPassword:(id<NXConstantStringProtocol>)password;

/**
 * @brief Disconnect from the current Wi‑Fi network.
 * @return YES if a disconnect was initiated; NO if not connected or another
 *         operation prevents it.
 *
 * The operation is asynchronous; the interface will transition to a
 * disconnected state shortly after the request.
 */
- (BOOL)disconnect;

@end
