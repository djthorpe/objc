/**
 * @file examples/NXApplication/timer/main.m
 * @brief Example showing how to create a simple application that
 * uses a timer to perform periodic actions.
 */
#include <NXApplication/NXApplication.h>

//////////////////////////////////////////////////////////////////////////

@interface MyAppDelegate : NXObject <ApplicationDelegate, TimerDelegate>
@end

//////////////////////////////////////////////////////////////////////////

@implementation MyAppDelegate

- (void)applicationDidFinishLaunching:(id)application {
  (void)application; // Unused parameter

  // Create a repeating timer that fires every second
  NXTimer *timer = [NXTimer timerWithInterval:Second repeats:YES];
  objc_assert(timer);

  // Set the timer's delegate to start the timer
  [timer setDelegate:self];
}

- (void)timerFired:(id)timer {
  // This method will be called every second
  NXLog(@"Timer fired = %@", timer);
}

- (void)applicationReceivedSignal:(NXApplicationSignal)signal {
  // Handle the received signal
  NXLog(@"Application received signal: %d", (int)signal);
  [[Application sharedApplication] terminateWithExitStatus:0];
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class]);
}

void *stdout = NULL; // Placeholder for standard output
void *stderr = NULL; // Placeholder for standard error