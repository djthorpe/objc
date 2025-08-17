#include <Network/Network.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// Define the shared instance
static id sharedInstance = nil;

///////////////////////////////////////////////////////////////////////////////
// PRIVATE CATEGORIES

@interface NXDate ()
+ (NXDate *)dateWithSystemDate:(const sys_date_t *)date;
@end

@interface NXNetworkTime ()
- (BOOL)networkTimeShouldUpdate:(NXDate *)date;
@end

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS

static void _ntp_callback(net_ntp_t *ntp, const sys_date_t *date,
                          void *user_data) {
  (void)ntp;
  NXNetworkTime *sender = (NXNetworkTime *)user_data;
  sys_assert(date);
  sys_assert(sender);

  // TODO: Should push this into the application queue, rather than calling
  // directly.

  // Update the systems time from the network time
  NXDate *now = [NXDate dateWithSystemDate:date];
  if (now) {
    if ([sender networkTimeShouldUpdate:now] == YES) {
      if (sys_date_set_now(date) == false) {
        sys_printf("Failed to set system date");
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

@implementation NXNetworkTime

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)init {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Initialize the NTP handle
  _ntp = net_ntp_init(_ntp_callback, self);

  // Return success
  return self;
}

- (void)dealloc {
  @synchronized([self class]) {
    // Finalize the NTP handle
    net_ntp_finalize(_ntp);
    _ntp = NULL;

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
 * @brief Gets the current network time delegate.
 */
- (id<NetworkTimeDelegate>)delegate {
  @synchronized(self) {
    return _delegate;
  }
}

/**
 * @brief Sets the network time delegate.
 */
- (void)setDelegate:(id<NetworkTimeDelegate>)delegate {
  @synchronized(self) {
    if (_delegate == delegate) {
      return; // No change
    }
    _delegate = delegate;
  }
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Called when a network time update is received.
 */
- (BOOL)networkTimeShouldUpdate:(NXDate *)date {
  if (_delegate && object_respondsToSelector(_delegate, @selector
                                             (networkTimeShouldUpdate:))) {
    return [_delegate networkTimeShouldUpdate:date];
  }
  return YES;
}

@end
