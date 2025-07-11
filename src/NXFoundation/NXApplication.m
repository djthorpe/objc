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
    if (sharedApplication == nil) {
      sharedApplication = [[self alloc] init];
    }
  }
  return sharedApplication;
}

#pragma mark - Instance Methods

- (void)run {
  // Start the main run loop
  while (YES) {
    NXLog(@"NXApplication is running...");
    // Process events and maintain application state
    // This is a placeholder for actual event processing logic
    // In a real application, this would handle user input, timers, etc.

    // For now, we just break to avoid an infinite loop in this example
    break;
  }
}

@end

int NXApplicationMain(Class app) {
  // Check if the application class is valid
  if (app == nil) {
    app = [NXApplication class];
  }

  // Create a default zone
  NXZone *zone = [NXZone zoneWithSize:0];
  if (zone == nil) {
    panicf("Failed to create default zone for NXApplication");
    return -1;
  }

  // Create an autorelease pool
  NXAutoreleasePool *pool = [[NXAutoreleasePool allocWithZone:zone] init];
  if (pool == nil) {
    panicf("Failed to create autorelease pool for NXApplication");
    return -1;
  }

  // Create the shared application instance
  NXApplication *application = [app sharedApplication];
  if (application == nil) {
    panicf("Failed to create shared application instance");
    return -1;
  } else if (![application isKindOfClass:[NXApplication class]]) {
    panicf("Shared application instance is not of type NXApplication");
    return -1;
  }

  // Run the application
  [application run];

  // Release the application instance
  [application release];

  // Release the autorelease pool first, then the zone
  [pool release];
  [zone release];

  // Return success
  return 0;
}