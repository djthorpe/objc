/**
 * @file NXDate.h
 * @brief Defines the NXDate class for date and time manipulation.
 *
 * This file provides the definition for the NXDate class, which offers
 * basic date and time manipulation capabilities.
 */

#pragma once
#include "NXTimeInterval.h"
#include <sys/sys.h>

/**
 * @brief A class for representing and manipulating dates.
 *
 * The NXDate class provides an interface for managing dates and times.
 * It represents a single point in time since a reference date.
 *
 * @note All time calculations are performed using NXTimeInterval, which
 * provides sub-second precision.
 *
 * \headerfile NXDate.h NXFoundation/NXFoundation.h
 */
@interface NXDate : NXObject {
@protected
  sys_time_t _time;
@private
  uint16_t _year;   ///< Cached year component
  uint8_t _month;   ///< Cached month component (1-12)
  uint8_t _day;     ///< Cached day component (1-31)
  uint8_t _weekday; ///< Cached weekday component (0-6, Sunday=0)
  uint8_t _hours;   ///< Cached hours component (0-23)
  uint8_t _minutes; ///< Cached minutes component (0-59)
  uint8_t _seconds; ///< Cached seconds component (0-59)
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Creates and returns a new date set to the current date and time.
 * @return A NXDate instance representing the current date and time.
 */
+ (NXDate *)date;

/**
 * @brief Creates and returns a date object set to a given time interval from
 * now.
 * @param interval The interval to add to the current date.
 *               Can be positive (future) or negative (past).
 * @return A NXDate instance representing the calculated date.
 */
+ (NXDate *)dateWithTimeIntervalSinceNow:(NXTimeInterval)interval;

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Retrieves the date components from this date.
 * @param year Pointer to store the year (e.g., 2025). Can be NULL if not
 * needed.
 * @param month Pointer to store the month (1-12). Can be NULL if not needed.
 * @param day Pointer to store the day of month (1-31). Can be NULL if not
 * needed.
 * @param weekday Pointer to store the day of week (0-6, Sunday=0). Can be NULL
 * if not needed.
 * @return YES if the operation was successful, NO otherwise.
 * @note All output parameters are optional and can be NULL if that component is
 * not needed.
 */
// - (BOOL)year:(uint16_t *)year
//        month:(uint8_t *)month
//          day:(uint8_t *)day
//      weekday:(uint8_t *)weekday;

/**
 * @brief Retrieves the time components from this date.
 * @param hours Pointer to store the hours (0-23). Can be NULL if not needed.
 * @param minutes Pointer to store the minutes (0-59). Can be NULL if not
 * needed.
 * @param seconds Pointer to store the seconds (0-59). Can be NULL if not
 * needed.
 * @param nanoseconds Pointer to store the nanoseconds (0-999999999). Can be
 * NULL if not needed.
 * @return YES if the operation was successful, NO otherwise.
 * @note All output parameters are optional and can be NULL if that component is
 * not needed.
 * @note Time is returned in UTC timezone.
 */
// - (BOOL)hours:(uint8_t *)hours
//         minutes:(uint8_t *)minutes
//         seconds:(uint8_t *)seconds
//     nanoseconds:(uint32_t *)nanoseconds;

/**
 * @brief Sets the date components for this date object.
 * @param year The year to set (e.g., 2025).
 * @param month The month to set (1-12).
 * @param day The day of month to set (1-31).
 * @param weekday The day of week to set (0-6, Sunday=0). This parameter is
 * ignored and calculated automatically based on the date.
 * @return YES if the date was set successfully, NO if the date is invalid.
 * @note The weekday parameter is provided for API consistency but is ignored.
 *       The actual weekday is calculated from the year, month, and day.
 * @note Invalid dates (e.g., February 30th) will cause this method to return
 * NO.
 * @warning This method modifies the internal time representation and may affect
 *          the time components (hours, minutes, seconds, nanoseconds).
 */
// - (BOOL)setYear:(uint16_t)year
//           month:(uint8_t)month
//             day:(uint8_t)day
//         weekday:(uint8_t)weekday;

/**
 * @brief Sets the time components for this date object.
 * @param hours The hours to set (0-23).
 * @param minutes The minutes to set (0-59).
 * @param seconds The seconds to set (0-59).
 * @param nanoseconds The nanoseconds to set (0-999999999).
 * @return YES if the time was set successfully, NO if any component is out of
 * range.
 * @note Time is set in UTC timezone.
 * @note This method preserves the date components (year, month, day) while
 *       only modifying the time portion.
 * @warning Invalid time values will cause this method to return NO without
 *          modifying the date object.
 */
// - (BOOL)setHours:(uint8_t)hours
//          minutes:(uint8_t)minutes
//          seconds:(uint8_t)seconds
//      nanoseconds:(uint32_t)nanoseconds;

@end
