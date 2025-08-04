#include <NXApplication/NXApplication.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// Define the shared application instance
static id sharedApplication = nil;

// Define the shared queue for events
static sys_event_queue_t _app_queue = {0};

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS

static void _app_gpio_callback(uint8_t pin, hw_gpio_event_t event,
                               void *userdata) {
  (void)userdata; // Unused parameter - NXApplication

  // Get the queue
  sys_event_queue_t *queue = &_app_queue;
  objc_assert(queue);

  // If the queue is not valid, return early
  if (!sys_event_queue_valid(queue)) {
    return;
  }

  // Create a sys_event_t for the GPIO event
  // Sender from pin
  // We also want to include the event type (rising|falling) in the payload
  const char *payload = sys_malloc(sizeof(uint8_t) * 64);
  if (payload != NULL) {
    sys_sprintf((char *)payload, 64, "GPIO %d: %d", (int)pin, (int)event);
    if (sys_event_queue_try_push(queue, (void *)payload) == false) {
      sys_free((void *)payload); // Free the payload if it cannot be pushed
    }
  }
}

/**
 * @brief Callback function for application timer events.
 */
void _app_timer_callback(sys_timer_t *timer) {
  objc_assert(timer);

  sys_event_queue_t *queue = &_app_queue;
  objc_assert(queue);

  // If the queue is not valid, return early
  if (!sys_event_queue_valid(queue)) {
    return;
  }

  // Create a sys_event_t for the Timer event
  // sender=timer->userdata (NXTimer)
  const char *payload = sys_malloc(sizeof(uint8_t) * 64);
  if (payload != NULL) {
    sys_sprintf((char *)payload, 64, "Timer %p", (void *)timer->userdata);
    if (sys_event_queue_try_push(queue, (void *)payload) == false) {
      sys_free((void *)payload); // Free the payload if it cannot be pushed
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation Application

- (id)initWithCapacity:(size_t)capacity {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Create an event queue for the application
  objc_assert(sys_event_queue_valid(&_app_queue) == false);
  _app_queue = sys_event_queue_init(capacity);

  // Initialize properties
  _delegate = nil;
  _run = NO;
  _exitstatus = 0;

  // Set the GPIO callback for the application, with the application instance as
  // userdata
  hw_gpio_set_callback(_app_gpio_callback, self);

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

  // Remove the GPIO callback for the application
  hw_gpio_set_callback(NULL, NULL);

  // Release retained resources (delegates are not retained)
  [_args release];

  // Finalize the event queue
  sys_event_queue_finalize(&_app_queue);

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
    return [[[NXArray alloc] init] autorelease];
  }
  return _args; // Return the command-line arguments
}

///////////////////////////////////////////////////////////////////////////////
// INSTANCE METHODS

- (int)run {
  if (_run) {
    return -1; // Already running
  }

  // Run the loop until the stop flag is set
  while (true) {
    // Notify the delegate that the application has finished launching
    if (_run == NO && _delegate != nil) {
      [_delegate applicationDidFinishLaunching:self];
      _run = YES; // Set the run flag to true
    }

    // TODO: Drain the autorelease pool occasionally
    // In our semantics, we likely have one pool which is used across threads?
    // or do we have one pool per thread?

    // Get an event from the queue
    // The queue might be invalid, as it's been shutdown
    sys_event_t event = sys_event_queue_pop(&_app_queue);
    if (event == NULL) {
      // No more events to process
      break;
    }

    // Simulate processing the event
    sys_printf("core %d: Processing event: %s (queue size=%d)\n",
               sys_thread_core(), (char *)event,
               sys_event_queue_size(&_app_queue));

    // Free the allocated event - release it
    sys_free(event);
  }

  // Reset the flags
  _run = NO;

  // Return success
  return _exitstatus;
}

/**
 * @brief This method notifies the app that you want to exit the run loop.
 */
- (void)terminate {
  objc_assert(sys_event_queue_valid(&_app_queue));

  // Shutdown the event queue
  // The run loop will exit on the next iteration
  sys_event_queue_shutdown(&_app_queue);
}

/**
 * @brief This method notifies the app that you want to exit the run loop,
 * with a specific exit status.
 */
- (void)terminateWithExitStatus:(int)status {
  _exitstatus = status;
  [self terminate];
}

/**
 * @brief Handles application signals.
 * @param signal The signal received from the environment.
 */
- (void)signal:(NXApplicationSignal)signal {
  id<ApplicationDelegate, ObjectProtocol> delegate =
      (id<ApplicationDelegate, ObjectProtocol>)_delegate;
  if (delegate &&
      [delegate respondsToSelector:@selector(applicationReceivedSignal:)]) {
    [delegate applicationReceivedSignal:signal];
  } else if (signal & NXApplicationSignalTerm ||
             signal & NXApplicationSignalQuit ||
             signal & NXApplicationSignalInt) {
    [self terminateWithExitStatus:-1];
  }
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
