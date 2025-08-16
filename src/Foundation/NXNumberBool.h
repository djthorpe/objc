/**
 * @file NXNumberBool.h
 * @brief Defines the NXNumberBool class, which represents boolean values.
 */
#pragma once
#include <Foundation/Foundation.h>

/**
 * @brief A class for managing and manipulating boolean values.
 */
@interface NXNumberBool : NXNumber {
@private
  BOOL _value; ///< The boolean value stored in this instance.
}

/**
 * @brief Initialize an instance with a boolean value.
 * @param value The boolean value to store.
 * @return An initialized NXNumberBool instance.
 */
- (id)initWithBool:(BOOL)value;

@end
