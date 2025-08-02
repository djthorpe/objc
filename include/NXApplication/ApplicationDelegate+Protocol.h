/**
 * @file ApplicationDelegate+Protocol.h
 * @brief Defines a protocol for the application delegate.
 */
#pragma once

/**
 * @protocol ApplicationDelegate
 * @ingroup Application
 * @headerfile ApplicationDelegate+Protocol.h NXApplication/NXApplication.h
 * @brief A protocol that defines the methods required for an application
 * delegate.
 *
 */
@protocol ApplicationDelegate <RetainProtocol>
@required

/**
 * @brief Called when the application has finished launching.
 * @param application The application instance.
 */
- (void)applicationDidFinishLaunching:(id)application;

@end
