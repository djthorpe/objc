#include <NXApplication/NXApplication.h>
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

// Define the shared application instance
static id sharedApplication = nil;

@implementation Application

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)init {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Initialize the application data
  _data = NULL;
  _delegate = nil;
  _run = NO;
  _stop = NO;

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

  _delegate = nil; // Clear the delegate reference
  [super release]; // Call superclass release
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Gets the current application delegate.
 */
- (id<ApplicationDelegate>)delegate {
  return _delegate; // Return the current delegate
}

/**
 * @brief Sets the application delegate.
 */
- (void)setDelegate:(id<ApplicationDelegate>)delegate {
  _delegate = delegate;
}

///////////////////////////////////////////////////////////////////////////////
// INSTANCE METHODS

- (int)run {
  if (_run) {
    NXLog(@"Application is already running.");
    return 0; // Already running
  }

  // Start the main run loop
  while (_stop == NO) {
    NXLog(@"NXApplication is running...");

    // Notify the delegate that the application has finished launching
    if (_run == NO && _delegate != nil) {
      [_delegate applicationDidFinishLaunching:self];
      _run = YES; // Set the run flag to true
    }

    // Process events and maintain application state
    // This is a placeholder for actual event processing logic
    // In a real application, this would handle user input, timers, etc.

    // For now, we just sleep a while
    if (_run) {
      NXLog(@"Processing events...");
      // Simulate event processing
      sys_sleep(100); // Sleep for 100 milliseconds
    }
  }

  // Reset the flags
  _run = NO;  // Reset the run flag
  _stop = NO; // Reset the stop flag

  // Return success
  return 0;
}

- (void)stop {
  _stop = YES; // Set the stop flag to true to exit the run loop
  NXLog(@"NXApplication is stopping...");
}

///////////////////////////////////////////////////////////////////////////////
// CLASS METHODS

+ (id)sharedApplication {
  @synchronized(self) {
    // Check if the shared application instance already exists
    if (sharedApplication != nil) {
      return sharedApplication; // Return existing instance
    }

    // Create the shared application instance
    sharedApplication = [[self alloc] init];
    if (sharedApplication == nil) {
      sys_panicf("Failed to create shared application instance");
      return nil;
    }

    // Return the shared application instance
    return sharedApplication;
  }
}

@end
