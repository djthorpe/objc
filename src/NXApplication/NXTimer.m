#include <NXApplication/NXApplication.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// CALLBACK

static void _timer_callback(sys_timer_t *timer) {
  objc_assert(timer);
  if (sys_timer_valid(timer)) {
    // TODO: Put an event into the event queue with the
    // delegate and userdata
    sys_printf("Timer fired with userdata: %p\n", timer->userdata);
  }

  // If the timer is not repeating, invalidate it
  NXTimer *aTimer = (NXTimer *)timer->userdata;
  objc_assert(aTimer);
  if (aTimer && ![aTimer repeats]) {
    [aTimer invalidate];
  }
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXTimer

/**
 * @brief Initializes an invalid timer.
 */
- (id)init {
  self = [super init];
  if (self == nil) {
    return nil;
  }

  // Initialize the timer with default values
  _delegate = nil;
  _repeats = NO;
  _interval = 0;

  // Return success
  return self;
}

/**
 * @brief Initializes a timer with the specified time interval and repeat
 * behaviour.
 */
- (id)initWithInterval:(NXTimeInterval)interval repeats:(BOOL)repeats {
  self = [self init];
  if (self == nil) {
    return nil;
  }

  // Initialize the timer with the specified interval and repeat flag
  uint32_t ms = NXTimeIntervalMilliseconds(interval);
  sys_printf("initialize timer with interval: %u ms\n", (unsigned int)ms);
  _timer = sys_timer_init(ms, self, _timer_callback);
  if (!sys_timer_valid(&_timer)) {
    sys_printf("ERR initialize timer with interval: %u ms\n", (unsigned int)ms);
    [self release];
    return nil;
  } else {
    _repeats = repeats;
    _interval = interval;
  }

  // Return success
  return self;
}

/**
 * @brief Creates a new timer with the specified time interval and repeat
 * behaviour.
 */
+ (NXTimer *)timerWithInterval:(NXTimeInterval)interval repeats:(BOOL)repeats {
  return [[[self alloc] initWithInterval:interval repeats:repeats] autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Gets the current timer delegate.
 */
- (id<TimerDelegate>)delegate {
  return _delegate;
}

/**
 * @brief Sets the application delegate.
 */
- (void)setDelegate:(id<TimerDelegate>)delegate {
  @synchronized(self) {
    if (_delegate == delegate) {
      return; // No change
    }
    if (_delegate == nil && sys_timer_valid(&_timer)) {
      // Start the timer if it was not running
      sys_timer_start(&_timer);
    }
    // Set the new delegate
    _delegate = delegate;
  }
}

/**
 * @brief Checks if the timer is still valid and active.
 */
- (BOOL)valid {
  @synchronized(self) {
    if (sys_timer_valid(&_timer) == false) {
      return NO; // Timer is not valid
    }
    if (_delegate == nil) {
      return NO; // No delegate set
    }
    return YES; // Timer is valid and has a delegate
  }
}

/**
 * @brief Gets the timer's configured time interval.
 */
- (NXTimeInterval)timeInterval {
  return _interval;
}

/**
 * @brief Returns whether the timer is set to repeat.
 */
- (BOOL)repeats {
  return _repeats;
}

///////////////////////////////////////////////////////////////////////////////
// INSTANCE METHODS

/**
 * @brief Manually fires the timer.
 * @return YES if the timer was successfully fired, NO otherwise.
 *
 * This method immediately triggers the timer's firing behavior, calling the
 * delegate's timer callback method. For repeating timers, this does not
 * affect the regular firing schedule.
 */
- (BOOL)fire {
  _timer_callback(&_timer);
  return YES;
}

/**
 * @brief Invalidates and stops the timer.
 *
 * Once a timer is invalidated, it cannot be reused and will no longer fire.
 * This method should be called to clean up timers that are no longer needed
 * to prevent memory leaks and unwanted timer callbacks.
 */
- (void)invalidate {
  @synchronized(self) {
    if (sys_timer_valid(&_timer)) {
      sys_timer_finalize(&_timer);
    }
    _delegate = nil; // Clear the delegate
  }
}

@end
