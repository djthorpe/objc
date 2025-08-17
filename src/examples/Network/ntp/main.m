/**
 * @file examples/Network/connect/main.m
 * @file examples/Network/ntp/main.m
 * @file examples/Network/ntp/main.m
 * @brief Example showing how to connect to Wi-Fi networks
 */
#include <Application/Application.h>
#include <Network/Network.h>

#ifndef WIFI_SSID
#error "WIFI_SSID not defined"
#endif

#ifndef WIFI_PASSWORD
#error "WIFI_PASSWORD not defined"
#endif

//////////////////////////////////////////////////////////////////////////

@interface MyAppDelegate
    : NXObject <ApplicationDelegate, WirelessDelegate, NetworkTimeDelegate>
@end

//////////////////////////////////////////////////////////////////////////

@implementation MyAppDelegate

- (void)applicationDidFinishLaunching:(id)application {

  // Initialize the wireless manager
  NXWireless *wifi = [NXWireless sharedInstance];
  if (wifi == nil) {
    NXLog(@"WiFi not supported on this board");
    [application terminateWithExitStatus:-1];
    return;
  }

  // Set the delegate for wireless events
  [wifi setDelegate:self];

  // Initialise the network time manager
  [[NXNetworkTime sharedInstance] setDelegate:self];

  // Determine network to connect to
  NXWirelessNetwork *net = [NXWirelessNetwork networkWithName:@WIFI_SSID];
  sys_assert(net);

  // Connect to the network
  if ([wifi connect:net withPassword:@WIFI_PASSWORD] == NO) {
    NXLog(@"Failed to initiate connection to network: %@", net);
  }
}

- (void)applicationReceivedSignal:(NXApplicationSignal)signal {
  // Handle the received signal
  NXLog(@"Application received signal: %d", (int)signal);

  // Terminate the application
  [[Application sharedApplication] terminateWithExitStatus:0];
}

/**
 * @brief Called when a connection attempt starts.
 */
- (void)connectDidStart:(NXWirelessNetwork *)network {
  NXLog(@"Connection started to network: %@", network);
}

/**
 * @brief Called if the connection fails.
 */
- (void)connectionFailed:(NXWirelessNetwork *)network
               withError:(NXWirelessError)error {
  NXLog(@"Connection failed to network: %@, error: %d", network, (int)error);
  switch (error) {
  case NXWirelessErrorBadAuth:
    NXLog(@" >>>> Bad authentication");
    break;
  case NXWirelessErrorNotFound:
    NXLog(@" >>>> Network not found");
    break;
  case NXWirelessErrorGeneral:
    NXLog(@" >>>> General error");
    break;
  }
}

/**
 * @brief Called when a connection is established, and then occasionally
 * after this with signal strength updates.
 */
- (void)connected:(NXWirelessNetwork *)network {
  NXLog(@"Connected to network: %@", network);
}

/**
 * @brief Called after disconnecting from a network.
 */
- (void)disconnected:(NXWirelessNetwork *)network {
  NXLog(@"Disconnected from network: %@", network);
}

/**
 * @brief Called after NTP time is received.
 */
- (void)networkTimeDidUpdate:(NXDate *)time {
  // Handle the network time update
  NXLog(@"Network time updated: %@", time);
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class],
                           NXApplicationCapabilityNone);
}
