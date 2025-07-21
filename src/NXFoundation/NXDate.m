#include <NXFoundation/NXFoundation.h>
#include <stdio.h>
#include <sys/sys.h>

@implementation NXDate

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)init {
  self = [super init];
  if (self) {
    bool success = sys_time_get_utc(&_time);
    if (!success) {
      [self release];
      self = nil;
    }
  }
  return self;
}

- (id)initWithTimeIntervalSinceNow:(NXTimeInterval)interval {
  self = [self init];
  if (self) {
    // Convert milliseconds to seconds and nanoseconds
    int64_t intervalInSeconds = interval / MILLISECONDS_PER_SECOND;
    int32_t remainingMilliseconds = interval % MILLISECONDS_PER_SECOND;
    int32_t additionalNanoseconds = remainingMilliseconds * 1000000;

    _time.seconds += intervalInSeconds;
    _time.nanoseconds += additionalNanoseconds;

    // Handle nanosecond overflow
    if (_time.nanoseconds >= 1000000000) {
      _time.seconds += 1;
      _time.nanoseconds -= 1000000000;
    } else if (_time.nanoseconds < 0) {
      _time.seconds -= 1;
      _time.nanoseconds += 1000000000;
    }
  }
  return self;
}

+ (NXDate *)date {
  return [[[NXDate alloc] init] autorelease];
}

+ (NXDate *)dateWithTimeIntervalSinceNow:(NXTimeInterval)interval {
  return [[[NXDate alloc] initWithTimeIntervalSinceNow:interval] autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

- (NXString *)description {
  // TODO
  return NULL; // Placeholder for now
}

@end
