/**
 * @file WirelessDelegate+Protocol.h
 * @brief Defines the WirelessDelegateProtocol for wireless events.
 *
 * This file defines the WirelessDelegate protocol which provides a standardized
 * interface for handling wireless events in the Network framework. Classes
 * conforming to this protocol can implement methods for handling these events.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PROTOCOL DEFINITIONS

/**
 * @brief Protocol for receiving wireless scan events.
 * @headerfile WirelessDelegate+Protocol.h Network/Network.h
 * @ingroup Network
 *
 * Classes conforming to this protocol receive callbacks for scan progress:
 * one invocation per network discovered and a final notification on
 * completion. Implementations should return quickly and avoid heavy work
 * on the callback thread.
 */
@protocol WirelessDelegate

/**
 * @brief Called when a wireless network is discovered during a scan.
 * @param network The network information for the discovered access point.
 *                This object may be short‑lived; copy fields you need to
 *                retain beyond the scope of this callback.
 */
- (void)scanDidDiscoverNetwork:(NXWirelessNetwork *)network;

/**
 * @brief Called once when the current scan completes (successfully or not).
 *
 * No further scanDidDiscoverNetwork: callbacks will be delivered after this
 * method for the corresponding scan operation.
 */
- (void)scanDidComplete;

@end
