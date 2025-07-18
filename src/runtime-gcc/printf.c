#include <objc/objc.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/sys.h>

#ifndef MIN_
#define MIN_(a, b) ((a) < (b) ? (a) : (b))
#endif

enum _objc_printf_flags {
  OBJC_PRINTF_NONE = 0,
  OBJC_PRINTF_LONG = 1 << 0,
  OBJC_PRINTF_HEX = 1 << 1,
  OBJC_PRINTF_BIN = 1 << 2,
  OBJC_PRINTF_UPPER = 1 << 3,
  OBJC_PRINTF_NEG = 1 << 4
};

#define OBJC_PRINTF_BUF 256
static char _objc_printf_buf[OBJC_PRINTF_BUF];
static const char *_objc_printf_nil = "<nil>";

/**
 * @brief Append a character into the buffer
 */
static inline size_t _objc_printf_chtostr(char *buf, size_t sz, size_t i,
                                          const char ch) {
  objc_assert(buf == NULL || sz > 0);
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
  if (str == NULL) {
    str = _objc_printf_nil;
  }
  while (*str) {
    printf("str=%s, i=%zu, sz=%zu\n", str, i, sz);
    i = _objc_printf_chtostr(buf, sz, i, *str++);
  }

  // Return next index
  return i;
}

/* @brief Return digit for a given value
 */
static inline char _objc_printf_digit(unsigned long value, int base,
                                      bool upper) {
  char c = (char)(value % base);
  return (c < 10) ? c + '0' : c - 10 + (upper ? 'A' : 'a');
}

/**
 * @brief Convert an unsigned integer to string representation
 */
static size_t _objc_printf_uinttostr(char *buf, size_t sz, size_t i,
                                     unsigned long value,
                                     enum _objc_printf_flags flags) {
  // Zero-value shortcut
  if (value == 0) {
    return _objc_printf_chtostr(buf, sz, i, '0');
  }

  // output the number, but in reverse order
  int base = 10;
  if (flags & OBJC_PRINTF_HEX) {
    base = 16;
  } else if (flags & OBJC_PRINTF_BIN) {
    base = 2;
  }

  size_t j = i;
  while (value > 0) {
    j = _objc_printf_chtostr(
        buf, sz, j, _objc_printf_digit(value, base, flags & OBJC_PRINTF_UPPER));
    value /= base;
  }

  // append the negative sign
  if (flags & OBJC_PRINTF_NEG) {
    j = _objc_printf_chtostr(buf, sz, j, '-');
  }

  // TODO: pad with spaces if needed

  // Reverse the number (only if we have a buffer)
  if (buf != NULL) {
    size_t start = i;
    size_t end = j - 1;
    while (start < end) {
      char temp = buf[start];
      buf[start] = buf[end];
      buf[end] = temp;
      start++;
      end--;
    }
  }
  return j;
}

/**
 * @brief Convert a signed integer to string representation
 */
static size_t _objc_printf_inttostr(char *buf, size_t sz, size_t i, long value,
                                    enum _objc_printf_flags flags) {
  if (value < 0) {
    return _objc_printf_uinttostr(buf, sz, i, -value, flags | OBJC_PRINTF_NEG);
  } else {
    return _objc_printf_uinttostr(buf, sz, i, value, flags);
  }
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
static size_t _objc_vsprintf(char *buf, size_t sz, const char *format,
                             va_list va) {
  objc_assert(format);
  objc_assert(buf == NULL || sz > 0);

  size_t i = 0;
  while (*format) {
    char ch = *format++;
    if (ch != '%') {
      i = _objc_printf_chtostr(buf, sz, i, ch);
      continue;
    }

    // evaluate modifier flags (currently only 'l' is supported)
    enum _objc_printf_flags flags = OBJC_PRINTF_NONE;
    ch = *format;
    switch (ch) {
    case 'l':
      flags |= OBJC_PRINTF_LONG;
      format++;
    case '\0':
      i = _objc_printf_chtostr(buf, sz, i, '%');
      continue;
    }

    // evaluate specifier
    ch = *format++;
    switch (ch) {
    case 's':
      // cstring
      i = _objc_printf_strtostr(buf, sz, i, va_arg(va, const char *));
      break;
    case 'd':
      // signed integer
      if (flags & OBJC_PRINTF_LONG) {
        i = _objc_printf_inttostr(buf, sz, i, va_arg(va, long), flags);
      } else {
        i = _objc_printf_inttostr(buf, sz, i, va_arg(va, int), flags);
      }
      break;
    case 'u':
      // unsigned integer
      if (flags & OBJC_PRINTF_LONG) {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned long),
                                   flags);
      } else {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned int), flags);
      }
      break;
    case 'X':
      // upper case unsigned hexadecimal value
      flags |= OBJC_PRINTF_UPPER;
      // fall through
    case 'x':
      // unsigned hexadecimal value
      if (flags & OBJC_PRINTF_LONG) {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned long),
                                   flags | OBJC_PRINTF_HEX);
      } else {
        i = _objc_printf_uinttostr(buf, sz, i, va_arg(va, unsigned int),
                                   flags | OBJC_PRINTF_HEX);
      }
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
      break;
    case '@':
      i = _objc_printf_objtostr(buf, sz, i, va_arg(va, id));
      break;
    case '%':
      i = _objc_printf_chtostr(buf, sz, i, '%');
      break;
    case '\0':
      i = _objc_printf_chtostr(buf, sz, i, '%');
      break;
    default:
      i = _objc_printf_chtostr(buf, sz, i, '%');
      break;
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
  size_t result = _objc_vsprintf(buf, sz, format, va);
  va_end(va);
  return result;
}

/**
 * @brief Public function to format a string with variable arguments
 */
size_t objc_vsprintf(char *buf, size_t sz, const char *format, va_list va) {
  return _objc_vsprintf(buf, sz, format, va);
}

/**
 * @brief Public function to print a formatted string with variable arguments
 */
size_t objc_vprintf(const char *format, va_list va) {
  size_t sz = objc_vsprintf(_objc_printf_buf, OBJC_PRINTF_BUF, format, va);
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

  // Free the allocated buffer, return the size
  sys_free(buf);
  return sz;
}

/**
 * @brief Public function to print a formatted string with variable arguments
 */
size_t objc_printf_old(const char *format, ...) {
  va_list va;
  va_start(va, format);
  size_t result = objc_vprintf(format, va);
  va_end(va);
  return result;
}
