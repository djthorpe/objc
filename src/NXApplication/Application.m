#include <NXApplication/NXApplication.h>
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static void _app_runloop(sys_event_queue_t *queue);

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// Define the shared application instance
static id sharedApplication = nil;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation Application

- (id)initWithCapacity:(size_t)capacity {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Create an event queue for the application
  _queue = sys_event_queue_init(capacity);
  _delegate = nil;
  _run = NO;

  // Return success
  return self;
}

- (id)initWithArgs:(NXArray *)args capacity:(size_t)capacity {
  self = [self initWithCapacity:capacity];
  if (self) {
    _args = [args retain]; // Retain the command-line arguments
  }
  return self;
}

- (id)init {
  return [self initWithArgs:nil capacity:20]; // Default capacity of 20 events
}

- (void)release {
  // Check if the shared application instance is being released
  @synchronized([self class]) {
    if (sharedApplication == self) {
      sharedApplication = nil; // Set to nil to avoid dangling pointer
    }
  }

  // Release retained resources (delegates are not retained)
  [_args release];

  // Finalize the event queue
  sys_event_queue_finalize(&_queue);

  // Clear the properties
  _delegate = nil;
  _args = nil;

  // Call superclass release
  [super release];
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

/**
 * @brief Sets command-line arguments passed to the application.
 */
- (void)setArgs:(NXArray *)args {
  [_args release];       // Release previous args
  _args = [args retain]; // Retain the new args
}

/**
 * @brief Returns command-line arguments passed to the application.
 */
- (NXArray *)args {
  if (_args == nil) {
    // If args are not set, return an empty array
    return [NXArray new];
  }
  return _args; // Return the command-line arguments
}

///////////////////////////////////////////////////////////////////////////////
// INSTANCE METHODS

- (int)run {
  if (_run) {
    NXLog(@"Application is already running.");
    return 0; // Already running
  }

  // Run the loop until the stop flag is set
  while (true) {
    // Notify the delegate that the application has finished launching
    if (_run == NO && _delegate != nil) {
      [_delegate applicationDidFinishLaunching:self];
      _run = YES; // Set the run flag to true
    }

    // Get an event from the queue
    sys_event_t event = sys_event_queue_pop(&_queue);
    if (event == NULL) {
      // No more events to process
      break;
    }

    // Simulate processing the event
    sys_printf("core %d: Processing event: %s (queue size=%d)\n",
               sys_thread_core(), (char *)event, sys_event_queue_size(&_queue));

    // Free the allocated string
    sys_free(event);
  }

  // Reset the flags
  _run = NO;

  // Return success
  return 0;
}

- (void)stop {
  NXLog(@"NXApplication is stopping...");
  sys_event_queue_shutdown(&_queue); // Shutdown the event queue
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

/////////////////////////////////////////////////////////////////////
// RUNLOOP

