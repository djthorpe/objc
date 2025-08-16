/**
 * @file NXNumberInt16.h
 * @brief Defines the NXNumberInt16 class, which represents 16-bit integers
 * values.
 */
#pragma once

#include <NXFoundation/NXFoundation.h>

/**
 * @brief NXNumber subclass for storing 16-bit signed integer values.
 */
@interface NXNumberInt16 : NXNumber {
  int16_t _value; ///< The stored 16-bit signed integer value
}

/**
 * @brief Initialize an instance with a int16_t value.
 */
- (id)initWithInt16:(int16_t)value;

/**
 * @brief Return an instance with a int16_t value.
 */
+ (NXNumber *)numberWithInt16:(int16_t)value;

@end
