/**
 * @file NXNumberUnsignedInt64.h
 * @brief Defines the NXNumberUnsignedInt64 class, which represents 64-bit
 * unsigned integer values.
 */
#pragma once
#include <Foundation/Foundation.h>

/**
 * @brief A class for managing and manipulating unsigned 64-bit integer values.
 */
@interface NXNumberUnsignedInt64 : NXNumber {
@private
  uint64_t
      _value; ///< The 64-bit unsigned integer value stored in this instance.
}

+ (NXNumber *)numberWithUnsignedInt64:(uint64_t)value;

@end
