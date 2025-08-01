/**
 * @file assert.h
 * @brief Defines a custom assertion macro.
 * @details This file provides an `sys_assert` macro that can be used for
 * debugging purposes.
 */

#pragma once
#include <runtime-sys/sys.h>

/**
 * @def sys_assert(condition)
 * @ingroup System
 * @brief Asserts that a condition is true.
 * @param condition The condition to check.
 * @details If the condition is false, it will call `panicf` with an assertion
 * failure message, including the condition, file name, and line number.
 */
#ifdef DEBUG
#define sys_assert(condition)                                                  \
  if (!(condition)) {                                                          \
    sys_panicf("ASSERT FAIL: %s, file %s, line %d", #condition, __FILE__,      \
               __LINE__);                                                      \
  }
#else
#define sys_assert(condition)                                                  \
  if (!(condition)) {                                                          \
    sys_panicf("ASSERT FAIL: %s", #condition);                                 \
  }
#endif
