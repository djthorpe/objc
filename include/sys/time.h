#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Represents a system time structure.
 *
 * This structure is used to represent time in the system, including
 * the current UTC time with nanosecond precision. Timezones are not included
 * in this structure at the moment.
 */
typedef struct sys_time {
  int64_t seconds;     // Seconds since Unix epoch
  int32_t nanoseconds; // Nanoseconds (0-999999999)
} sys_time_t;

/**
 * @brief Gets the current UTC time.
 * @param time Pointer to a sys_time_t structure to fill with current time.
 * @return true on success, false on error.
 */
bool sys_time_get_utc(sys_time_t *time);

/**
 * @brief Extracts time components from a sys_time_t structure.
 * @param time Pointer to a sys_time_t structure to extract time from.
 * @param hours Pointer to store hours (0-23).
 * @param minutes Pointer to store minutes (0-59).
 * @param seconds Pointer to store seconds (0-59).
 * @param nanoseconds Pointer to store nanoseconds (0-999999999).
 * @return true on success, false on error.
 */
bool sys_time_get_time_utc(sys_time_t *time, uint8_t *hours, uint8_t *minutes,
                           uint8_t *seconds, uint32_t *nanoseconds);

/**
 * @brief Extracts date components from a sys_time_t structure.
 * @param time Pointer to a sys_time_t structure to extract date from.
 * @param year Pointer to store year (e.g., 2025).
 * @param month Pointer to store month (1-12).
 * @param day Pointer to store day (1-31).
 * @param weekday Pointer to store weekday (0-6, where 0=Sunday).
 * @return true on success, false on error.
 */
bool sys_time_get_date_utc(sys_time_t *time, uint16_t *year, uint8_t *month,
                           uint8_t *day, uint8_t *weekday);

/**
 * @brief Sets time components in a sys_time_t structure, preserving the date.
 * @param time Pointer to a sys_time_t structure to modify.
 * @param hours Hours to set (0-23).
 * @param minutes Minutes to set (0-59).
 * @param seconds Seconds to set (0-59).
 * @param nanoseconds Nanoseconds to set (0-999999999).
 * @return true on success, false on error or invalid parameters.
 */
bool sys_time_set_time_utc(sys_time_t *time, uint8_t hours, uint8_t minutes,
                           uint8_t seconds, uint32_t nanoseconds);

/**
 * @brief Sets date components in a sys_time_t structure, preserving the time.
 * @param time Pointer to a sys_time_t structure to modify.
 * @param year Year to set (e.g., 2025).
 * @param month Month to set (1-12).
 * @param day Day to set (1-31).
 * @return true on success, false on error or invalid parameters.
 */
bool sys_time_set_date_utc(sys_time_t *time, uint16_t year, uint8_t month,
                           uint8_t day);

/**
 * @brief Calculates the difference in nanoseconds between two sys_time_t
 * structures.
 * @param start Pointer to the start time. If `NULL`, this is treated as the
 * epoch (seconds since January 1, 1970).
 * @param end Pointer to the end time. If `NULL`, this is treated as the current
 * time.
 * @return The difference in nanoseconds. If end is earlier than start, returns
 * a negative value.
 */
int64_t sys_time_compare_ns(const sys_time_t *start, const sys_time_t *end);
