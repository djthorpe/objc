/**
 * @file examples/Application/timer/main.m
 * @brief Example showing how to create a simple application that
 * uses a timer to perform periodic actions.
 */
#include <Application/Application.h>

//////////////////////////////////////////////////////////////////////////

@interface MyAppDelegate : NXObject <ApplicationDelegate, TimerDelegate> {
  NXTimer *_timer;
}
@end

//////////////////////////////////////////////////////////////////////////

@implementation MyAppDelegate

- (void)applicationDidFinishLaunching:(id)application {
  (void)application; // Unused parameter

  // Create a repeating timer that fires every second
  _timer = [[NXTimer timerWithInterval:Second repeats:YES] retain];
  objc_assert(_timer);

  // Set the timer's delegate to start the timer
  [_timer setDelegate:self];
}

- (void)timerFired:(id)timer {
  static int count = 0;
  (void)timer;

  // This method will be called every second
  NXLog(@"Timer fired: %d", ++count);
}

- (void)applicationReceivedSignal:(NXApplicationSignal)signal {
  // Handle the received signal
  NXLog(@"Application received signal: %d", (int)signal);
  [[Application sharedApplication] terminateWithExitStatus:0];

  // Release the timer
  [_timer release];
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class],
                           NXApplicationCapabilityNone);
}
