/**
 * @file examples/NXApplication/helloworld/main.m
 * @brief Example showing how to create a simple application that stops after
 * launching.
 *
 * This example demonstrates how to start an NXApplication application
 * with a custom delegate.
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
  NXTimer *timer = [NXTimer timerWithInterval:1.0 repeats:YES];
  objc_assert(timer);

  // Set the timer's delegate to start the timer
  [timer setDelegate:self];
}

- (void)timerFired:(id)timer {
  // This method will be called every second
  NXLog(@"Timer fired = %@", timer);
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class]);
}

void *stdout = NULL; // Placeholder for standard output
void *stderr = NULL; // Placeholder for standard error