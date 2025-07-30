#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

@implementation NXDate

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialise an instance representing the current time.
 */
- (id)init {
  self = [super init];
  if (self) {
    bool success = sys_date_get_now(&_date);
    if (!success) {
      [self release];
      self = nil;
    } else {
      _year = 0; // Indicates components not cached yet
    }
  }
  return self;
}

/**
 * @brief Initialise an instance representing an offset from the current time.
 */
- (id)initWithTimeIntervalSinceNow:(NXTimeInterval)interval {
  self = [self init];
  if (self) {
    int64_t s = interval / Second;
    int64_t ns = interval % Second;
    _date.seconds += s;
    _date.nanoseconds += ns;
    if (_date.nanoseconds >= 1000000000) {
      int64_t overflow = _date.nanoseconds / 1000000000;
      _date.seconds += overflow;
      _date.nanoseconds -= overflow * 1000000000;
    }
  }
  return self;
}

/**
 * @brief Return an instance representing the current time.
 */
+ (NXDate *)date {
  return [[[NXDate alloc] init] autorelease];
}

/**
 * @brief Return an instance representing an offset from the current time.
 */
+ (NXDate *)dateWithTimeIntervalSinceNow:(NXTimeInterval)interval {
  return [[[NXDate alloc] initWithTimeIntervalSinceNow:interval] autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Cache the date components from the underlying time.
 *
 * We use the _year to indicate whether components have been cached.
 */
- (BOOL)_cacheComponents {
  if (_year == 0) {
    if (!sys_date_get_date_utc(&_date, &_year, &_month, &_day, &_weekday)) {
      _year = 0;
      return NO;
    }
    if (!sys_date_get_time_utc(&_date, &_hours, &_minutes, &_seconds)) {
      _year = 0;
      return NO;
    }
  }
  return YES;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Return the date as a string representation.
 */
- (NXString *)description {
  BOOL cacheSuccess = [self _cacheComponents];
  objc_assert(cacheSuccess); // Ensure components are cached
  return [[[NXString alloc] initWithFormat:@"%04d-%02d-%02dT%02d:%02d:%02dZ",
                                           _year, _month, _day, _hours,
                                           _minutes, _seconds] autorelease];
}

/**
 * @brief Return the date as a JSON string representation.
 */
- (NXString *)JSONString {
  BOOL cacheSuccess = [self _cacheComponents];
  objc_assert(cacheSuccess); // Ensure components are cached
  return
      [NXString stringWithFormat:@"\"%04d-%02d-%02dT%02d:%02d:%02dZ\"", _year,
                                 _month, _day, _hours, _minutes, _seconds];
}

/**
 * @brief Returns the appropriate JSON length for the instance.
 */
- (size_t)JSONBytes {
  // Format: "YYYY-MM-DDTHH:MM:SSZ"
  return 22; // 22 characters
}

/**
 * @brief Retrieves the date components from this date.
 */
- (BOOL)year:(uint16_t *)year
       month:(uint8_t *)month
         day:(uint8_t *)day
     weekday:(uint8_t *)weekday {
  if ([self _cacheComponents]) {
    if (year)
      *year = _year;
    if (month)
      *month = _month;
    if (day)
      *day = _day;
    if (weekday)
      *weekday = _weekday;
    return YES; // Components successfully cached
  }
  return NO; // Failed to cache components
}

/**
 * @brief Retrieves the time components from this date.
 */
- (BOOL)hours:(uint8_t *)hours
        minutes:(uint8_t *)minutes
        seconds:(uint8_t *)seconds
    nanoseconds:(uint32_t *)nanoseconds {
  if ([self _cacheComponents]) {
    if (hours)
      *hours = _hours;
    if (minutes)
      *minutes = _minutes;
    if (seconds)
      *seconds = _seconds;
    if (nanoseconds)
      *nanoseconds = _date.nanoseconds;
    return YES; // Components successfully cached
  }
  return NO; // Failed to cache components
}

/**
 * @brief Sets the date components for this date object.
 */
- (BOOL)setYear:(uint16_t)year month:(uint8_t)month day:(uint8_t)day {
  // Check parameters
  if (year == 0 || month < 1 || month > 12 || day < 1 || day > 31) {
    return NO; // Invalid date
  }

  // Set the date components in the underlying time structure
  if (!sys_date_set_date_utc(&_date, year, month, day)) {
    return NO; // Failed to set date
  }

  // Mark components as uncached
  _year = 0;

  // Return success
  return YES;
}

/**
 * @brief Sets the time components for this date object.
 */
- (BOOL)setHours:(uint8_t)hours
         minutes:(uint8_t)minutes
         seconds:(uint8_t)seconds
     nanoseconds:(uint32_t)nanoseconds {
  // Check parameters
  if (hours > 23 || minutes > 59 || seconds > 59 || nanoseconds >= Second) {
    return NO; // Invalid time
  }

  // Set the time components in the underlying time structure
  if (!sys_date_set_time_utc(&_date, hours, minutes, seconds)) {
    return NO; // Failed to set time
  }

  // Set nanoseconds directly
  _date.nanoseconds = nanoseconds;

  // Mark components as uncached
  _year = 0;

  // Return success
  return YES;
}

/**
 * @brief Compares the date against another date, and returns the difference.
 */
- (NXTimeInterval)compare:(NXDate *)otherDate {
  objc_assert(otherDate != nil);
  return sys_date_compare_ns(&otherDate->_date, &_date);
}

/**
 * @brief Determine if the date is earlier than another date
 */
- (BOOL)isEarlierThan:(NXDate *)otherDate {
  objc_assert(otherDate != nil);
  return sys_date_compare_ns(&otherDate->_date, &_date) < 0;
}

/**
 * @brief Determine if the date is later than another date
 */
- (BOOL)isLaterThan:(NXDate *)otherDate {
  objc_assert(otherDate != nil);
  return sys_date_compare_ns(&otherDate->_date, &_date) > 0;
}

/**
 * @brief Determine if the date is equal to another date
 */
- (BOOL)isEqual:(id)other {
  if (other == self) {
    return YES; // Same instance
  }
  if (other == nil || ![other isKindOfClass:[NXDate class]]) {
    return NO; // Not equal if other is nil or not a date
  }
  return sys_date_compare_ns(&((NXDate *)other)->_date, &_date) == 0;
}

/**
 * @brief Add a time interval to this date.
 */
- (void)addTimeInterval:(NXTimeInterval)interval {
  // Compute seconds and nanoseconds from the interval
  int64_t s = interval / Second;
  int64_t ns = interval % Second;

  // Update the underlying time structure
  _date.seconds += s;
  _date.nanoseconds += ns;

  // Handle nanosecond overflow
  if (_date.nanoseconds >= Second) {
    int64_t overflow = _date.nanoseconds / Second;
    _date.seconds += overflow;
    _date.nanoseconds -= overflow * Second;
  } else if (_date.nanoseconds < ns) { // Detect underflow
    int64_t underflow = (ns - _date.nanoseconds + (Second - 1)) / Second;
    _date.seconds -= underflow;
    _date.nanoseconds += underflow * Second;
  }

  // Mark components as uncached
  _year = 0;
}

/**
 * @brief Return a new date by adding a time interval to this date.
 * @param interval The time interval to add.
 * @return A new NXDate instance representing the date after adding the
 * interval.
 */
- (NXDate *)dateByAddingTimeInterval:(NXTimeInterval)interval {
  NXDate *newDate = [[NXDate alloc] init];
  if (newDate) {
    newDate->_date = _date;             // Copy the current date and time
    [newDate addTimeInterval:interval]; // Add the interval
  }
  return [newDate autorelease];
}

@end
