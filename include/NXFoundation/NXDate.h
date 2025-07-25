/**
 * @file NXDate.h
 * @brief Defines the NXDate class for date and time manipulation.
 *
 * This file provides the definition for the NXDate class, which offers
 * basic date and time manipulation capabilities.
 */

#pragma once
#include "NXTimeInterval.h"
#include <runtime-sys/sys.h>

/**
 * @brief A class for representing and manipulating dates.
 *
 * The NXDate class provides an interface for managing dates and times.
 * It represents a single point in time since a reference date.
 *
 * \headerfile NXDate.h NXFoundation/NXFoundation.h
 */
@interface NXDate : NXObject <JSONProtocol> {
@protected
  sys_time_t _time; ///< Time representation
@private
  uint16_t _year;   ///< Cached year component (1-9999)
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
 */
- (BOOL)year:(uint16_t *)year
       month:(uint8_t *)month
         day:(uint8_t *)day
     weekday:(uint8_t *)weekday;

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
 * @note Time is returned in UTC timezone.
 */
- (BOOL)hours:(uint8_t *)hours
        minutes:(uint8_t *)minutes
        seconds:(uint8_t *)seconds
    nanoseconds:(uint32_t *)nanoseconds;

/**
 * @brief Sets the date components for this date object.
 * @param year The year to set (e.g., 2025).
 * @param month The month to set (1-12).
 * @param day The day of month to set (1-31).
 * @return YES if the date was set successfully, NO if the date is invalid.
 * @note Invalid dates (e.g., February 30th) will cause this method to return
 * NO.
 */
- (BOOL)setYear:(uint16_t)year month:(uint8_t)month day:(uint8_t)day;

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
 */
- (BOOL)setHours:(uint8_t)hours
         minutes:(uint8_t)minutes
         seconds:(uint8_t)seconds
     nanoseconds:(uint32_t)nanoseconds;

/**
 * @brief Compares the date against another date, and returns the difference.
 * @param otherDate The date to compare against.
 * @return The time interval since the reference date. If the other date is
 * later, the interval will be negative.
 */
- (NXTimeInterval)compare:(NXDate *)otherDate;

/**
 * @brief Determine if the date is earlier than another date
 * @param otherDate The date to compare against.
 * @return YES if this date is earlier than otherDate, NO otherwise.
 */
- (BOOL)isEarlierThan:(NXDate *)otherDate;

/**
 * @brief Determine if the date is later than another date
 * @param otherDate The date to compare against.
 * @return YES if this date is later than otherDate, NO otherwise.
 */
- (BOOL)isLaterThan:(NXDate *)otherDate;

/**
 * @brief Add a time interval to this date.
 * @param interval The time interval to add.
 */
- (void)addTimeInterval:(NXTimeInterval)interval;

/**
 * @brief Return a new date by adding a time interval to this date.
 * @param interval The time interval to add.
 * @return A new NXDate instance representing the date after adding the
 * interval.
 */
- (NXDate *)dateByAddingTimeInterval:(NXTimeInterval)interval;

@end
