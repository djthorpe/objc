/**
 * @file  examples/NXApplication/gpio/main.m
 * @brief Example showing how to respond to GPIO events in an NXApplication
 * application.
 *
 * This example demonstrates how to read the state of GPIO pins and respond to
 * button presses.
 */
#include <NXApplication/NXApplication.h>

#define GPIO_BOOTSEL 23 // BOOTSEL Button
#define GPIO_A 12       // Button A
#define GPIO_B 13       // Button B
#define GPIO_C 14       // Button C

//////////////////////////////////////////////////////////////////////////

@interface MyAppDelegate : NXObject <ApplicationDelegate>
@end

//////////////////////////////////////////////////////////////////////////

@implementation MyAppDelegate

- (void)applicationDidFinishLaunching:(id)application {
  // If GPIO is not supported, log a warning and stop the application
  if ([GPIO count] == 0) {
    NXLog(@"GPIO is not supported on this platform");
    [application terminateWithExitStatus:-1];
    return;
  }

  // Initialize a GPIO pin for input
  [GPIO pullupWithPin:GPIO_A];
  [GPIO pullupWithPin:GPIO_B];
  [GPIO pullupWithPin:GPIO_C];
  [GPIO inputWithPin:GPIO_BOOTSEL];
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class]);
}

void *stdout = NULL; // Placeholder for standard output
void *stderr = NULL; // Placeholder for standard error