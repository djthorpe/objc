/**
 * @file ApplicationDelegate+Protocol.h
 * @brief Defines a protocol for the application delegate.
 *
 * The ApplicationDelegate protocol defines methods that are called by the
 * Application object at key moments in the application's lifecycle. By
 * implementing these methods, objects can respond to application state
 * changes and customize application behavior.
 */
#pragma once

/**
 * @protocol ApplicationDelegate
 * @ingroup Application
 * @headerfile ApplicationDelegate+Protocol.h NXApplication/NXApplication.h
 * @brief A protocol that defines the methods for an application delegate.
 *
 * The ApplicationDelegate protocol provides a way for objects to receive
 * notifications about application lifecycle events. Classes that conform to
 * this protocol can be set as the application's delegate to handle these
 * events.
 */
@protocol ApplicationDelegate <RetainProtocol>

@required

/**
 * @brief Called when the application has finished launching.
 * @param application The application instance that finished launching.
 *
 * This method is called once during the application's lifecycle, after
 * the application has completed its initialization and is ready to begin
 * processing events. This is the appropriate place to perform any final
 * setup or initialization that your application requires.
 *
 * @note This method is called from the main run loop thread.
 * @note The application's run loop will begin processing events after
 * this method returns.
 */
- (void)applicationDidFinishLaunching:(id)application;

@optional

/**
 * @brief Called immediately before the application terminates.
 * @param application The application instance that is terminating.
 *
 * This method is called just before the application actually terminates.
 * Use this method to perform any final cleanup tasks.
 */
- (void)applicationWillTerminate:(id)application;

@end
