/**
 * @file NXNumberUnsignedInt32.h
 * @brief Defines the NXNumberUnsignedInt32 class, which represents 32-bit
 * unsigned integer values.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief A class for managing and manipulating unsigned 32-bit integer values.
 */
@interface NXNumberUnsignedInt32 : NXNumber {
@private
  uint32_t
      _value; ///< The 32-bit unsigned integer value stored in this instance.
}

+ (NXNumber *)numberWithUnsignedInt32:(uint32_t)value;

@end
