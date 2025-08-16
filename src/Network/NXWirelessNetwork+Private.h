#include <Network/Network.h>
#include <runtime-hw/hw.h>

/**
 * @brief Category for private methods of the NXWirelessNetwork class.
 */
@interface NXWirelessNetwork (Private)

/**
 * @brief Initializes a new NXWirelessNetwork instance with the specified
 * network.
 * @param network The network to associate with the NXWirelessNetwork instance.
 * @return An initialized NXWirelessNetwork instance, or nil if the
 * initialization failed.
 */
- (id)initWithNetwork:(const hw_wifi_network_t *)network;

@end
