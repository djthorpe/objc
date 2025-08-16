#include "NXWireless+Private.h"
#include "NXWirelessNetwork+Private.h"
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
  NXWireless *sender = (NXWireless *)user_data;
  sys_assert(wifi);
  sys_assert(sender);
  sys_assert(event);

  switch (event) {
  case hw_wifi_event_scan:
    if (network) {
      NXWirelessNetwork *networkInfo =
          [[NXWirelessNetwork alloc] initWithNetwork:network];
      if (networkInfo) {
        [sender scanDidDiscoverNetwork:[networkInfo autorelease]];
      }
    } else {
      [sender scanDidComplete];
    }
    break;
  case hw_wifi_event_joining:
    sys_printf("Wi-Fi joining event received\n");
    break;
  case hw_wifi_event_connected:
    sys_printf("Wi-Fi connected event received\n");
    break;
  case hw_wifi_event_disconnected:
    sys_printf("Wi-Fi disconnected event received\n");
    break;
  case hw_wifi_event_badauth:
    sys_printf("Wi-Fi bad authentication event received\n");
    break;
  case hw_wifi_event_notfound:
    sys_printf("Wi-Fi not found event received\n");
    break;
  case hw_wifi_event_error:
    sys_printf("Wi-Fi error event received\n");
    break;
  }
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
    if (self == sharedInstance) {
      sharedInstance = nil;
    }
  }

  // Call superclass dealloc
  [super dealloc];
}

+ (id)sharedInstance {
  @synchronized([self class]) {
    if (sharedInstance == nil) {
      sharedInstance = [[self alloc] init];
    }
    return sharedInstance;
  }
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

- (void)scanDidDiscoverNetwork:(NXWirelessNetwork *)network {
  if (_delegate && object_respondsToSelector(_delegate, @selector
                                             (scanDidDiscoverNetwork:))) {
    [_delegate scanDidDiscoverNetwork:network];
  }
}

- (void)scanDidComplete {
  if (_delegate &&
      object_respondsToSelector(_delegate, @selector(scanDidComplete))) {
    [_delegate scanDidComplete];
  }
}

@end
