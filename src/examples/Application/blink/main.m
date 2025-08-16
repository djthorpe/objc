/**
 * @file examples/Application/blink/main.m
 * @brief Example showing how to blink the on-board status LED
 * with various mechanisms!
 */
#include <Application/Application.h>

//////////////////////////////////////////////////////////////////////////

@interface MyAppDelegate : NXObject <ApplicationDelegate, TimerDelegate> {
  LED *_led;
  NXTimer *_timer;
}
@end

//////////////////////////////////////////////////////////////////////////

@implementation MyAppDelegate

- (void)applicationDidFinishLaunching:(id)application {
  (void)application; // Unused parameter

  // Get the on-board status LED
  _led = [[LED status] retain];
  if (_led == nil) {
    NXLog(@"Failed to get status LED");
    [[Application sharedApplication] terminateWithExitStatus:-1];
    return;
  }

  // Create a repeating timer that fires every second
  _timer = [[NXTimer timerWithInterval:Second repeats:YES] retain];
  objc_assert(_timer);

  // Set the timer's delegate to start the timer
  [_timer setDelegate:self];
}

- (void)timerFired:(id)timer {
  (void)timer;

  // Count is the current counter
  static int count = 0;

  // This method will be called every second
  switch (count++) {
  case 0:
    NXLog(@"LED: ON");
    [_led setState:YES];
    break;
  case 1:
    NXLog(@"LED: OFF");
    [_led setState:NO];
    break;
  case 2:
    NXLog(@"LED: 0%% brightness");
    [_led setBrightness:0x00];
    break;
  case 3:
    NXLog(@"LED: 100%% brightness");
    [_led setBrightness:0xFF];
    break;
  default:
    NXLog(@"LED: OFF");
    [_led setState:NO];

    // Reset counter
    count = 0;
  }
}

- (void)applicationReceivedSignal:(NXApplicationSignal)signal {
  // Handle the received signal
  NXLog(@"Application received signal: %d", (int)signal);

  // Release the retained resources
  [_timer release];
  [_led release];

  // Terminate the application
  [[Application sharedApplication] terminateWithExitStatus:0];
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class],
                           NXApplicationCapabilityNone);
}
