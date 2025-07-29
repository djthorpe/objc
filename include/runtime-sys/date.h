/**
 * @file date.h
 * @brief Date and Time management functions.
 * @defgroup SystemDate Date and Time
 * @ingroup System
 *
 * Methods for manipulating time and date.
 *
 * @example clock/main.c
 * This is a complete example showing how to read the system date and time,
 * format it, and print it to the console. It uses the
 * sys_date_get_now() to retrieve the current date and time.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Represents a system date and time structure.
 * @ingroup SystemDate
 *
 * This structure is used to represent time in the system, including
 * the current UTC time with nanosecond precision, and a timezone offset.
 */
typedef struct sys_date_t {
  int64_t seconds;     // Seconds since January 1, 1970 00:00:00 UTC
  int32_t nanoseconds; // Nanoseconds (0-999999999)
  int32_t tzoffset;    // Timezone offset in seconds from UTC
} sys_date_t;

/**
 * @brief Gets the current system time.
 * @ingroup SystemDate
 * @param date Pointer to a sys_date_t structure to fill with current date, time
 * and timezone offset.
 * @return true on success, false on error.
 */
bool sys_date_get_now(sys_date_t *date);

/**
 * @brief Sets the current system time.
 * @ingroup SystemDate
 * @param date Pointer to a sys_date_t structure that contains the new time and
 * timezone offset.
 * @return true on success, false on error.
 *
 * This function sets the system time by automatically converting the input
 * to UTC using the timezone offset (date->seconds - date->tzoffset).
 * Many platforms do not allow setting the system time directly.
 * This function may fail if the platform does not support setting the system
 * time or if insufficient privileges are available.
 */
bool sys_date_set_now(sys_date_t *date);

/**
 * @brief Extracts time components from a sys_date_t structure.
 * @ingroup SystemDate
 * @param date Pointer to a sys_date_t structure to extract time from.  If NULL,
 *             the time is extracted from the current system time.
 * @param hours Pointer to store hours (0-23), or NULL to skip.
 * @param minutes Pointer to store minutes (0-59), or NULL to skip.
 * @param seconds Pointer to store seconds (0-59), or NULL to skip.
 * @return true on success, false on error.
 *
 * This function extracts the time components from a sys_date_t structure,
 * in universal time (UTC). The timezone offset is not applied to the
 * extracted time.
 */
bool sys_date_get_time_utc(sys_date_t *date, uint8_t *hours, uint8_t *minutes,
                           uint8_t *seconds);

/**
 * @brief Extracts time components from a sys_date_t structure.
 * @ingroup SystemDate
 * @param date Pointer to a sys_date_t structure to extract time from. If NULL,
 *             the time is extracted from the current system time.
 * @param hours Pointer to store hours (0-23), or NULL to skip.
 * @param minutes Pointer to store minutes (0-59), or NULL to skip.
 * @param seconds Pointer to store seconds (0-59), or NULL to skip.
 * @return true on success, false on error.
 *
 * This function extracts the time components from a sys_date_t structure,
 * in local time. The timezone offset is applied to the
 * extracted time.
 */
bool sys_date_get_time_local(sys_date_t *date, uint8_t *hours, uint8_t *minutes,
                             uint8_t *seconds);

/**
 * @brief Extracts date components from a sys_date_t structure.
 * @ingroup SystemDate
 * @param date Pointer to a sys_date_t structure to extract date from.  If NULL,
 *             the date is extracted from the current system time.
 * @param year Pointer to store year (e.g., 2025), or NULL to skip.
 * @param month Pointer to store month (1-12), or NULL to skip.
 * @param day Pointer to store day (1-31), or NULL to skip.
 * @param weekday Pointer to store weekday (0-6, where 0=Sunday), or NULL to
 * skip.
 * @return true on success, false on error.
 *
 * This function extracts the year, month, day and weekday components from a
 * sys_date_t structure, in universal time (UTC). The timezone offset is not
 * applied to the extracted time.
 */
bool sys_date_get_date_utc(sys_date_t *date, uint16_t *year, uint8_t *month,
                           uint8_t *day, uint8_t *weekday);

/**
 * @brief Extracts date components from a sys_date_t structure.
 * @ingroup SystemDate
 * @param date Pointer to a sys_date_t structure to extract date from.  If NULL,
 *             the date is extracted from the current system time.
 * @param year Pointer to store year (e.g., 2025), or NULL to skip.
 * @param month Pointer to store month (1-12), or NULL to skip.
 * @param day Pointer to store day (1-31), or NULL to skip.
 * @param weekday Pointer to store weekday (0-6, where 0=Sunday), or NULL to
 * skip.
 * @return true on success, false on error.
 *
 * This function extracts the year, month, day and weekday components from a
 * sys_date_t structure in local time. The timezone offset is applied to the
 * extracted date.
 */
bool sys_date_get_date_local(sys_date_t *date, uint16_t *year, uint8_t *month,
                             uint8_t *day, uint8_t *weekday);

/**
 * @brief Sets time components in a sys_date_t structure, preserving the date
 * and timezone offset.
 * @ingroup SystemDate
 * @param date Pointer to a sys_date_t structure to modify.
 * @param hours Hours to set (0-23).
 * @param minutes Minutes to set (0-59).
 * @param seconds Seconds to set (0-59).
 * @return true on success, false on invalid parameters.
 *
 * This function updates the sys_date_t value by setting the time components
 * in UTC. The date and timezone offset remain unchanged. Time components
 * are interpreted as UTC time regardless of the timezone offset.
 */
bool sys_date_set_time_utc(sys_date_t *date, uint8_t hours, uint8_t minutes,
                           uint8_t seconds);

/**
 * @brief Sets date components in a sys_date_t structure, preserving the time
 * and timezone offset.
 * @ingroup SystemDate
 * @param date Pointer to a sys_date_t structure to modify.
 * @param year Year to set (1900 or later, supports historical dates).
 * @param month Month to set (1-12).
 * @param day Day to set (1-31).
 * @return true on success, false on invalid parameters.
 *
 * This function updates the sys_date_t value by setting the date components
 * in UTC. The time and timezone offset remain unchanged. Date components
 * are interpreted as UTC date regardless of the timezone offset.
 */
bool sys_date_set_date_utc(sys_date_t *date, uint16_t year, uint8_t month,
                           uint8_t day);

/**
 * @brief Calculate the difference in nanoseconds between two sys_date_t
 * structures.
 * @ingroup SystemDate
 * @param start Pointer to the start time. If `NULL`, use the
 * current system time.
 * @param end Pointer to the end time. Cannot be `NULL`.
 * @return The difference in nanoseconds. If end is earlier than start, returns
 * a negative value.
 */
int64_t sys_date_compare_ns(const sys_date_t *start, const sys_date_t *end);

#ifdef __cplusplus
}
#endif
