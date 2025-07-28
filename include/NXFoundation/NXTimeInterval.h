/**
 * @file NXTimeInterval.h
 * @brief Time interval type and constants for duration measurements.
 *
 * This header defines NXTimeInterval, a 64-bit signed integer type for
 * representing time durations in nanoseconds. It provides predefined
 * constants for common time units to simplify time calculations.
 */

#pragma once
#include <stdint.h>

/**
 * @brief Time interval type representing nanoseconds as a 64-bit signed
 * integer.
 * @ingroup Foundation
 *
 * NXTimeInterval stores time durations in nanoseconds using a signed
 * 64-bit integer. This provides sufficient precision for most timing
 * operations while maintaining compatibility with system APIs.
 *
 * @par Range and Limitations:
 * - Minimum value: -9,223,372,036,854,775,808 nanoseconds (~-292 billion years)
 * - Maximum value: +9,223,372,036,854,775,807 nanoseconds (~+292 billion years)
 * - Resolution: 1 nanosecond
 *
 * @warning Values exceeding the 64-bit range will overflow. Always validate
 *          duration calculations in applications requiring extended time
 * ranges.
 *
 * \headerfile NXTimeInterval.h NXFoundation/NXFoundation.h
 */
typedef int64_t NXTimeInterval;

/** @brief Base unit: 1 nanosecond */
extern const NXTimeInterval Nanosecond;

/** @brief 1 millisecond = 1,000,000 nanoseconds */
extern const NXTimeInterval Millisecond;

/** @brief 1 second = 1000 milliseconds */
extern const NXTimeInterval Second;

/** @brief 1 minute = 60 seconds */
extern const NXTimeInterval Minute;

/** @brief 1 hour = 60 minutes */
extern const NXTimeInterval Hour;

/** @brief 1 day = 24 hours */
extern const NXTimeInterval Day;

/**
 * @brief Converts a time interval to milliseconds.
 *
 * This function converts a NXTimeInterval value to its milliseconds
 * representation, discarding any fractional nanoseconds.
 *
 * @param interval The time interval to convert.
 * @return The milliseconds equivalent of the given time interval.
 */
extern int32_t NXTimeIntervalMilliseconds(NXTimeInterval interval);

/**
 * @brief Converts a time interval to a string representation.
 *
 * This function converts a NXTimeInterval value to its string
 * representation, including the appropriate time unit.
 *
 * @param interval The time interval to convert.
 * @param truncate Truncate the interval based on the precision required.
 * @return The string representation of the given time interval.
 */
extern NXString *NXTimeIntervalDescription(NXTimeInterval interval,
                                           NXTimeInterval truncate);
