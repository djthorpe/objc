/**
 * @file NetworkTimeDelegate+Protocol.h
 * @brief Defines the NetworkTimeDelegate for network time events.
 *
 * This file defines the NetworkTimeDelegate protocol which provides a
 * standardized interface for handling network time events in the Network
 * framework. Classes conforming to this protocol can implement methods for
 * handling these events.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PROTOCOL DEFINITIONS

/**
 * @brief Protocol for receiving network time update events.
 * @headerfile NetworkTimeDelegate+Protocol.h Network/Network.h
 * @ingroup Network
 *
 * Classes conforming to this protocol receive callbacks for network time
 * updates: one invocation per time update. It is up to the delegate to
 * update the system time based on these events.
 */
@protocol NetworkTimeDelegate

@required
/**
 * @brief Called when a network time update is received.
 * @param time The updated network time.
 */
- (void)networkTimeDidUpdate:(NXDate *)time;

@end
