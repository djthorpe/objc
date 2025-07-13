#include <objc/objc.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/sys.h>

#ifndef MIN_
#define MIN_(a, b) ((a) < (b) ? (a) : (b))
#endif

enum _objc_printf_flags {
  OBJC_PRINTF_NONE = 0,
  OBJC_PRINTF_LONG = 1 << 0,
  OBJC_PRINTF_HEX = 1 << 1,
  OBJC_PRINTF_BIN = 1 << 2,
  OBJC_PRINTF_UPPER = 1 << 3
};

static const char *_objc_printf_nil = "<nil>";

#define OBJC_PRINTF_BUF 256
static char _objc_printf_buf[OBJC_PRINTF_BUF];

/**
 * @brief Append a character into the buffer
 */
static inline size_t _objc_printf_chtostr(char *buf, size_t sz, size_t i,
                                          const char ch) {
  assert(buf == NULL || sz > 0);
  if (buf && i < (sz - 1)) {
    buf[i] = ch;
  }
  return i + 1;
}

/**
 * @brief Append a null-terminated string into the buffer
 */
static inline size_t _objc_printf_strtostr(char *buf, size_t sz, size_t i,
                                           const char *str) {
  assert(buf == NULL || sz > 0);
  if (str == NULL) {
    str = _objc_printf_nil;
  }
  while (*str) {
    if (buf && i < (sz - 1)) {
      buf[i] = *str;
    }
    str++;
    i++;
  }

  // Return next index
  return i;
}

/**
 * @brief Convert an unsigned integer to string representation
 */
static size_t _objc_printf_uinttostr(char *buf, size_t sz, size_t i,
                                     unsigned long value,
                                     enum _objc_printf_flags flags) {
  return _objc_printf_strtostr(buf, sz, i, "TODO (unsigned)");
}

/**
 * @brief Convert a signed integer to string representation
 */
static size_t _objc_printf_inttostr(char *buf, size_t sz, size_t i, long value,
                                    enum _objc_printf_flags flags) {
  // Print a minus sign for negative values
  if (value < 0) {
    i = _objc_printf_chtostr(buf, sz, i, '-');
    value = -value;
  }
  return _objc_printf_uinttostr(buf, sz, i, value, flags);
}

/**
 * @brief Convert an Objective-C object to string representation by calling
 * description
 */
static size_t _objc_printf_objtostr(char *buf, size_t sz, size_t i, id object) {
  if (object == nil) {
    return _objc_printf_strtostr(buf, sz, i, _objc_printf_nil);
  }
  return _objc_printf_strtostr(buf, sz, i, "TODO (@)");
}

/**
 * @brief Format a string into the output buffer, replacing
 * formatting directives with a string representation of the
 * provided arguments.
 */
static size_t _objc_printf(char *buf, size_t sz, const char *format,
                           va_list va) {
  assert(format);
  assert(buf == NULL || sz > 0);

  size_t i = 0;
  while (*format) {
    if (*format != '%') {
      i = _objc_printf_chtostr(buf, sz, i, *format);
      format++;
      continue;
    } else {
      format++;
      if (*format == '\0') {
        i = _objc_printf_chtostr(buf, sz, i, '%');
        continue;
      }
    }

    // evaluate flags
    enum _objc_printf_flags flags = OBJC_PRINTF_NONE;
    switch (*format) {
    case 'l':
      flags |= OBJC_PRINTF_LONG;
      if (*++format == '\0') {
        i = _objc_printf_strtostr(buf, sz, i, "%l");
        continue;
      }
      break;
    }

    // evaluate specifier
    switch (*format) {
    case 's':
      // cstring
      i = _objc_printf_strtostr(buf, sz, i, va_arg(va, const char *));
      format++;
      break;
    case 'd':
      // signed integer
      if (flags & OBJC_PRINTF_LONG) {
        i = _objc_printf_inttostr(buf, sz, i, va_arg(va, long), flags);
      } else {
        i = _objc_printf_inttostr(buf, sz, i, va_arg(va, int), flags);
      }
      format++;
      break;
    case 'u':
      // unsigned integer
      if (flags & OBJC_PRINTF_LONG) {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned long),
                                   flags);
      } else {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned int), flags);
      }
      format++;
      break;
    case 'X':
      // upper case unsigned hexadecimal value
      flags |= OBJC_PRINTF_UPPER;
      [[fallthrough]];
    case 'x':
      // unsigned hexadecimal value
      if (flags & OBJC_PRINTF_LONG) {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned long),
                                   flags | OBJC_PRINTF_HEX);
      } else {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned int),
                                   flags | OBJC_PRINTF_HEX);
      }
      format++;
      break;
    case 'b':
      // unsigned binary value
      if (flags & OBJC_PRINTF_LONG) {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned long),
                                   flags | OBJC_PRINTF_BIN);
      } else {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned int),
                                   flags | OBJC_PRINTF_BIN);
      }
      format++;
      break;
    case '@':
      i = _objc_printf_objtostr(buf, sz, i, va_arg(va, id));
      format++;
      break;
    case '%':
      // quote a %
      i = _objc_printf_chtostr(buf, sz, i, '%');
      format++;
      break;
    default:
      assert(*format == 0);
    }
  }

  // Always terminate the string
  if (buf) {
    size_t k = MIN_(i, sz - 1);
    buf[k] = '\0';
  }

  // return number of characters written
  return i;
}

/**
 * @brief Public function to format a string with variable arguments
 */
size_t objc_sprintf(char *buf, size_t sz, const char *format, ...) {
  va_list va;
  va_start(va, format);
  size_t result = _objc_printf(buf, sz, format, va);
  va_end(va);
  return result;
}

/**
 * @brief Public function to format a string with variable arguments
 */
size_t objc_vsprintf(char *buf, size_t sz, const char *format, va_list va) {
  return _objc_printf(buf, sz, format, va);
}

/**
 * @brief Public function to print a formatted string with variable arguments
 */
size_t objc_vprintf(const char *format, va_list va) {
  size_t sz = objc_printf(_objc_printf_buf, OBJC_PRINTF_BUF, format, va);
  if (sz < OBJC_PRINTF_BUF) {
    sys_puts(_objc_printf_buf);
    return sz;
  }

  // Buffer was too small, allocate memory and do it again
  char *buf = sys_malloc(sz + 1);
  if (buf == NULL) {
    // Out of memory, fallback to static buffer
    sys_puts(_objc_printf_buf);
    return sz;
  }

  // Format the string into the allocated buffer
  objc_vsprintf(buf, sz + 1, format, va);
  sys_puts(buf);

  // Free the allocated buffer
  sys_free(buf);
  return sz;
}

/**
 * @brief Public function to print a formatted string with variable arguments
 */
size_t objc_printf(const char *format, ...) {
  va_list va;
  va_start(va, format);
  size_t result = objc_vprintf(format, va);
  va_end(va);
  return result;
}
