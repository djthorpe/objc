/**
 * @file NXWireless.h
 * @brief Defines the NXWireless class for managing wireless connections.
 *
 * This file provides the definition for NXWireless, which is used
 * to manage the shared wireless connection instance for managing
 * scanning, connection and disconnection from Wi-Fi networks.
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
@protected
  id<WirelessDelegate> _delegate; ///< The wireless delegate
  hw_wifi_t *_wifi;               ///< The underlying Wi-Fi handle
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

@end
