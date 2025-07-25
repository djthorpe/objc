/**
 * @file assert.h
 * @brief Defines a custom assertion macro.
 * @details This file provides an `assert` macro that can be used for debugging
 * purposes. The assertion is only active when the `DEBUG` preprocessor macro is
 * defined.
 */

#pragma once
#include <runtime-sys/sys.h>

/**
 * @def objc_assert(condition)
 * @ingroup objc   
 * @brief Asserts that a condition is true.
 * @param condition The condition to check.
 * @details If the `DEBUG` macro is defined, this macro will check the given
 * condition. If the condition is false, it will call `panicf` with an assertion
 * failure message, including the condition, file name, and line number. If
 * `DEBUG` is not defined, this macro does nothing.
 */
#ifdef DEBUG
#define objc_assert(condition)                                                 \
  if (!(condition)) {                                                          \
    sys_panicf("ASSERT FAIL: %s, file %s, line %d", #condition, __FILE__,      \
               __LINE__);                                                      \
  }
#else
#define objc_assert(condition)                                                 \
  if (!(condition)) {                                                          \
    sys_panicf("ASSERT FAIL: %s", #condition);                                 \
  }
#endif
