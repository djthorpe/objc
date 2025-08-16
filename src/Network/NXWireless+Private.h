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

@end
