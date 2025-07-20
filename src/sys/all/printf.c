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
} sys_printf_flags_t;

///////////////////////////////////////////////////////////////////////////////
// PRIVATE  METHODS

size_t _sys_printf_handle_specifier(struct sys_printf_state *state, char spec,
                                    va_list va) {
  size_t i = 0; // Length of the output

  switch (spec) {
  case 's': {
    const char *str = va_arg(va, const char *);
    if (str == NULL) {
      str = _nullstr; // Use placeholder for NULL strings
    }
    while (*str) {
      i += state->putch(state, *str++);
    }
    break;
  }
  case 'd': {
    int num = va_arg(va, int);
    i += state->putch(state, (char)(num + '0')); // Simplified for single digit
    break;
  }
  // Add more cases for other specifiers as needed
  default:
    i += state->putch(state, spec); // Just output the specifier as is
  }
  return i;
}

size_t _sys_vprintf(struct sys_printf_state *state, const char *format,
                    va_list va) {

  state->pos = 0; // Set the length
  while (*format) {
    char ch = *format++;

    // Handle non-formatting and % at end of string
    if (ch != '%') {
      state->pos += state->putch(state, ch);
      continue;
    }
    if (*format == '\0') {
      state->pos += state->putch(state, '%');
      break;
    }

    // Parse format specifier with flags
    sys_printf_flags_t flags = 0;

    // Parse flags and modifiers
    while (*format) {
      if (*format == 'l') {
        flags |= SYS_PRINTF_FLAG_LONG;
        format++;
      } else if (*format == 'z') {
        flags |= SYS_PRINTF_FLAG_SIZET;
        format++;
      } else {
        break; // Found the actual specifier
      }
    }

    // Handle the actual specifier
    if (*format) {
      char spec = *format++;
      if (spec == '%') {
        state->pos += state->putch(state, '%');
      } else {
        state->pos += _sys_printf_handle_specifier(state, spec, va);
      }
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
  if (state->buffer && state->pos < state->size - 1) {
    state->buffer[state->pos] = ch;
  }
  return 1;
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
