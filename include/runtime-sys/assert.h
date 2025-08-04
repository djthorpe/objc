/**
 * @file runtime-sys/assert.h
 * @brief Defines a custom assertion macro.
 * @details This file provides an `sys_assert` macro that can be used for
 * debugging purposes.
 */

#pragma once

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Aborts the current process immediately.
 * @ingroup SystemThread
 * @note This function does not return to the caller.
 *
 *  This function terminates the current process abnormally and
 *  immediately. It does not perform any cleanup operations and
 *  does not call exit handlers or destructors.
 */
extern void sys_abort(void);

/**
 * @brief Prints a formatted panic message and aborts the process.
 * @ingroup SystemThread
 * @param fmt A printf-style format string specifying the panic message.
 * @param ... Additional arguments corresponding to format specifiers in fmt.
 * @note This function does not return to the caller.
 */
extern void sys_panicf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
