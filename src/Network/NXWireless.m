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

  NXWirelessNetwork *networkInfo = nil;
  if (network) {
    networkInfo = [[NXWirelessNetwork alloc] initWithNetwork:network];
  }

  // TODO: Should push this into the application queue, rather than calling
  // directly

  switch (event) {
  case hw_wifi_event_scan:
    if (network) {
      [sender scanDidDiscoverNetwork:[networkInfo autorelease]];
    } else {
      [sender scanDidComplete];
    }
    break;
  case hw_wifi_event_joining:
    [sender connectDidStart:networkInfo];
    break;
  case hw_wifi_event_connected:
    [sender connected:networkInfo];
    break;
  case hw_wifi_event_disconnected:
    [sender disconnected:networkInfo];
    break;
  case hw_wifi_event_badauth:
    [sender connectionFailed:networkInfo withError:NXWirelessErrorBadAuth];
    break;
  case hw_wifi_event_notfound:
    [sender connectionFailed:networkInfo withError:NXWirelessErrorNotFound];
    break;
  case hw_wifi_event_error:
    [sender connectionFailed:networkInfo withError:NXWirelessErrorGeneral];
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
#ifdef DEBUG
    sys_printf("hw_wifi_init() failed\n");
#endif
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

/**
 * @brief A shared instance of the wireless manager.
 */
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
// PUBLIC METHODS

/**
 * @brief Initiates a scan for available Wi-Fi networks.
 */
- (BOOL)scan {
  return hw_wifi_scan(_wifi);
}

/**
 * @brief Begin an asynchronous connection to a Wi‑Fi network.
 */
- (BOOL)connect:(NXWirelessNetwork *)network {
  // TODO: In future, we can store some passwords and use those if needed
  return hw_wifi_connect(_wifi, [network context], NULL);
}

/**
 * @brief Begin an asynchronous connection with an explicit password.
 */
- (BOOL)connect:(NXWirelessNetwork *)network
    withPassword:(id<NXConstantStringProtocol>)password {
  return hw_wifi_connect(_wifi, [network context], [password cStr]);
}

/**
 * @brief Disconnect from the current Wi‑Fi network.
 */
- (BOOL)disconnect {
  return hw_wifi_disconnect(_wifi);
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Called when a wireless network is discovered during a scan.
 */
- (void)scanDidDiscoverNetwork:(NXWirelessNetwork *)network {
  if (_delegate && object_respondsToSelector(_delegate, @selector
                                             (scanDidDiscoverNetwork:))) {
    [_delegate scanDidDiscoverNetwork:network];
  }
}

/**
 * @brief Called once when the current scan completes (successfully or not).
 */
- (void)scanDidComplete {
  if (_delegate &&
      object_respondsToSelector(_delegate, @selector(scanDidComplete))) {
    [_delegate scanDidComplete];
  }
}

/**
 * @brief Called when a connection attempt starts.
 */
- (void)connectDidStart:(NXWirelessNetwork *)network {
  if (_delegate &&
      object_respondsToSelector(_delegate, @selector(connectDidStart:))) {
    [_delegate connectDidStart:network];
  }
}

/**
 * @brief Called if the connection fails.
 */
- (void)connectionFailed:(NXWirelessNetwork *)network
               withError:(NXWirelessError)error {
  if (_delegate && object_respondsToSelector(_delegate, @selector
                                             (connectionFailed:withError:))) {
    [_delegate connectionFailed:network withError:error];
  }
}

/**
 * @brief Called when a connection is established.
 */
- (void)connected:(NXWirelessNetwork *)network {
  @synchronized(self) {
    // Copy the network information
    sys_memcpy(&_network, [network context], sizeof(hw_wifi_network_t));
  }
  if (_delegate &&
      object_respondsToSelector(_delegate, @selector(connected:))) {
    [_delegate connected:network];
  }
}

/**
 * @brief Called after disconnecting from a network.
 */
- (void)disconnected:(NXWirelessNetwork *)network {
  if (_delegate &&
      object_respondsToSelector(_delegate, @selector(disconnected:))) {
    [_delegate disconnected:network];
  }
  @synchronized(self) {
    // Set all bytes to zero of _network
    sys_memset(&_network, 0, sizeof(hw_wifi_network_t));
  }
}

@end
