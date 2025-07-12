#include <NXFoundation/NXFoundation.h>

// Define the shared application instance
static id sharedApplication = nil;

@implementation NXApplication

#pragma mark - Lifecycle

- (id)init {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Return success
  return self;
}

- (void)release {
  // Check if the shared application instance is being released
  @synchronized([self class]) {
    if (sharedApplication == self) {
      sharedApplication = nil; // Set to nil to avoid dangling pointer
    }
  }
  [super release]; // Call superclass release
}

#pragma mark - Class Methods

+ (id)sharedApplication {
  @synchronized(self) {
    // Check if the shared application instance already exists
    if (sharedApplication != nil) {
      return sharedApplication; // Return existing instance
    }

    // Create the shared application instance
    sharedApplication = [[self alloc] init];
    if (sharedApplication == nil) {
      panicf("Failed to create shared application instance");
      return nil;
    }

    // Return the shared application instance
    return sharedApplication;
  }
}

+ (int)run {
  // If there is no default zone, create one
  NXZone *zone = [NXZone defaultZone];
  if (zone == nil) {
    zone = [NXZone zoneWithSize:0];
    if (zone == nil) {
      panicf("Failed to create default zone for NXApplication");
      return -1;
    }
  }

  // Get the shared application instance
  NXApplication *app = [self sharedApplication];
  if (app == nil) {
    panicf("No shared application instance available");
    return -1;
  }

  // Call the run method on the shared application instance
  int returnValue = [app run];

  // Release the application instance
  [app release];

  // Release the zone
  [zone release];

  // Return the result of the run method
  return returnValue;
}

#pragma mark - Instance Methods

- (int)run {
  // Start the main run loop
  while (YES) {
    NXLog(@"NXApplication is running...");
    // Process events and maintain application state
    // This is a placeholder for actual event processing logic
    // In a real application, this would handle user input, timers, etc.

    // For now, we just break to avoid an infinite loop in this example
    break;
  }

  // Return success
  return 0;
}

@end
