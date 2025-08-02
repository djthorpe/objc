#include <NXApplication/NXApplication.h>
#include <NXFoundation/NXFoundation.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>

int NXApplicationMain(int argc, char *argv[], Class delegate) {
  sys_init();
  hw_init();

  (void)argc; // Currently unused parameter
  (void)argv; // Currently unused parameter

  // If there is no default zone, create one
  BOOL releaseZone = NO;
  NXZone *zone = [NXZone defaultZone];
  if (zone == nil) {
    // Default zone size is set to 64KB
    zone = [NXZone zoneWithSize:64 * 1024];
    if (zone == nil) {
      sys_panicf("Failed to create default zone");
      return -1;
    } else {
      releaseZone = YES;
    }
  }

  // Create an autorelease pool for memory management
  NXAutoreleasePool *pool = [[NXAutoreleasePool allocWithZone:zone] init];
  if (pool == nil) {
    sys_panicf("Failed to create autorelease pool");
    [zone release];
    return -1;
  }

  // Get the shared application instance
  Application *app = [Application sharedApplication];
  if (app == nil) {
    sys_panicf("No shared application instance available");
    return -1;
  }

  // Create and set the application delegate
  id<ApplicationDelegate> appDelegate = nil;
  if (delegate != nil) {
    appDelegate = [[delegate allocWithZone:zone] init];
    if (appDelegate == nil) {
      sys_panicf("Failed to create application delegate");
      [zone release];
      return -1;
    } else {
      [app setDelegate:appDelegate];
    }
  }

  // Call the run method on the shared application instance
  int returnValue = [app run];

  // Release the application delegate, instance, pool and zone
  [app release];
  [appDelegate release];
  [pool release];
  if (releaseZone) {
    [zone release];
  }

  // Clean up the system resources
  hw_exit();
  sys_exit();

  // Return the result of the run method
  return returnValue;
}
