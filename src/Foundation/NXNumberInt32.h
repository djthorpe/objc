/**
 * @file NXNumberInt32.h
 * @brief Defines the NXNumberInt32 class, which represents 32-bit integers
 * values.
 */
#pragma once
#include <Foundation/Foundation.h>

/**
 * @brief A class for managing and manipulating 32-bit integer values.
 */
@interface NXNumberInt32 : NXNumber {
@private
  int32_t _value; ///< The 32-bit integer value stored in this instance.
}

+ (NXNumber *)numberWithInt32:(int32_t)value;

@end
