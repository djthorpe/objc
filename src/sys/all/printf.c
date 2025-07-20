#include <stdarg.h>
#include <stddef.h>
#include <sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

struct sys_printf_state {
  char *buffer; //**< Buffer for formatted output
  size_t size;  //**< Size of the buffer, including null terminator
  size_t pos;   //**< Current position in the buffer
  size_t (*putch)(struct sys_printf_state *state,
                  char ch); //**< Function to output a character
};

static const char *_nullstr = "<nil>"; // Placeholder for NULL strings

typedef enum {
  SYS_PRINTF_FLAG_FORMAT = 1 << 0, //**< Flag for format specifiers
  SYS_PRINTF_FLAG_SIZET = 1 << 1,  //**< Flag for size_t specifier
  SYS_PRINTF_FLAG_LONG = 1 << 2,   //**< Flag for long integer specifier
  SYS_PRINTF_FLAG_LEFT = 1 << 3,   //**< Flag for left alignment
  SYS_PRINTF_FLAG_SIGN = 1 << 4,   //**< Flag to force numeric sign
  SYS_PRINTF_FLAG_PREFIX = 1 << 5, //**< Flag to force 0, 0x or 0b prefix
  SYS_PRINTF_FLAG_PAD = 1 << 6,    //**< Flag for zero-padding
  SYS_PRINTF_FLAG_NEG = 1 << 7,    //**< Flag to process as negative number
  SYS_PRINTF_FLAG_HEX = 1 << 8,    //**< Flag for hexadecimal output
  SYS_PRINTF_FLAG_BIN = 1 << 9,    //**< Flag for binary output
  SYS_PRINTF_FLAG_OCT = 1 << 10,   //**< Flag for octal output
  SYS_PRINTF_FLAG_UPPER = 1 << 11  //**< Flag for uppercase hexadecimal output
} sys_printf_flags_t;

///////////////////////////////////////////////////////////////////////////////
// PRIVATE  METHODS

size_t _sys_printf_putc(struct sys_printf_state *state,
                        sys_printf_flags_t flags, va_list *va) {
  (void)flags;               // Unused in this implementation
  int ch = va_arg(*va, int); // char is promoted to int in variadic functions
  return state->putch(state, (char)ch);
}

size_t _sys_printf_puts(struct sys_printf_state *state,
                        sys_printf_flags_t flags, va_list *va) {
  (void)flags; // Unused in this implementation
  const char *str = va_arg(*va, const char *);
  if (str == NULL) {
    str = _nullstr; // Use placeholder for NULL strings
  }
  size_t i = 0;
  while (*str) {
    i += state->putch(state, *str++);
  }
  return i;
}

static inline char _sys_printf_putuv_digit(unsigned long num,
                                           sys_printf_flags_t flags) {
  if (flags & SYS_PRINTF_FLAG_UPPER) {
    return (num < 10) ? '0' + num : 'A' + (num - 10);
  } else {
    return (num < 10) ? '0' + num : 'a' + (num - 10);
  }
}

size_t _sys_printf_putuv(struct sys_printf_state *state,
                         sys_printf_flags_t flags, unsigned long num) {
  char buffer[64];         // Enough to hold 64-bit unsigned long in binary
  char *ptr = &buffer[63]; // Start from the end of the buffer
  *ptr = '\0';             // Null terminate

  int base = 10; // Default to decimal
  if (flags & SYS_PRINTF_FLAG_HEX) {
    base = 16; // Hexadecimal
  } else if (flags & SYS_PRINTF_FLAG_BIN) {
    base = 2; // Binary
  } else if (flags & SYS_PRINTF_FLAG_OCT) {
    base = 8; // Octal
  }

  // Convert number to string (in reverse order)
  do {
    *--ptr = _sys_printf_putuv_digit(num % base, flags);
    num /= base;
  } while (num > 0);

  // If prefix add 0x, 0b or 0
  if (flags & SYS_PRINTF_FLAG_PREFIX) {
    if (base == 16) {
      if (flags & SYS_PRINTF_FLAG_UPPER) {
        *--ptr = 'X'; // Add 'X' for uppercase hexadecimal
      } else {
        *--ptr = 'x'; // Add 'x' for lowercase hexadecimal
      }
      *--ptr = '0'; // Add '0' for hexadecimal prefix
    } else if (base == 2) {
      *--ptr = 'b'; // Add 'b' for binary
      *--ptr = '0'; // Add '0' for binary prefix
    } else if (base == 8) {
      *--ptr = '0'; // Add '0' for octal prefix
    }
  }

  // Add sign prefix AFTER number conversion
  if (flags & SYS_PRINTF_FLAG_NEG) {
    *--ptr = '-'; // Add negative sign
  } else if (flags & SYS_PRINTF_FLAG_SIGN) {
    *--ptr = '+'; // Add positive sign
  }

  size_t i = 0;
  while (*ptr) {
    i += state->putch(state, *ptr++);
  }
  return i;
}

size_t _sys_printf_putu(struct sys_printf_state *state,
                        sys_printf_flags_t flags, va_list *va) {
  unsigned long num;

  if (flags & SYS_PRINTF_FLAG_SIZET) {
    num = (unsigned long)va_arg(*va, size_t);
  } else if (flags & SYS_PRINTF_FLAG_LONG) {
    num = va_arg(*va, unsigned long);
  } else {
    num = (unsigned long)va_arg(*va, unsigned int);
  }

  return _sys_printf_putuv(state, flags, num);
}

size_t _sys_printf_putd(struct sys_printf_state *state,
                        sys_printf_flags_t flags, va_list *va) {
  unsigned long abs_num;

  if (flags & SYS_PRINTF_FLAG_SIZET) {
    // Handle size_t integers - size_t is always unsigned
    // Even for %zd, treat size_t as unsigned (no negative handling)
    size_t num = va_arg(*va, size_t);
    abs_num = (unsigned long)num;
  } else if (flags & SYS_PRINTF_FLAG_LONG) {
    long num = va_arg(*va, long);
    if (num < 0) {
      flags |= SYS_PRINTF_FLAG_NEG; // Set negative flag
      // Safe negation to avoid overflow with LONG_MIN
      abs_num = (unsigned long)(-(num + 1)) + 1;
    } else {
      abs_num = (unsigned long)num;
    }
  } else {
    int num = va_arg(*va, int);
    if (num < 0) {
      flags |= SYS_PRINTF_FLAG_NEG; // Set negative flag
      // Safe negation to avoid overflow with INT_MIN
      abs_num = (unsigned long)((unsigned int)(-(num + 1)) + 1);
    } else {
      abs_num = (unsigned long)num;
    }
  }
  return _sys_printf_putuv(state, flags, abs_num);
}

size_t _sys_printf_put(struct sys_printf_state *state, char spec, va_list *va,
                       sys_printf_flags_t flags) {
  switch (spec) {
  case 'c':
    return _sys_printf_putc(state, flags, va); // Handle character output
  case 's':
    return _sys_printf_puts(state, flags, va); // Handle string output
  case 'd':
    return _sys_printf_putd(state, flags, va); // Handle signed decimal output
  case 'u':
    return _sys_printf_putu(state, flags,
                            va); // Handle unsigned decimal output
  case 'x':
  case 'X':
    flags |= SYS_PRINTF_FLAG_HEX; // Set hexadecimal flag
    if (spec == 'X') {
      flags |= SYS_PRINTF_FLAG_UPPER; // Set uppercase flag for 'X'
    }
    return _sys_printf_putu(state, flags, va);
  case 'b':
    flags |= SYS_PRINTF_FLAG_BIN; // Set binary flag
    return _sys_printf_putu(state, flags, va);
  case 'o':
    flags |= SYS_PRINTF_FLAG_OCT; // Set octal flag
    return _sys_printf_putu(state, flags, va);
  default:
    sys_panicf("Unsupported format specifier: %c", spec);
    return 0;
  }
}

size_t _sys_vprintf(struct sys_printf_state *state, const char *format,
                    va_list va) {

  state->pos = 0; // Set the length
  while (*format) {
    char ch = *format++;

    // Handle non-formatting
    if (ch != '%') {
      state->pos += state->putch(state, ch);
      continue;
    }

    // Check for %% (escaped percent)
    if (*format == '%') {
      state->pos += state->putch(state, '%');
      format++; // Skip the second %
      continue;
    }

    // Parse format specifier with flags
    sys_printf_flags_t flags = 0;

    // Parse flags
    while (*format) {
      switch (*format) {
      case '-':
        flags |= SYS_PRINTF_FLAG_LEFT;
        format++;
        break;
      case '+':
        flags |= SYS_PRINTF_FLAG_SIGN;
        format++;
        break;
      case '0':
        flags |= SYS_PRINTF_FLAG_PAD;
        format++;
        break;
      case '#':
        flags |= SYS_PRINTF_FLAG_PREFIX;
        format++;
        break;
      default:
        goto parse_modifiers; // Exit flag parsing loop
      }
    }

  parse_modifiers:
    while (*format) {
      switch (*format) {
      case 'l':
        flags |= SYS_PRINTF_FLAG_LONG;
        format++;
        break;
      case 'z':
        flags |= SYS_PRINTF_FLAG_SIZET;
        format++;
        break;
      default:
        goto handle_specifier;
      }
    }

  handle_specifier:
    if (*format) {
      char spec = *format++;
      state->pos += _sys_printf_put(state, spec, &va, flags);
    } else {
      // If we reach here, it means we had a '%' at the end without a
      // specifier
      state->pos += state->putch(state, '%');
    }
  }

  return state->pos;
}

size_t _sys_printf_putch(struct sys_printf_state *state, char ch) {
  (void)state; // Unused in this implementation
  sys_putch(ch);
  return 1;
}

size_t _sys_sprintf_putch(struct sys_printf_state *state, char ch) {
  // Write character at current position and increment immediately
  if (state->buffer && state->pos < state->size - 1) {
    state->buffer[state->pos] = ch;
    state->pos++; // Increment position immediately after writing
    return 0;     // Return 0 so main loop doesn't double-increment
  }
  // Buffer full or no buffer - still increment for total length calculation
  state->pos++;
  return 0; // Return 0 so main loop doesn't double-increment
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

size_t sys_vprintf(const char *format, va_list args) {
  struct sys_printf_state state = {.putch = _sys_printf_putch};
  return _sys_vprintf(&state, format, args);
}

size_t sys_printf(const char *format, ...) {
  va_list va;
  va_start(va, format);
  size_t len = sys_vprintf(format, va);
  va_end(va);
  return len;
}

size_t sys_sprintf(char *buf, size_t sz, const char *format, ...) {
  va_list va;
  va_start(va, format);
  struct sys_printf_state state = {
      .putch = _sys_sprintf_putch, .buffer = buf, .size = sz};
  size_t len = _sys_vprintf(&state, format, va);
  va_end(va);

  // Null terminate the buffer
  if (buf && sz > 0) {
    buf[state.pos < sz - 1 ? state.pos : sz - 1] = '\0';
  }

  return len;
}
