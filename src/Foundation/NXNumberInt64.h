/**
 * @file NXNumberInt64.h
 * @brief Defines the NXNumberInt64 class, which represents 64-bit integers
 * values.
 */
#pragma once
#include <NXFoundation/NXFoundation.h>

/**
 * @brief A class for managing and manipulating 64-bit integer values.
 */
@interface NXNumberInt64 : NXNumber {
@private
  int64_t _value; ///< The 64-bit integer value stored in this instance.
}

+ (NXNumber *)numberWithInt64:(int64_t)value;

@end
