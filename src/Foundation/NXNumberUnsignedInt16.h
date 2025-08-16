/**
 * @file NXNumberUnsignedInt16.h
 * @brief Defines the NXNumberUnsignedInt16 class, which represents 16-bit
 * unsigned integers values.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief NXNumber subclass for storing 16-bit unsigned integer values.
 */
@interface NXNumberUnsignedInt16 : NXNumber {
  uint16_t _value; ///< The stored 16-bit unsigned integer value
}

/**
 * @brief Initialize an instance with a uint16_t value.
 */
- (id)initWithUnsignedInt16:(uint16_t)value;

@end
