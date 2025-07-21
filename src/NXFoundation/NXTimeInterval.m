#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/** @brief Base unit: 1 nanosecond */
const NXTimeInterval Nanosecond = 1;

/** @brief 1 millisecond = 1,000,000 nanoseconds */
const NXTimeInterval Millisecond = 1000000 * Nanosecond;

/** @brief 1 second = 1000 milliseconds */
const NXTimeInterval Second = 1000 * Millisecond;

/** @brief 1 minute = 60 econds */
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
 * @param interval The time interval to format
 * @param truncate The minimum unit to display (e.g., Second to ignore
 * milliseconds)
 * @return A formatted string like "1d 2h 30m 45s 500ms" or "0s" for zero/small
 * intervals
 */
NXString *NXTimeIntervalDescription(NXTimeInterval interval,
                                    NXTimeInterval truncate) {
  // TODO: Implement after NXString concatenation methods are available
  // Should format time intervals like "1d 2h 30m 45s 500ms"
  // with truncation support for precision control
  (void)interval; // Avoid unused parameter warning
  (void)truncate; // Avoid unused parameter warning
  return [[[NXString alloc] initWithCString:"TODO"] autorelease];
}
