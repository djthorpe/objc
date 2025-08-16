/**
 * @file NXNetworkTime.h
 * @brief Defines the NXNetworkTime class for managing network time
 * synchronization.
 *
 * This file provides the definition for NXNetworkTime, which is used
 * to manage the shared network time instance for synchronizing
 * system time with a remote NTP server.
 *
 * @example examples/Network/ntp/main.m
 */
#pragma once

/**
 * @brief A class for managing network time synchronization.
 * @ingroup Network
 */
@interface NXNetworkTime : NXObject {
@private
  id<NetworkTimeDelegate> _delegate; ///< The network time delegate
  net_ntp_t *_ntp;                   ///< The underlying NTP handle
}

/**
 * @brief A shared instance of the network time manager.
 *
 * This variable holds the current network time manager instance that is being
 * used. It is automatically set when a network time manager is created.
 */
+ (id)sharedInstance;

/**
 * @brief Gets the current network time delegate.
 * @return The current network time delegate, or nil if no delegate is set.
 *
 * This method returns the object that serves as the network time delegate.
 */
- (id<NetworkTimeDelegate>)delegate;

/**
 * @brief Sets the network time delegate.
 * @param delegate The object to set as the network time delegate, or nil to
 * remove the current delegate.
 *
 * The delegate object will receive network time-related callbacks when the
 * time is updated from the network. The delegate should conform to the
 * NetworkTimeDelegate protocol.
 */
- (void)setDelegate:(id<NetworkTimeDelegate>)delegate;

@end
