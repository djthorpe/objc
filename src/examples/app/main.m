/**
 * @file main.m
 * @brief Example showing how to create a simple application that stops after
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
  (void)application; // Cast to void to avoid unused parameter warning
  NXLog(@"Application did finish launching");
}

@end

//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  // Initialize the NXApplication framework
  return NXApplicationMain(argc, argv, [MyAppDelegate class]);
}

void *stdout = NULL; // Placeholder for standard output
void *stderr = NULL; // Placeholder for standard error