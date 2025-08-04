/**
 * @file ApplicationDelegate+Protocol.h
 * @brief Defines a protocol for the application delegate.
 *
 * The ApplicationDelegate protocol defines methods that are called by the
 * Application object at key moments in the application's lifecycle. By
 * implementing these methods, objects can respond to application state
 * changes and customize application behaviour.
 *
 * @example examples/NXApplication/helloworld/main.m
 * @example examples/NXApplication/gpio/main.m
 * @example examples/NXApplication/timer/main.m
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
@protocol ApplicationDelegate

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

/**
 * @brief Called when the application receives a system signal.
 * @param signal The type of signal that was received.
 *
 * This method is invoked when the application receives system signals such as
 * termination requests (SIGTERM), interrupt signals (SIGINT/Ctrl+C), or quit
 * signals (SIGQUIT). Implement this method to handle signals gracefully by
 * performing cleanup operations, saving state, or initiating shutdown
 * procedures.
 *
 * If this method is not implemented, the application will terminate
 * with a -1 exit status when it receives a NXApplicationSignalTerm or
 * NXApplicationSignalQuit signal. Other signals may be ignored.
 *
 * In the future, this method may be called for power management events,
 * such as low or empty battery conditions, or sleep conditions, allowing
 * applications to respond appropriately to changes in power status.
 *
 * @note This method may be called from a signal handler context on some
 * platforms. Keep the implementation simple and avoid blocking operations,
 * memory allocation, or complex system calls.
 *
 * @see NXApplicationSignal for available signal types.
 */
- (void)applicationReceivedSignal:(NXApplicationSignal)signal;

@end
