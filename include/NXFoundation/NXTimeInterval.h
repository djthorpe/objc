/**
 * @file NXTimeInterval.h
 * @brief Time interval type and constants for duration measurements.
 * 
 * @details This header defines NXTimeInterval, a 32-bit signed integer type for
 *          representing time durations in milliseconds. It provides predefined
 *          constants for common time units to simplify time calculations.
 * 
 * @example
 * @code
 * // Sleep for 5 seconds
 * NXTimeInterval duration = 5 * Second;
 * objc_sleep(duration);
 * 
 * // Calculate timeout in minutes
 * NXTimeInterval timeout = 30 * Minute;
 * @endcode
 * 
 * @note The maximum representable duration is approximately 596 hours (24.8 days).
 */

#pragma once
#include <stdint.h>

/**
 * @brief Time interval type representing milliseconds as a 32-bit signed integer.
 * 
 * NXTimeInterval stores time durations in milliseconds using a signed
 * 32-bit integer. This provides sufficient precision for most timing
 * operations while maintaining compatibility with system APIs.
 * 
 * @par Range and Limitations:
 * - Minimum value: -2,147,483,648 milliseconds (~-596 hours)
 * - Maximum value: +2,147,483,647 milliseconds (~+596 hours)
 * - Resolution: 1 millisecond
 *
 * @warning Values exceeding the 32-bit range will overflow. Always validate
 *          duration calculations in applications requiring extended time ranges.
 */
typedef int32_t NXTimeInterval;

/**
 * @brief Base time unit representing one millisecond.
 */
extern const NXTimeInterval Millisecond;

/**
 * @brief Time constant representing one second.
 */
extern const NXTimeInterval Second;

/**
 * @brief Time constant representing one minute.
 */
extern const NXTimeInterval Minute;

/**
 * @brief Time constant representing one hour.
 */
extern const NXTimeInterval Hour;
