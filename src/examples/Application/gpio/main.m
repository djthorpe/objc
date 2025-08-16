/**
 * @file  examples/Application/gpio/main.m
 * @brief Example showing how to respond to GPIO events in an
 * application.
 *
 * This example demonstrates how to read the state of GPIO pins and respond to
 * button presses.
 */
#include <Application/Application.h>

#define GPIO_BOOTSEL 30 // BOOTSEL Button
#define GPIO_A 12       // Button A
#define GPIO_B 13       // Button B
#define GPIO_C 14       // Button C

//////////////////////////////////////////////////////////////////////////

@interface MyAppDelegate : NXObject <ApplicationDelegate, GPIODelegate>
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

  // Set GPIO delegate
  [GPIO setDelegate:self];

  // Initialize a GPIO pin for input
  [GPIO pullupWithPin:GPIO_A];
  [GPIO pullupWithPin:GPIO_B];
  [GPIO pullupWithPin:GPIO_C];
  [GPIO inputWithPin:GPIO_BOOTSEL];
}

- (void)gpio:(GPIO *)gpio changed:(GPIOEvent)event {
  // Handle GPIO events
  switch (event) {
  case GPIOEventRising:
    NXLog(@"GPIO pin %d rising edge detected", [gpio pin]);
    break;
  case GPIOEventFalling:
    NXLog(@"GPIO pin %d falling edge detected", [gpio pin]);
    break;
  case GPIOEventChanged:
    NXLog(@"GPIO pin %d state change (rising & falling)", [gpio pin]);
    break;
  default:
    NXLog(@"Unknown GPIO event for pin %d", [gpio pin]);
    break;
  }
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class]);
}
