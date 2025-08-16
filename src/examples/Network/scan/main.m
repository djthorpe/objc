/**
 * @file examples/Network/scan/main.m
 * @brief Example showing how to scan for Wi-Fi networks
 */
#include <Application/Application.h>
#include <Network/Network.h>

//////////////////////////////////////////////////////////////////////////

@interface MyAppDelegate : NXObject <ApplicationDelegate, WirelessDelegate> {
}
@end

//////////////////////////////////////////////////////////////////////////

@implementation MyAppDelegate

- (void)applicationDidFinishLaunching:(id)application {
  (void)application; // Unused parameter

  // Initialize the wireless manager
  [[NXWireless sharedInstance] setDelegate:self];

  // Start scanning for Wi-Fi networks
  [[NXWireless sharedInstance] scan];
}

- (void)applicationReceivedSignal:(NXApplicationSignal)signal {
  // Handle the received signal
  NXLog(@"Application received signal: %d", (int)signal);

  // Terminate the application
  [[Application sharedApplication] terminateWithExitStatus:0];
}

/**
 * @brief Called when a wireless network is discovered during a scan.
 */
- (void)scanDidDiscoverNetwork:(NXWirelessNetwork *)network {
  NXLog(@"Scan discovered network: %@", network);
}

/**
 * @brief Called once when the current scan completes (successfully or not).
 */
- (void)scanDidComplete {
  NXLog(@"Scan completed");
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class],
                           NXApplicationCapabilityNone);
}
