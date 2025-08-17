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
 * updates: one invocation per time update. The delegate should return YES
 * if the system time should be updated.
 */
@protocol NetworkTimeDelegate

/**
 * @brief Called when a network time is received.
 * @param time The received network time.
 * @return YES if the system time should be updated, NO otherwise.
 */
- (BOOL)networkTimeShouldUpdate:(NXDate *)time;

@end
