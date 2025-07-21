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
    int64_t s = interval / Second;
    int64_t ns = interval % Second;
    _time.seconds += s;
    _time.nanoseconds += ns;
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
// PRIVATE METHODS

- (BOOL)_cacheComponents {
  if (_year == 0) {
    if (!sys_time_get_date_utc(&_time, &_year, &_month, &_day, &_weekday)) {
      _year = 0;
      return NO;
    }
    if (!sys_time_get_time_utc(&_time, &_hours, &_minutes, &_seconds)) {
      _year = 0;
      return NO;
    }
  }
  return YES;
}

- (BOOL)_cacheTimeComponents {
  if (_year == 0) {
    if (!sys_time_get_date_utc(&_time, &_year, &_month, &_day, &_weekday)) {
      return NO;
    }
    if (!sys_time_get_time_utc(&_time, &_hours, &_minutes, &_seconds)) {
      _year = 0;
      return NO;
    }
  }
  return YES;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

- (NXString *)description {
  // Attempt to cache time components
  if ([self _cacheTimeComponents]) {
    // Successfully cached time components
    return [[[NXString alloc]
        initWithFormat:@"%04d-%02d-%02d %02d:%02d:%02d UTC", _year, _month,
                       _day, _hours, _minutes, _seconds] autorelease];
  } else {
    return [[[NXString alloc]
        initWithFormat:@"[NXDate seconds: %ld, nanoseconds: %d]", _time.seconds,
                       _time.nanoseconds] autorelease];
  }
}

@end
