#include <Network/Network.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// Define the shared instance
static id sharedInstance = nil;

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS

static void _wifi_callback(hw_wifi_t *wifi, hw_wifi_event_t event,
                           const hw_wifi_network_t *network, void *user_data) {
  sys_assert(wifi);
  sys_assert(user_data);
  (void)event;
  (void)network;
  //  NXWireless *sender = (NXWireless *)user_data;
  sys_printf("Wi-Fi event received");
}

///////////////////////////////////////////////////////////////////////////////

@implementation NXWireless

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)init {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Initialize the Wi-Fi handle
  _wifi = hw_wifi_init(NULL, _wifi_callback, self);
  if (!hw_wifi_valid(_wifi)) {
    [self release];
    return nil; // Initialization failed
  }

  // Return success
  return self;
}

- (void)dealloc {
  @synchronized([self class]) {
    // Finalize the Wi-Fi handle
    hw_wifi_finalize(_wifi);
    _wifi = NULL;

    // Reset the shared instance to nil
    sharedInstance = nil;
  }

  // Call superclass dealloc
  [super dealloc];
}

+ (id)sharedInstance {
  @synchronized([self class]) {
    if (sharedInstance == nil) {
      // TODO: Should this be autoreleased?
      sharedInstance = [[self alloc] init];
    }
  }
  return sharedInstance;
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Gets the current wireless delegate.
 */
- (id<WirelessDelegate>)delegate {
  @synchronized(self) {
    return _delegate;
  }
}

/**
 * @brief Sets the wireless delegate.
 */
- (void)setDelegate:(id<WirelessDelegate>)delegate {
  @synchronized(self) {
    if (_delegate == delegate) {
      return; // No change
    }
    _delegate = delegate;
  }
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

- (BOOL)scan {
  return hw_wifi_scan(_wifi);
}

@end
