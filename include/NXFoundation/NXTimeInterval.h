#pragma once
#include <stdint.h>

/**
 * @brief Time interval type representing milliseconds as a 32-bit signed integer.
 * 
 * NXTimeInterval stores time durations in milliseconds. The maximum value that can
 * be stored is 2,147,483,647 milliseconds, which equals approximately 596 hours
 * or about 24.8 days.
 */
typedef int32_t NXTimeInterval;

/** @brief Base unit: 1 millisecond */
extern const NXTimeInterval Millisecond;

/** @brief 1 second = 1,000 milliseconds */
extern const NXTimeInterval Second;

/** @brief 1 minute = 60,000 milliseconds */
extern const NXTimeInterval Minute;

/** @brief 1 hour = 3,600,000 milliseconds */
extern const NXTimeInterval Hour;
