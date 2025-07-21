/**
 * @file NXDate.h
 * @brief Defines the NXDate class for date and time manipulation.
 *
 * This file provides the definition for the NXDate class, which offers
 * basic date and time manipulation capabilities.
 */

#pragma once
#include "NXTimeInterval.h"

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
@interface NXDate : NXObject

/**
 * @brief Creates and returns a new date set to the current date and time.
 * @return A NXDate instance representing the current date and time.
 */
+ (id)date;

/**
 * @brief Creates and returns a date object set to a given time interval from now.
 * @param interval The interval to add to the current date.
 *               Can be positive (future) or negative (past).
 * @return A NXDate instance representing the calculated date.
 */
+ (id)dateWithTimeIntervalSinceNow:(NXTimeInterval)interval;

@end
