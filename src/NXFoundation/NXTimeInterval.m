#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/** @brief Base unit: 1 nanosecond */
const NXTimeInterval Nanosecond = 1;

/** @brief 1 millisecond = 1,000,000 nanoseconds */
const NXTimeInterval Millisecond = 1000000 * Nanosecond;

/** @brief 1 second = 1000 milliseconds */
const NXTimeInterval Second = 1000 * Millisecond;

/** @brief 1 minute = 60 seconds */
const NXTimeInterval Minute = 60 * Second;

/** @brief 1 hour = 60 minutes */
const NXTimeInterval Hour = 60 * Minute;

/** @brief 1 day = 24 hours */
const NXTimeInterval Day = 24 * Hour;

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Converts a time interval to milliseconds.
 */
int32_t NXTimeIntervalMilliseconds(NXTimeInterval interval) {
  return (int32_t)(interval / Millisecond);
}

/**
 * @brief Converts a time interval to a string representation.
 */
NXString *NXTimeIntervalDescription(NXTimeInterval interval,
                                    NXTimeInterval truncate) {

  // Handle negative intervals by taking absolute value and adding minus sign
  BOOL isNegative = interval < 0;
  if (isNegative) {
    interval = -interval;
  }

  // Truncate the interval to the specified precision
  interval = interval - (interval % truncate);
  if (interval == 0) {
    return [NXString stringWithCString:"0s"];
  }

  // Create a string with capacity for the maximum possible length
  NXString *desc = [NXString stringWithCapacity:64];

  // Add minus sign for negative intervals
  if (isNegative) {
    [desc appendCString:"-"];
  }

  // Calculate each time component
  int64_t days = interval / Day;
  interval -= days * Day;
  if (days > 0) {
    [desc appendStringWithFormat:@"%ld day%s ", days, (days == 1) ? "" : "s"];
  }

  int64_t hours = interval / Hour;
  interval -= hours * Hour;
  if (hours > 0) {
    [desc appendStringWithFormat:@"%ldh ", hours];
  }

  int64_t minutes = interval / Minute;
  interval -= minutes * Minute;
  if (minutes > 0) {
    [desc appendStringWithFormat:@"%ldm ", minutes];
  }

  int64_t seconds = interval / Second;
  interval -= seconds * Second;
  if (seconds > 0) {
    [desc appendStringWithFormat:@"%lds ", seconds];
  }

  int64_t milliseconds = interval / Millisecond;
  interval -= milliseconds * Millisecond;
  if (milliseconds > 0) {
    [desc appendStringWithFormat:@"%ldms ", milliseconds];
  }

  // Return the formatted string, trimming any trailing spaces
  [desc trimWhitespace];
  return desc;
}
