#include <Network/Network.h>

/**
 * @brief Category for private methods of the NXWireless class.
 */
@interface NXWireless (Private)

/**
 * @brief Called when a wireless network is discovered during a scan.
 */
- (void)scanDidDiscoverNetwork:(NXWirelessNetwork *)network;

/**
 * @brief Called once when the current scan completes (successfully or not).
 */
- (void)scanDidComplete;

/**
 * @brief Called when a connection attempt starts.
 */
- (void)connectDidStart:(NXWirelessNetwork *)network;

/**
 * @brief Called if the connection fails.
 */
- (void)connect:(NXWirelessNetwork *)network withError:(NXWirelessError)error;

/**
 * @brief Called when a connection is established.
 */
- (void)connected:(NXWirelessNetwork *)network;

/**
 * @brief Called after disconnecting from a network.
 */
- (void)disconnected:(NXWirelessNetwork *)network;

@end
