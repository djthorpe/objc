/**
 * @file main.m
 * @brief An application how to create a simple application that stops after
 * launching.
 *
 * This example demonstrates how to read the system date and time,
 * format it, and print it to the console. It uses the sys_date_t structure
 * to represent the current date and time, and sys_date_get_now() to retrieve
 * the current date and time.
 */
#include <NXApplication/NXApplication.h>

//////////////////////////////////////////////////////////////////////////

@interface MyAppDelegate : NXObject <ApplicationDelegate>
@end

//////////////////////////////////////////////////////////////////////////

@implementation MyAppDelegate

- (void)applicationDidFinishLaunching:(id)application {
  NXLog(@"Application did finish launching");
  [application stop];
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class]);
}
