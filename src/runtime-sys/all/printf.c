#include <runtime-sys/sys.h>
#include <stdarg.h>
#include <stddef.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

// Global mutex for thread-safe printf operations
static sys_mutex_t printf_mutex;

///////////////////////////////////////////////////////////////////////////////
// TYPES

typedef enum {
  SYS_PRINTF_FLAG_SIZET = 1 << 0,  /**< Flag for size_t specifier */
  SYS_PRINTF_FLAG_LONG = 1 << 1,   /**< Flag for long integer specifier */
  SYS_PRINTF_FLAG_LEFT = 1 << 2,   /**< Flag for left alignment */
  SYS_PRINTF_FLAG_SIGN = 1 << 3,   /**< Flag to force numeric sign */
  SYS_PRINTF_FLAG_PREFIX = 1 << 4, /**< Flag to force 0, 0x or 0b prefix */
  SYS_PRINTF_FLAG_PAD = 1 << 5,    /**< Flag for zero-padding */
  SYS_PRINTF_FLAG_NEG = 1 << 6,    /**< Flag to process as negative number */
  SYS_PRINTF_FLAG_HEX = 1 << 7,    /**< Flag for hexadecimal output */
  SYS_PRINTF_FLAG_BIN = 1 << 8,    /**< Flag for binary output */
  SYS_PRINTF_FLAG_OCT = 1 << 9,    /**< Flag for octal output */
  SYS_PRINTF_FLAG_UPPER = 1 << 10  /**< Flag for uppercase hexadecimal output */
} sys_printf_flags_t;

struct sys_printf_state {
  char *buffer; /**< Buffer for formatted output */
  size_t size;  /**< Size of the buffer, including null terminator */
  size_t pos;   /**< Current position in the buffer */
  size_t (*putch)(struct sys_printf_state *state,
                  char ch); /**< Function to output a character */
  size_t width;             /**< Width specifier for padding */
  sys_printf_flags_t flags; /**< Current format flags */
};

static const char *_nullstr = "<nil>"; // Placeholder for NULL strings

///////////////////////////////////////////////////////////////////////////////
// PRIVATE  METHODS

size_t _sys_printf_putc(struct sys_printf_state *state, va_list *va) {
  int ch = va_arg(*va, int); // char is promoted to int in variadic functions

  size_t total_chars = 0;
  size_t padding = (state->width > 1) ? state->width - 1 : 0;

  // Right-aligned by default (left padding)
  if (!(state->flags & SYS_PRINTF_FLAG_LEFT)) {
    for (size_t i = 0; i < padding; i++) {
      total_chars += state->putch(state, ' ');
    }
  }

  // Output the character
  total_chars += state->putch(state, (char)ch);

  // Left-aligned (right padding)
  if (state->flags & SYS_PRINTF_FLAG_LEFT) {
    for (size_t i = 0; i < padding; i++) {
      total_chars += state->putch(state, ' ');
    }
  }

  return total_chars;
}

size_t _sys_printf_puts(struct sys_printf_state *state, va_list *va) {
  const char *str = va_arg(*va, const char *);
  if (str == NULL) {
    str = _nullstr; // Use placeholder for NULL strings
  }

  size_t len = 0;
  const char *ptr = str;
  while (*ptr) {
    len++;
    ptr++;
  }

  size_t total_chars = 0;
  size_t padding = (state->width > len) ? state->width - len : 0;

  // Right-aligned by default (left padding)
  if (!(state->flags & SYS_PRINTF_FLAG_LEFT)) {
    for (size_t i = 0; i < padding; i++) {
      total_chars += state->putch(state, ' ');
    }
  }

  // Output the string
  while (*str) {
    total_chars += state->putch(state, *str++);
  }

  // Left-aligned (right padding)
  if (state->flags & SYS_PRINTF_FLAG_LEFT) {
    for (size_t i = 0; i < padding; i++) {
      total_chars += state->putch(state, ' ');
    }
  }

  return total_chars;
}

static inline char _sys_printf_putuv_digit(unsigned long num,
                                           sys_printf_flags_t flags) {
  if (flags & SYS_PRINTF_FLAG_UPPER) {
    return (num < 10) ? '0' + num : 'A' + (num - 10);
  } else {
    return (num < 10) ? '0' + num : 'a' + (num - 10);
  }
}

size_t _sys_printf_putuv(struct sys_printf_state *state, unsigned long num) {
  char buffer[64];         // Enough to hold 64-bit unsigned long in binary
  char *ptr = &buffer[63]; // Start from the end of the buffer
  *ptr = '\0';             // Null terminate

  int base = 10; // Default to decimal
  if (state->flags & SYS_PRINTF_FLAG_HEX) {
    base = 16; // Hexadecimal
  } else if (state->flags & SYS_PRINTF_FLAG_BIN) {
    base = 2; // Binary
  } else if (state->flags & SYS_PRINTF_FLAG_OCT) {
    base = 8; // Octal
  }

  // Convert number to string (in reverse order)
  size_t digits_written = 0;
  do {
    *--ptr = _sys_printf_putuv_digit(num % base, state->flags);
    num /= base;
    digits_written++;
  } while (num > 0);

  // Count prefix length for zero padding calculation
  size_t prefix_len = 0;
  if (state->flags & SYS_PRINTF_FLAG_NEG ||
      state->flags & SYS_PRINTF_FLAG_SIGN) {
    prefix_len++; // For +/- sign
  }
  if (state->flags & SYS_PRINTF_FLAG_PREFIX) {
    if (base == 16 || base == 2) {
      prefix_len += 2; // For 0x, 0X, 0b
    } else if (base == 8) {
      prefix_len += 1; // For 0
    }
  }

  // Zero-pad the number part if PAD flag is set and width is specified
  if ((state->flags & SYS_PRINTF_FLAG_PAD) &&
      state->width > (digits_written + prefix_len)) {
    size_t zero_pad_count = state->width - digits_written - prefix_len;
    for (size_t i = 0; i < zero_pad_count; i++) {
      *--ptr = '0';
    }
  }

  // If prefix add 0x, 0b or 0
  if (state->flags & SYS_PRINTF_FLAG_PREFIX) {
    if (base == 16) {
      if (state->flags & SYS_PRINTF_FLAG_UPPER) {
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
  if (state->flags & SYS_PRINTF_FLAG_NEG) {
    *--ptr = '-'; // Add negative sign
  } else if (state->flags & SYS_PRINTF_FLAG_SIGN) {
    *--ptr = '+'; // Add positive sign
  }

  // Calculate string length for width padding
  size_t str_len = 0;
  char *temp_ptr = ptr;
  while (*temp_ptr) {
    str_len++;
    temp_ptr++;
  }

  size_t total_chars = 0;
  size_t padding = (state->width > str_len) ? state->width - str_len : 0;

  // Right-aligned by default (left padding)
  if (!(state->flags & SYS_PRINTF_FLAG_LEFT) && padding > 0) {
    for (size_t i = 0; i < padding; i++) {
      total_chars += state->putch(state, ' ');
    }
  }

  // Output the formatted number
  while (*ptr) {
    total_chars += state->putch(state, *ptr++);
  }

  // Left-aligned (right padding with spaces)
  if ((state->flags & SYS_PRINTF_FLAG_LEFT) && padding > 0) {
    for (size_t i = 0; i < padding; i++) {
      total_chars += state->putch(state, ' ');
    }
  }

  return total_chars;
}

size_t _sys_printf_putu(struct sys_printf_state *state, va_list *va) {
  unsigned long num;

  if (state->flags & SYS_PRINTF_FLAG_SIZET) {
    num = (unsigned long)va_arg(*va, size_t);
  } else if (state->flags & SYS_PRINTF_FLAG_LONG) {
    num = va_arg(*va, unsigned long);
  } else {
    num = (unsigned long)va_arg(*va, unsigned int);
  }

  return _sys_printf_putuv(state, num);
}

size_t _sys_printf_putd(struct sys_printf_state *state, va_list *va) {
  unsigned long abs_num;

  if (state->flags & SYS_PRINTF_FLAG_SIZET) {
    // Handle size_t integers - size_t is always unsigned
    // Even for %zd, treat size_t as unsigned (no negative handling)
    size_t num = va_arg(*va, size_t);
    abs_num = (unsigned long)num;
  } else if (state->flags & SYS_PRINTF_FLAG_LONG) {
    long num = va_arg(*va, long);
    if (num < 0) {
      state->flags |= SYS_PRINTF_FLAG_NEG; // Set negative flag
      // Safe negation to avoid overflow with LONG_MIN
      abs_num = (unsigned long)(-(num + 1)) + 1;
    } else {
      abs_num = (unsigned long)num;
    }
  } else {
    int num = va_arg(*va, int);
    if (num < 0) {
      state->flags |= SYS_PRINTF_FLAG_NEG; // Set negative flag
      // Safe negation to avoid overflow with INT_MIN
      abs_num = (unsigned long)((unsigned int)(-(num + 1)) + 1);
    } else {
      abs_num = (unsigned long)num;
    }
  }
  return _sys_printf_putuv(state, abs_num);
}

size_t _sys_printf_put(struct sys_printf_state *state, char spec, va_list *va) {
  switch (spec) {
  case 'c':
    return _sys_printf_putc(state, va); // Handle character output
  case 's':
    return _sys_printf_puts(state, va); // Handle string output
  case 'd':
    return _sys_printf_putd(state, va); // Handle signed decimal output
  case 'u':
    return _sys_printf_putu(state, va); // Handle unsigned decimal output
  case 'x':
  case 'X':
    state->flags |= SYS_PRINTF_FLAG_HEX; // Set hexadecimal flag
    if (spec == 'X') {
      state->flags |= SYS_PRINTF_FLAG_UPPER; // Set uppercase flag for 'X'
    }
    return _sys_printf_putu(state, va);
  case 'b':
    state->flags |= SYS_PRINTF_FLAG_BIN; // Set binary flag
    return _sys_printf_putu(state, va);
  case 'o':
    state->flags |= SYS_PRINTF_FLAG_OCT; // Set octal flag
    return _sys_printf_putu(state, va);
  case 'p': {
    // Handle pointer output with proper padding
    uintptr_t ptr_value = va_arg(*va, uintptr_t);
    state->flags |=
        SYS_PRINTF_FLAG_HEX | SYS_PRINTF_FLAG_PREFIX; // Set hex with 0x prefix
    const size_t ptr_hex_digits = sizeof(void *) * 2; // 2 hex digits per byte

    // For pointers, set width to hex digits + prefix length to get full padding
    size_t saved_width = state->width;
    state->width = ptr_hex_digits + 2;   // +2 for "0x" prefix
    state->flags |= SYS_PRINTF_FLAG_PAD; // Force zero padding for pointers

    size_t result = _sys_printf_putuv(state, (unsigned long)ptr_value);

    state->width = saved_width; // Restore original width
    return result;
  }
  default:
    sys_panicf("Unsupported format specifier: %c", spec);
    return 0;
  }
}

size_t _sys_vprintf(struct sys_printf_state *state, const char *format,
                    va_list *va) {

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
    state->flags = 0; // Reset flags for each format specifier
    state->width = 0; // Reset width for each format specifier

    // Parse flags
    while (*format) {
      switch (*format) {
      case '-':
        state->flags |= SYS_PRINTF_FLAG_LEFT;
        format++;
        break;
      case '+':
        state->flags |= SYS_PRINTF_FLAG_SIGN;
        format++;
        break;
      case '0':
        state->flags |= SYS_PRINTF_FLAG_PAD;
        format++;
        break;
      case '#':
        state->flags |= SYS_PRINTF_FLAG_PREFIX;
        format++;
        break;
      default:
        goto parse_width; // Exit flag parsing loop
      }
    }

  parse_width:
    // Parse width specifier (numeric digits)
    while (*format >= '0' && *format <= '9') {
      state->width = state->width * 10 + (*format - '0');
      format++;
    }

    while (*format) {
      switch (*format) {
      case 'l':
        state->flags |= SYS_PRINTF_FLAG_LONG;
        format++;
        break;
      case 'z':
        state->flags |= SYS_PRINTF_FLAG_SIZET;
        format++;
        break;
      default:
        goto handle_specifier;
      }
    }

  handle_specifier:
    if (*format) {
      char spec = *format++;
      state->pos += _sys_printf_put(state, spec, va);
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
  if (state->buffer && state->size > 0 && state->pos < state->size - 1) {
    state->buffer[state->pos] = ch;
    state->pos++; // Increment position immediately after writing
    return 0;     // Return 0 so main loop doesn't double-increment
  }
  // Buffer full or no buffer - still increment for total length calculation
  state->pos++;
  return 0; // Return 0 so main loop doesn't double-increment
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

void _sys_printf_init(void) { printf_mutex = sys_mutex_init(); }

void _sys_printf_finalize(void) { sys_mutex_finalize(&printf_mutex); }

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

size_t sys_vprintf(const char *format, va_list args) {
  //  sys_mutex_lock(&printf_mutex);
  struct sys_printf_state state = {.putch = _sys_printf_putch};
  va_list args_copy;
  va_copy(args_copy, args);
  size_t len = _sys_vprintf(&state, format, &args_copy);
  va_end(args_copy);
  //  sys_mutex_unlock(&printf_mutex);
  return len;
}

size_t sys_vsprintf(char *buf, size_t sz, const char *format, va_list args) {
  struct sys_printf_state state = {
      .putch = _sys_sprintf_putch, .buffer = buf, .size = sz};
  va_list args_copy;
  va_copy(args_copy, args);
  size_t len = _sys_vprintf(&state, format, &args_copy);
  va_end(args_copy);

  // Null terminate the buffer
  if (buf && sz > 0) {
    buf[state.pos < sz - 1 ? state.pos : sz - 1] = '\0';
  }

  return len;
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
  va_list va_copy;
  va_copy(va_copy, va);
  size_t len = _sys_vprintf(&state, format, &va_copy);
  va_end(va_copy);
  va_end(va);

  // Null terminate the buffer
  if (buf && sz > 0) {
    buf[state.pos < sz - 1 ? state.pos : sz - 1] = '\0';
  }

  return len;
}
