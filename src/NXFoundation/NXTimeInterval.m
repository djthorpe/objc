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
 */
NXString *NXTimeIntervalDescription(NXTimeInterval interval,
                                    NXTimeInterval truncate) {
  // TODO
}
