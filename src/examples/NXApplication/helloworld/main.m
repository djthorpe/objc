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

@interface MyAppDelegate : NXObject <ApplicationDelegate>
@end

//////////////////////////////////////////////////////////////////////////

@implementation MyAppDelegate

- (void)applicationDidFinishLaunching:(id)application {
  // Print out the arguments passed to the application
  NXLog(@"Application did finish launching with arguments: %@",
        [application args]);

  // Stop the application immediately after launching
  [application terminate];
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class]);
}
