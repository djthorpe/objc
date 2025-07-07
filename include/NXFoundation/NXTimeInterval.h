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
const NXTimeInterval Millisecond = 1;

/** @brief 1 second = 1,000 milliseconds */
const NXTimeInterval Second = 1000 * Millisecond;

/** @brief 1 minute = 60,000 milliseconds */
const NXTimeInterval Minute = 60 * Second;

/** @brief 1 hour = 3,600,000 milliseconds */
const NXTimeInterval Hour = 60 * Minute;